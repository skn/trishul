#include "context-analysis.h"
#include "decompile.h"
#include "utils.h"
#include "lookup.h"
/**********************************************************************************************************************/
//#define TRISHUL_EXCEPTION_EXCLUDE_RUNTIME

#define RENDER_FINISHED

//#define DEBUG_PRINTS
#ifdef DEBUG_PRINTS
	#define dbg_printf printf
#else
	#define dbg_printf(x,...)
#endif


enum { PC_INVALID = 0xFFFFFFFF };


typedef struct BasicBlockList
{
	BasicBlock	*first,
				*last;
} BasicBlockList;



typedef struct BasicBlockAlloc
{
	int						used;
	struct BasicBlockAlloc	*next;
	BasicBlock				basic_blocks[64];
} BasicBlockAlloc;

/* Kludge to handle allocation of small items in blocks */
#define PRIMITIVE_ALLOC_LIST_DECL1(name,Type)																		\
	typedef struct name##Alloc																						\
	{																												\
		struct name##Alloc	*next;																					\
		int					used;																					\
		Type				data[1022];																				\
	} name##Alloc;
PRIMITIVE_ALLOC_LIST_DECL1 (Bitmap, BitmapElement);
PRIMITIVE_ALLOC_LIST_DECL1 (SwitchTarget, BasicBlock *);
																													\
#define PRIMITIVE_ALLOC_LIST_DECL2(name,Type)																		\
	name##Alloc	*name##_alloc_first,																				\
				*name##_alloc_last,																					\
				*name##_alloc_ptr;

typedef struct ContextAnalysisInfo
{
	Method *method;

	/**
	 * Linked list of all basic blocks used in the control-flow graph.
	 */
	BasicBlockList list_normal;

	/**
	 * Linked list of forward blocks, sorted by pc_start.
	 * The next_normal field is used to form the list, as a BasicBlock is never in both lists
	 * at the same time.
	 */
	BasicBlockList list_forward;

	/**
	 * Linked list of all conditional blocks.
	 */
	BasicBlockList list_cond;

	/**
	 * Linked list of conditional blocks. When the bitmaps created, all conditionals
	 * are added to the list. When the graph is processed, any block whose 'finished' pointer is
	 * determined, is removed from the list.
	 */
	BasicBlockList list_cond_unfinished;

	union
	{
		/**
		 * List used during flowing of bitmaps.
		 */
		BasicBlockList list_flow;

		BasicBlockList list_temp;
	};

	/* Size of the bitmaps, in elements and bits */
	unsigned int bitmap_size_elements;
	unsigned int bitmap_size_bits;

	unsigned int branch_count;
	unsigned int block_count;

	BasicBlock *join_block;

	BasicBlockAlloc		*basic_block_alloc_first,
						*basic_block_alloc_last,
						*basic_block_alloc_ptr;

	PRIMITIVE_ALLOC_LIST_DECL2 (Bitmap, BitmapElement);
	PRIMITIVE_ALLOC_LIST_DECL2 (SwitchTarget, BasicBlock *);
} ContextAnalysisInfo;

#define LIST_ADD_FRONT(bb,name) 											\
do																			\
{ 																			\
	BasicBlock *tmp = (bb);													\
	tmp->next_##name = info->list_##name.first;								\
	if (!tmp->next_##name) info->list_##name.last = tmp;					\
	info->list_##name.first = tmp;											\
} while (0)

#define LIST_ADD_BACK(bb,name) 												\
do																			\
{ 																			\
	BasicBlock *tmp = (bb);													\
	if (info->list_##name.last) info->list_##name.last->next_##name = tmp;	\
	else info->list_##name.first = tmp;										\
	info->list_##name.last = tmp;											\
	tmp->next_##name = NULL;												\
} while (0)

#define LIST_INSERT_AFTER(bb,after,name) 									\
do																			\
{ 																			\
	BasicBlock *tmp = (bb);													\
	BasicBlock *tmp_after = (after);										\
	tmp->next_##name = tmp_after->next_##name;								\
	tmp_after->next_##name = tmp;											\
	if (!tmp->next_##name) info->list_##name.last = tmp;					\
} while (0)

#define LIST_REMOVE_FRONT(name) 											\
do																			\
{ 																			\
	info->list_##name.first = info->list_##name.first->next_##name;			\
	if (!info->list_##name.first) info->list_##name.last = NULL;			\
} while (0)

#define LIST_REMOVE_PREV(bb,prev,name) 										\
do																			\
{ 																			\
	BasicBlock *tmp = (bb);													\
	BasicBlock *tmp_prev = (prev);											\
	if (tmp_prev) tmp_prev->next_##name = tmp->next_##name;					\
	else info->list_##name.first = tmp->next_##name;						\
	if (!tmp->next_##name) info->list_##name.last = tmp_prev;				\
} while (0)


#define LIST_CLEAR(name)													\
do																			\
{ 																			\
	info->list_##name.first = NULL;											\
	info->list_##name.last = NULL;											\
} while (0)

#ifdef KAFFE_VMDEBUG
	#define DUMP_LIST(name)													\
		do {																\
			BasicBlock *tmp;												\
			printf (#name ": ");											\
			for (tmp = LIST_FRONT (name); tmp; 								\
				 tmp = LIST_NEXT (tmp, name))								\
				printf ("%s%d(%p)%s ", 										\
					tmp == LIST_FRONT(name) ? "+" : "",						\
					tmp->pc_start, tmp,										\
					tmp == LIST_BACK(name) ? "-" : "");						\
			printf ("\n");													\
		} while (0)
#else
	#define DUMP_LIST(name)
#endif

#define LIST_IS_EMPTY(name)				(info->list_##name.first == NULL)
#define LIST_SINGLE_ELEMENT(name)		(info->list_##name.first != NULL && \
										 info->list_##name.first == info->list_##name.last)
#define LIST_FRONT(name)				(info->list_##name.first)
#define LIST_BACK(name)					(info->list_##name.last)
#define LIST_NEXT(bb,name)				((bb)->next_##name)

/* Allow the next_normal field to be reused for the forward list */
#define next_forward	next_normal

/* Calculates the number of elements required to hold the specified number of bits */
#define bitmapElementCount(bits) (bits / (8 * sizeof (BitmapElement))) + ((bits % (8 * sizeof (BitmapElement))) ? 1 : 0);

/**********************************************************************************************************************/

/**
 * Allocates a BasicBlock structure.
 */
static BasicBlock *allocBasicBlock (ContextAnalysisInfo *info,
									uint pc_start, uint pc_end,
									uint pc_flow, BasicBlockType type);

/**
 * Releases all allocated BasicBlock structures and retains them for reallocation
 */
static void freeBasicBlocks (ContextAnalysisInfo *info);

/**
 * Allocates a bitmap of size bitmap_size_elements
 */
static inline BitmapElement *allocBitmap ();

/**
 * Releases all allocated bitmaps and retains them for reallocation
 */
static void freeBitmaps ();

#ifdef KAFFE_VMDEBUG
	/**
	 * Dumps the control-flow graph in graphviz format, if the method is in the list of debug methods.
	 * @param force If true, ignore the global debug settings and always dump the graph, for debug purposes.
	 */
	static void dumpGraph (ContextAnalysisInfo *info, Method *meth, int step, bool force);
#else
	#define dumpGraph(info,meth,step,force)
#endif

/**
 * Creates a new BasicBlock for a block of code covering (pc_start, pc_end)
 * If BasicBlocks have been added to the forward list in the covered range, the range is split
 * to accomodate the ranges of code.
 *
 * @param type	The type of the branch instruction at the end of the block.
 * 				If multiple BasicBlocks are created, this is assigned to the last created block.
 * @return The newly created BasicBlock. If multiple blocks are created, the last is returned.
 */
static BasicBlock *newBasicBlock (ContextAnalysisInfo *info, uint pc_start, uint pc_end,
								  uint pc_flow, BasicBlockType type);

/**
 * Signals a forward branch.
 * A BasicBlock is added to the forward list, unless a forward block at the specified pc already
 * exists. In this case, the existing BasicBlock is returned. Otherwise, the new BasicBlock is
 * returned.
 */
static BasicBlock *forwardBranch (ContextAnalysisInfo *info, uint pc_start);

/**
 * Signals a backward branch.
 * If the branch jumps into the middle of a BasicBlock, that block is split so that
 * the branch jumps into the head of a BasicBlock.
 */
static BasicBlock *backwardBranch (ContextAnalysisInfo *info, uint pc_start);

/**
 * Sets/retrieves the specified bit in the bitmap
 */
static inline void bitmapSet (ContextAnalysisInfo *info, BitmapElement *bitmap, uint bit_index);
static inline bool bitmapGet (ContextAnalysisInfo *info, BitmapElement *bitmap, uint bit_index);

static inline void runtimeBitmapSet (Method *meth, ContextBasicBlock *bitmap, uint bit_index);
static inline bool runtimeBitmapGet (Method *meth, ContextBasicBlock *bitmap, uint bit_index);

/**
 * Performs the flow of the condition-dependencies bitmap from block 'from' to block 'to'.
 * Returns true if anything changed, false otherwise.
 */
static inline bool bitmapFlow (ContextAnalysisInfo *info, BasicBlock *from, BasicBlock *to);

/**
 * Checks if 'check_block' is the finish block for any of the control-flow blocks in the 'cond' list,
 * which is the case if it is the first block in which all of the dependency bits are set.
 * If so, the control-flow block's 'finished' pointer is pointed to 'check_block' and the
 * control-flow block is removed from the 'cond' list.
 */
static inline void checkFinished (ContextAnalysisInfo *info, BasicBlock *check_block);

/**********************************************************************************************************************/
/* Helper functions for the creation of control-flow graphs */
/**********************************************************************************************************************/

static BasicBlock *newBasicBlock (ContextAnalysisInfo *info,
								  uint pc_start, uint pc_end,
								  uint pc_flow, BasicBlockType type)
{
	BasicBlock *ff = LIST_FRONT (forward);

	/* Check if a forward branch was defined into this block */
	if (ff && ff->pc_start <= pc_end)
	{
		if (ff->pc_start == pc_start)
		{
			BasicBlock *next = LIST_NEXT(ff, forward);

			/* May still have to split if the new block covers several forward branches */
			if (next && next->pc_start <= pc_end)
			{
				dbg_printf ("splitting forward 2: %d %d\n", pc_start, pc_flow);
				ff->pc_end 	= next->pc_start - 1;
				ff->type 	= BBT_NONE;

				/* Remove from forward list and add to the back of normal list */
				LIST_REMOVE_FRONT	(forward);
				LIST_ADD_BACK		(ff, normal);

				/* Split the block */
				BasicBlock *nb = newBasicBlock (info, next->pc_start, pc_end, pc_flow, type);

				/* Reassign jump/switch targets */
				nb->target	= ff->target;
				ff->target	= NULL;
				nb->cases	= ff->cases;
				ff->cases	= NULL;

				return nb;
			}
			else
			{
				dbg_printf ("using forward: %d %d\n", pc_start, pc_end);
				ff->pc_end 	= pc_end;
				ff->pc_flow = pc_flow;
				ff->type	= type;

				/* Remove from forward list and add to the back of normal list */
				LIST_REMOVE_FRONT	(forward);
				LIST_ADD_BACK		(ff, normal);
				return ff;
			}
		}
		else
		{
			dbg_printf ("splitting forward 1: %d %d %d\n", pc_start, pc_end, ff->pc_start);

			/* Split the block */
			newBasicBlock (info, pc_start, ff->pc_start - 1, PC_INVALID, BBT_NONE);

			/* This will also remove it from the forward list */
			return newBasicBlock (info, ff->pc_start, pc_end, pc_flow, type);
		}
	}

	/* No splitting required */
	dbg_printf ("! %d %d\n", pc_start, pc_end);
	BasicBlock *bb = allocBasicBlock (info, pc_start, pc_end, pc_flow, type);
	LIST_ADD_BACK (bb, normal);

	return bb;
}

/**********************************************************************************************************************/

static BasicBlock *forwardBranch (ContextAnalysisInfo *info, uint pc_start)
{
	/* Find the position to insert the new forward block in the sorted list */
	BasicBlock *cur  = LIST_FRONT (forward);
	BasicBlock *prev = NULL;
	while (cur)
	{
		if (cur->pc_start > pc_start)
		{
			break;
		}
		else
		{
			prev = cur;
			cur = LIST_NEXT(cur, forward);
		}
	}
	/* prev is now NULL, or the position after which the new block is added */

	if (prev && prev->pc_start == pc_start)
	{
		/* Don't store duplicate forwards */
		dbg_printf ("f %d dup\n", pc_start);
		return prev;
	}

	/* Add a new block to the sorted list */
	BasicBlock *bb = allocBasicBlock (info, pc_start, PC_INVALID, PC_INVALID, BBT_NONE);
	if (prev) 	LIST_INSERT_AFTER 	(bb, prev, forward);
	else		LIST_ADD_FRONT 		(bb, forward);
	dbg_printf ("stored forward: %d\n", pc_start);
	return bb;
}

/**********************************************************************************************************************/

static BasicBlock *backwardBranch (ContextAnalysisInfo *info, uint pc_start)
{
	BasicBlock *cur = LIST_FRONT(normal);
	while (cur)
	{
		if (cur->pc_start == pc_start)
		{
			/* Branch into the head of a BasicBlock. Return that BasicBlock */
			return cur;
		}
		else if (cur->pc_start < pc_start && cur->pc_end > pc_start)
		{
			/* The target BasicBlock must be split */
			BasicBlock *newBB		= allocBasicBlock (info, pc_start, cur->pc_end, cur->pc_flow, cur->type);

			/* The existing block now terminates earlier */
			cur->pc_end				= pc_start - 1;

			/* The conditional is now at the end of the new block.
			 * Adjust the type flag, target and bitmap fields
			 */
			cur->type				= BBT_NONE;
			cur->pc_flow			= PC_INVALID;

			newBB->target			= cur->target;
			cur->target				= NULL;
			newBB->cases			= cur->cases;
			cur->cases				= NULL;

			newBB->bitmap_index		= cur->bitmap_index;
			cur->bitmap_index		= (BitmapElement) -1;

			newBB->bitmap_size_bits	= cur->bitmap_size_bits;
			cur->bitmap_size_bits	= 0;

			LIST_INSERT_AFTER (newBB, cur, normal);
			return newBB;
		}
		else
		{
			cur = LIST_NEXT (cur, normal);
		}
	}

	/* A backward branch should have branched into an existing block */
	assert (false);
	return NULL;
}

/**********************************************************************************************************************/
/* Bitmap functions */
/**********************************************************************************************************************/

static inline void bitmapSet (ContextAnalysisInfo *info, BitmapElement *bitmap, uint bit_index)
{
	assert (bit_index < info->bitmap_size_bits);

	uint index = bit_index / (8 * sizeof (*bitmap));
	uint bit   = bit_index % (8 * sizeof (*bitmap));

	bitmap[index] |= 1 << bit;
}

static inline void runtimeBitmapSet (Method *meth, ContextBasicBlock *cbb, uint bit_index)
{
	assert (bit_index < meth->bitmap_size_bits);

	uint index = bit_index / (8 * sizeof (*cbb->bitmap));
	uint bit   = bit_index % (8 * sizeof (*cbb->bitmap));

	cbb->bitmap[index] |= 1 << bit;
}

/**********************************************************************************************************************/

static inline bool bitmapGet (ContextAnalysisInfo *info, BitmapElement *bitmap, uint bit_index)
{
	assert (bit_index < info->bitmap_size_bits);

	uint index = bit_index / (8 * sizeof (*bitmap));
	uint bit   = bit_index % (8 * sizeof (*bitmap));

	return ((bitmap[index] & (1 << bit)) != 0);
}


static inline bool runtimeBitmapGet (Method *meth, ContextBasicBlock *cbb, uint bit_index)
{
	assert (bit_index < meth->bitmap_size_bits);

	uint index = bit_index / (8 * sizeof (*cbb->bitmap));
	uint bit   = bit_index % (8 * sizeof (*cbb->bitmap));

	return ((cbb->bitmap[index] & (1 << bit)) != 0);
}

/**********************************************************************************************************************/

static inline bool bitmapFlow (ContextAnalysisInfo *info, BasicBlock *from, BasicBlock *to)
{
	bool changed = false;
	unsigned int i;
	for (i = 0; i < info->bitmap_size_elements; i++)
	{
		BitmapElement old = to->bitmap[i];
		to->bitmap[i] |= from->bitmap[i];

		if (old != to->bitmap[i])
		{
			changed = true;
		}
	}

	return changed;
}

/**********************************************************************************************************************/
#ifdef KAFFE_VMDEBUG

/* Simple array of debug names. Hashtables cannot be used as they use the VM's memory allocation, which is not yet
 * available when this is initialized.
 * This will do for now.
 */
static struct
{
	const char	*classname;
	const char	*methodname;
} debug_methods[16];
static unsigned int debug_methods_used 	= 0;
static bool debug_methods_flow			= false;

void trishulAddContextDebugMethod (const char *classname, const char *methodname)
{
	if (debug_methods_used >= sizeof (debug_methods) / sizeof (debug_methods[0]))
	{
		fprintf (stderr, "Too many debug methods\n");
		abort ();
	}

	debug_methods[debug_methods_used].classname 	= classname;
	debug_methods[debug_methods_used].methodname	= methodname;
	debug_methods_used++;
}

void trishulSetContextDebugMethodFlow (bool flow)
{
	debug_methods_flow = flow;
}

/**********************************************************************************************************************/

static void dumpStores (FILE *f, const StorageRecord *store)
{
	uint i;
	const StorageEntry *entry;

	/* Dump the locals */
	for (i = 0; i < sizeof (store->quick_locals) * 8; i++)
	{
		if (store->quick_locals & (1 << i))
		{
			fprintf (f, "local %u\\n", i);
		}
	}

	/* Dump the other entries */
	for (entry = store->first; entry; entry = entry->next)
	{
		switch (entry->type)
		{
		case STORE_LOCAL_VARIABLE:
			fprintf (f, "local2 %u\\n", entry->param.local.index);
			break;
		case STORE_STACK:
			fprintf (f, "stack2 @%d\\n", entry->param.stack.depth);
			break;
		case STORE_FIELD_STATIC:
			assert (entry->param.field_static.field);
			fprintf (f, "static %s.%s\\n", entry->param.field_static.field->clazz->name->data,
						entry->param.field_static.field->name->data);
			break;
		case STORE_FIELD_OBJECT:
			assert (entry->param.field_object.field);
			fprintf (f, "object %s.%s, this in %d (%d)\\n", entry->param.field_object.field->clazz->name->data,
						entry->param.field_object.field->name->data, entry->param.field_object.index_this, entry->param.field_object.pc_this);
			break;
		default:
			/* Unimplemented type */
			abort ();
		}

	}
}

/**********************************************************************************************************************/

static void dumpGraph (ContextAnalysisInfo *info, Method *meth, int step, bool force)
{
	unsigned int i;
	bool found = force;
	for (i = 0; i < debug_methods_used && !found; i++)
	{
		//printf ("COMP: %s.%s == %s.%s\n", meth->class->name->data, meth->name->data,
		//			debug_methods[i].classname, debug_methods[i].methodname);
		found = (!strcmp (meth->class->name->data, debug_methods[i].classname) &&
				 !strcmp (meth->name->data, debug_methods[i].methodname));
	}

	if (found)
	{
		BasicBlock *block;
		uint pc;
		unsigned int i;
		char buffer[1024];
		if (step < 0)
		{
			if (snprintf (buffer, sizeof (buffer), "%s.%s.graph",
						  meth->class->name->data, meth->name->data) >= (int) sizeof (buffer))
			{
				perror ("Unable to create graph file");
				abort ();
			}

		}
		else if (debug_methods_flow)
		{
			if (snprintf (buffer, sizeof (buffer), "%s.%s.%u.graph",
						  meth->class->name->data, meth->name->data, step) >= (int) sizeof (buffer))
			{
				perror ("Unable to create graph file");
				abort ();
			}
		}
		else return;

		for (i = 0; buffer[i]; i++)
			switch (buffer[i])
			{
			case '/':	buffer[i] = '.'; break;
			case '<':
			case '>':
						buffer[i] = '_'; break;
			}

		FILE *f = fopen (buffer, "wt");
		if (!f)
		{
			perror ("Unable to create graph file");
			abort ();
		}

		fprintf (f, "digraph flow {\n");
		for (block = LIST_FRONT (normal); block; block = LIST_NEXT (block, normal))
		{
			fprintf (f, "\tblock%p [shape=record label=\"{",block);
			if (BBT_IS_CATCH (block->type))
			{
				fprintf (f, "Try %d-%d: Catch %s", block->catch_info->start_pc, block->catch_info->end_pc,
					block->catch_info->catch_type ? block->catch_info->catch_type->name->data : "Any");
			}
			else
			{
				fprintf (f, "%d - %d", block->pc_start, block->pc_end);
			}

			fprintf (f, "|{{");
			if (block->bitmap)
			{
				for (i = 0; i < info->bitmap_size_bits; i++)
				{
					if (i == block->bitmap_index) fprintf (f, "[");
					fprintf (f, "%c", bitmapGet (info, block->bitmap, i) ? '1' : '0');
					if (i + 1 == block->bitmap_index + block->bitmap_size_bits) fprintf (f, "]");
				}
			}

			fprintf (f, "|");
			/* The test for block->final->bitmap is only required to allow dumping incomplete graphs.
			 * It should not be NULL under normal circumstances
			 */
			if (block->final && block->final->bitmap)
			{
				for (i = 0; i < meth->bitmap_size_bits; i++)
				{
					if (i == (uint) block->final->bitmap_index) fprintf (f, "[");
					fprintf (f, "%c", runtimeBitmapGet (meth, block->final, i) ? '1' : '0');
					if (i == (uint) block->final->bitmap_index) fprintf (f, "]");
				}
			}

			fprintf (f, "}|");
			if (!BBT_IS_FAKE(block->type))
			{
				for (pc = block->pc_start; pc <= block->pc_end;)
				{
					fprintf (f, "%s%s\\n",
								pc == block->pc_flow ? "?" : "",
								instruction_name (meth->c.bcode.code + pc));
					pc = pc + instruction_length (meth->c.bcode.code + pc, meth->c.bcode.code + meth->c.bcode.codelen);
				}
			}

			if (block->final && (!STORAGE_IS_EMPTY (&block->final->store) || block->final->store.has_throw))
			{
				fprintf (f, "| WRITES\\n");

				if (block->final->store.has_throw)
				{
					fprintf (f, "THROW\\n");
				}

				if (!STORAGE_IS_EMPTY (&block->final->store))
				{
					if (meth->context_failed)
					{
						fprintf (f, "FALLBACK MODE\\n");
					}
					else
					{
						dumpStores (f, &block->final->store);
					}
				}
			}

			if (block->final && /*(BB_HAS_CASES (block) || BB_HAS_CATCHES (block)) &&*/ block->final->store_case_count)
			{
				uint i;
				for (i = 0; i < block->final->store_case_count; i++)
				{
					fprintf (f, "| CASE %u\\n", i);
					dumpStores (f, &block->final->store_cases[i]);
				}
			}

			fprintf (f, "}}}\"];\n");

			if (BB_HAS_TARGET(block))
			{
				fprintf (f, "\tblock%p -> block%p[label=\"t\"];\n", block, block->target);
			}
			if (BB_HAS_NEXT(block))
			{
				fprintf (f, "\tblock%p -> block%p[label=\"n\"];\n", block, LIST_NEXT (block, normal));
			}
			if (BB_HAS_CASES (block) || BB_HAS_CATCHES (block))
			{
				int caseIndex;
				for (caseIndex = 0; block->cases[caseIndex]; caseIndex++)
				{
					fprintf (f, "\tblock%p -> block%p[label=\"%u\"];\n", block, block->cases[caseIndex], caseIndex);
				}
			}
			if (BB_HAS_TRIES (block))
			{
				int tryIndex;
				for (tryIndex = 0; block->tries[tryIndex]; tryIndex++)
				{
					fprintf (f, "\tblock%p -> block%p[style=dotted label=\"%u\"];\n", block->tries[tryIndex], block, tryIndex);
				}
			}
#ifdef RENDER_FINISHED
			if (block->finished)
			{
				fprintf (f, "\tblock%p -> block%p[style=dashed,label=\"finished\"];\n", block, block->finished);
			}
#endif

			if (info->join_block)
			{
				fprintf (f, "\tblock%p [label=\"JOIN", info->join_block);
				if (info->join_block->bitmap)
				{
					fprintf (f, "\\n");
					for (i = 0; i < info->bitmap_size_bits; i++)
					{
						fprintf (f, "%c", bitmapGet (info, info->join_block->bitmap, i) ? '1' : '0');
					}
				}
				fprintf (f, "\"];\n");
			}
		}

		if (step < 0)
		{
			for (block = LIST_FRONT (cond_unfinished); block; block = LIST_NEXT (block, cond_unfinished))
			{
				fprintf (f, "\tblock%p -> UNFINISHED;\n", block);
			}
		}

		fprintf (f, "}\n");
		fclose (f);
	}
}

#endif /* KAFFE_VMDEBUG */
/**********************************************************************************************************************/
/* Memory-management routines for BasicBlock structures. */
/**********************************************************************************************************************/

static BasicBlock *allocBasicBlock (ContextAnalysisInfo *info,
									uint pc_start, uint pc_end,
									uint pc_flow, BasicBlockType type)
{
	BasicBlock *bb;

	while (info->basic_block_alloc_ptr &&
		   info->basic_block_alloc_ptr->used == (sizeof (info->basic_block_alloc_ptr->basic_blocks) /
		   sizeof (info->basic_block_alloc_ptr->basic_blocks[0])))
	{
		info->basic_block_alloc_ptr = info->basic_block_alloc_ptr->next;
	}

	if (!info->basic_block_alloc_ptr)
	{
		BasicBlockAlloc *alloc = (BasicBlockAlloc *) malloc (sizeof (BasicBlockAlloc));
		if (info->basic_block_alloc_last) info->basic_block_alloc_last->next = alloc;
		else info->basic_block_alloc_first = alloc;
		info->basic_block_alloc_last = alloc;
		info->basic_block_alloc_ptr  = alloc;
		alloc->used = 0;
		alloc->next = NULL;
	}

	bb = &info->basic_block_alloc_ptr->basic_blocks[info->basic_block_alloc_last->used++];
	bb->pc_start 				= pc_start;
	bb->pc_end					= pc_end;
	bb->pc_flow					= pc_flow;
	bb->type					= type;
	bb->bitmap					= NULL;
	bb->bitmap_index			= (BitmapElement) -1;
	bb->bitmap_size_bits		= 0;
	bb->finished				= NULL;
	bb->target					= NULL;
	bb->cases					= NULL;
	bb->next_normal				= NULL;
	bb->next_cond				= NULL;
	bb->next_cond_unfinished	= NULL;
	bb->next_flow				= NULL;
	bb->final					= NULL;
	bb->catch_info				= NULL;

	info->block_count++;
	dbg_printf ("* %d %d\n", pc_start, pc_end);

	return bb;
}

/**********************************************************************************************************************/

static void freeBasicBlocks (ContextAnalysisInfo *info)
{
	for (info->basic_block_alloc_ptr = info->basic_block_alloc_first; info->basic_block_alloc_ptr;
		 info->basic_block_alloc_ptr = info->basic_block_alloc_ptr->next)
	{
		info->basic_block_alloc_ptr->used = 0;
	}
	info->basic_block_alloc_ptr = info->basic_block_alloc_first;
}

/**********************************************************************************************************************/
/* Memory management routines for bitmaps and switch targets */
/**********************************************************************************************************************/

#define PRIMITIVE_ALLOC_LIST(name,Type)																				\
	static Type *alloc##name##Elements (ContextAnalysisInfo *info, unsigned int count)								\
	{																												\
		assert (count <= (sizeof (info->name##_alloc_ptr->data) / sizeof (info->name##_alloc_ptr->data[0])));		\
																													\
		while (info->name##_alloc_ptr && info->name##_alloc_ptr->used + count >=									\
				  		(sizeof (info->name##_alloc_ptr->data) / sizeof (info->name##_alloc_ptr->data[0])))			\
		{																											\
			info->name##_alloc_ptr = info->name##_alloc_ptr->next;													\
		}																											\
																													\
		if (!info->name##_alloc_ptr)																				\
		{																											\
			name##Alloc *alloc = (name##Alloc *) malloc (sizeof (name##Alloc));										\
			if (info->name##_alloc_last) info->name##_alloc_last->next = alloc;										\
			else info->name##_alloc_first = alloc;																	\
			info->name##_alloc_last = alloc;																		\
			info->name##_alloc_ptr  = alloc;																		\
			alloc->used = 0;																						\
			alloc->next = NULL;																						\
		}																											\
																													\
		Type *res = &info->name##_alloc_ptr->data[info->name##_alloc_last->used];									\
		memset (res, 0, sizeof (BitmapElement) * count);															\
		info->name##_alloc_last->used += count;																		\
		return res;																									\
	}																												\
																													\
	static void free##name##s (ContextAnalysisInfo *info)																\
	{																												\
		for (info->name##_alloc_ptr = info->name##_alloc_first; 													\
			 info->name##_alloc_ptr; info->name##_alloc_ptr = info->name##_alloc_ptr->next)							\
		{																											\
			info->name##_alloc_ptr->used = 0;																		\
		}																											\
		info->name##_alloc_ptr = info->name##_alloc_first;															\
	}																												\
																													\
	static void destroy##name##s (ContextAnalysisInfo *info)															\
	{																												\
		for (info->name##_alloc_ptr = info->name##_alloc_first; info->name##_alloc_ptr; )							\
		{																											\
			name##Alloc *next = info->name##_alloc_ptr->next;														\
			free (info->name##_alloc_ptr);																			\
			info->name##_alloc_ptr = next;																			\
		}																											\
	}

PRIMITIVE_ALLOC_LIST (Bitmap, BitmapElement);
static inline BitmapElement *allocBitmap (ContextAnalysisInfo *info)
{
	return allocBitmapElements (info, info->bitmap_size_elements);
}

PRIMITIVE_ALLOC_LIST (SwitchTarget, BasicBlock *);
/**********************************************************************************************************************/

void trishulDestroyCodeAnalyseData (UNUSED ContextAnalysisInfo *info)
{
	/* TODO: this has been moved to endAnalyze, as no more global pointers are kept. This means no more memory
	 * is cached. If possible re-enable this.
	 */
}

/**********************************************************************************************************************/

static void allocateBitmapBits (ContextAnalysisInfo *info, BasicBlock *block, uint bitCount)
{
	block->bitmap_index 		 = info->bitmap_size_bits;
	block->bitmap_size_bits 	 = bitCount;
	info->bitmap_size_bits 		+= block->bitmap_size_bits;
	info->branch_count++;
}

/**********************************************************************************************************************/

BasicBlock *contextAnalysisControlFlowInstruction (ContextAnalysisInfo *info, uint pc, uint pc_target,
												 unsigned int length, BasicBlockType type)
{
	/* The block starts at pc 0, or after the previous block */
	int start_pc;
	if (LIST_IS_EMPTY (normal))
	{
		start_pc = 0;
	}
	else
	{
		start_pc = LIST_BACK (normal)->pc_end + 1;
	}

	/* Create the BasicBlock, or several if forward branches were defined */
	BasicBlock *branch = newBasicBlock (info, start_pc, pc + length - 1, pc, type);

	if (BBT_IS_IF (type))
	{
		allocateBitmapBits (info, branch, 2);
	}
	else if (BBT_IS_SWITCH (type))
	{
		int case_count = pc_target;
		allocateBitmapBits (info, branch, case_count + 1); /* +1 for default case */
		branch->cases = allocSwitchTargetElements (info, case_count + 1); /* +1 for terminating NULL */
	}
	else if (BB_HAS_JOIN (branch))
	{
		if (!info->join_block)
			info->join_block = allocBasicBlock (info, PC_INVALID, PC_INVALID, PC_INVALID, BBT_JOIN);
		assert (!branch->target);
		branch->target = info->join_block;
	}

	/* No else here, this overlaps the above if */
	if (BBT_IS_JUMP (type))
	{
		if (pc_target > pc)
		{
			/* Forward branch */
			dbg_printf ("> %d\n", pc_target);
			branch->target = forwardBranch (info, pc_target);
		}
		else
		{
			/* Backward branch */
			dbg_printf ("< %d\n", pc_target);
			/* Store the old end of the block, as the backward branch may be into the current block, which would
			 * cause it to split. By checking pc_end after the branch, the split can be detected.
			 */
			uint old_end = branch->pc_end;
			BasicBlock *target = backwardBranch (info, pc_target);
			if (branch->pc_end == old_end)
			{
				branch->target = target;
			}
			else
			{
				target->target = target;
			}
		}
	}

	return branch;
}

/**********************************************************************************************************************/

void contextAnalysisMethodCallException(struct ContextAnalysisInfo *info, uint pc, uint length, const Method *invoked)
{
	assert (invoked->ndeclared_exceptions);
	int i;
	int throwCount = invoked->ndeclared_exceptions;
#ifdef TRISHUL_EXCEPTION_EXCLUDE_RUNTIME
	int nonRuntime = 0;

	// If runtime exceptions are declared, ignore them anyway
	for (i = 0; i < invoked->ndeclared_exceptions; i++)
	{
		errorInfo einfo;
		constIndex exceptIndex = invoked->declared_exceptions[i];
		Hjava_lang_Class *except = getClass (exceptIndex, invoked->class, &einfo);
		if (!isRuntimeException (except))
		{
			nonRuntime++;
		}
	}

	if (!nonRuntime)
		return;

	throwCount = nonRuntime;
#endif

	BasicBlock *bb = contextAnalysisControlFlowInstruction (info, pc, PC_INVALID, length, BBT_METHODCALLEXCEPTION);
	bb->method  = invoked;
	bb->catches = allocSwitchTargetElements (info, throwCount + 1);

	LIST_ADD_FRONT (bb, cond);

	if (!info->join_block)
		info->join_block = allocBasicBlock (info, PC_INVALID, PC_INVALID, PC_INVALID, BBT_JOIN);

	// The proper catch is determined in endAnalyze
	for (i = 0; i < invoked->ndeclared_exceptions; i++)
	{
#ifdef TRISHUL_EXCEPTION_EXCLUDE_RUNTIME
		errorInfo einfo;
		constIndex exceptIndex = invoked->declared_exceptions[i];
		Hjava_lang_Class *except = getClass (exceptIndex, invoked->class, &einfo);
		if (!isRuntimeException (except))
#endif
		{
			bb->catches[i] = info->join_block;
		}
	}

	allocateBitmapBits (info, bb, 1 + throwCount);
}

/**********************************************************************************************************************/

void contextAnalysisSwitchCase (ContextAnalysisInfo *info, BasicBlock *switchBlock, int index, uint pc, uint pc_target)
{
	assert (BBT_IS_SWITCH (switchBlock->type));
	BasicBlock **target;

	if (index < 0) target = &switchBlock->target;
	else target = &switchBlock->cases[index];

	if (pc_target > pc)
	{
		/* Forward branch */
		dbg_printf ("> %d\n", pc_target);
		*target = forwardBranch (info, pc_target);
	}
	else
	{
		/* Backward branch */
		dbg_printf ("< %d\n", pc_target);
		*target = backwardBranch (info, pc_target);
	}
}

/**********************************************************************************************************************/

ContextAnalysisInfo *contextAnalysisBeginAnalyze (Method *meth)
{
	ContextAnalysisInfo *info = (ContextAnalysisInfo *) calloc (1, sizeof (*info));
	info->method = meth;

	dbg_printf ("BEGIN ANALYZE\n");
	/* Add forward blocks for th etry blocks. This prevents try blocks partially covering basic blocks */
	if (meth->exception_table)
	{
		uint i;
		for (i = 0; i < meth->exception_table->length; i++)
		{
			jexceptionEntry *entry 	= &(meth->exception_table->entry[i]);
			forwardBranch (info, entry->start_pc);
			forwardBranch (info, entry->end_pc + instruction_length (meth->c.bcode.code + entry->end_pc,
																meth->c.bcode.code + meth->c.bcode.codelen));
		}
	}

	return info;
}

/**********************************************************************************************************************/

static inline void checkFinished (ContextAnalysisInfo *info, BasicBlock *check_block)
{
	BasicBlock *cur;
	BasicBlock *prev = NULL;
	BitmapElement i;

	for (cur = LIST_FRONT (cond_unfinished); cur; cur = LIST_NEXT (cur, cond_unfinished))
	{
		bool finished = true;
		/* TODO: this can be done better with a bitmap per condition */
		for (i = 0; i < cur->bitmap_size_bits && finished; i++)
		{
			if (!bitmapGet (info, check_block->bitmap, cur->bitmap_index + i))
				finished = false;
		}

		if (finished)
		{
			/* Set the finished flag */
			cur->finished = check_block;
			LIST_REMOVE_PREV (cur, prev, cond_unfinished);
		}
		else
		{
			prev = cur;
		}
	}

#ifdef KAFFE_VMDEBUG
	for (cur = LIST_FRONT (cond_unfinished); cur; cur = LIST_NEXT (cur, cond_unfinished))
	{
		assert (!cur->finished);
	}
#endif
}

/**********************************************************************************************************************/

static void initializeBitmaps (ContextAnalysisInfo *info)
{
	BasicBlock *bb;
	info->bitmap_size_elements = bitmapElementCount (info->bitmap_size_bits);

	/* Allocate bitmaps */
	for (bb = LIST_FRONT(normal); bb; bb = LIST_NEXT (bb, normal))
	{
		bb->bitmap = allocBitmap (info);
		if (BBT_IS_IF (bb->type) || BBT_IS_SWITCH (bb->type) || (false && BBT_IS_CATCH (bb->type) && !strcmp (info->method->name->data, "leak")))
		{
			LIST_ADD_BACK (bb, cond);
			LIST_ADD_BACK (bb, cond_unfinished);
		}
	}

	if (info->join_block)
		info->join_block->bitmap = allocBitmap (info);

	/* Initialize bitmaps */
	for (bb = LIST_FRONT(cond); bb; bb = LIST_NEXT(bb, cond))
	{
		if (BBT_IS_IF (bb->type) || BBT_IS_SWITCH (bb->type) || BB_HAS_TRIES (bb))
		{
			assert (bb->target && bb->target->bitmap);
			bitmapSet (info, bb->target->bitmap, bb->bitmap_index);
			LIST_ADD_BACK (bb->target, flow);
			checkFinished (info, bb->target);
		}

		// No else here

		if (BBT_IS_IF (bb->type))
		{
			assert (LIST_NEXT (bb, normal) && LIST_NEXT (bb, normal)->bitmap);
			bitmapSet (info, LIST_NEXT (bb, normal)->bitmap, bb->bitmap_index + 1);
			LIST_ADD_BACK (LIST_NEXT (bb, normal), flow);
			checkFinished (info, LIST_NEXT (bb, normal));
		}
		else if (BB_HAS_CASES (bb) || BB_HAS_TRIES (bb))
		{
			int caseIndex;
			for (caseIndex = 0; bb->cases[caseIndex]; caseIndex++)
			{
				assert (bb->cases[caseIndex]->bitmap);
				bitmapSet (info, bb->cases[caseIndex]->bitmap, bb->bitmap_index + 1 + caseIndex);
				LIST_ADD_BACK (bb->cases[caseIndex], flow);
				checkFinished (info, bb->cases[caseIndex]);
			}
		}
		else if (BB_HAS_CATCHES (bb))
		{
			int caseIndex;
			for (caseIndex = 0; bb->cases[caseIndex]; caseIndex++)
			{
				assert (bb->cases[caseIndex]->bitmap);
				bitmapSet (info, bb->cases[caseIndex]->bitmap, bb->bitmap_index + caseIndex);
				LIST_ADD_BACK (bb->cases[caseIndex], flow);
				checkFinished (info, bb->cases[caseIndex]);
			}

			if (BBT_IS_CALL (bb->type))
			{
				// The case when no exception is throw in the method
				assert (LIST_NEXT (bb, normal) && LIST_NEXT (bb, normal)->bitmap);
				bitmapSet (info, LIST_NEXT (bb, normal)->bitmap, bb->bitmap_index + caseIndex);
				LIST_ADD_BACK (LIST_NEXT (bb, normal), flow);
				checkFinished (info, LIST_NEXT (bb, normal));
			}
		}
	}
}

/**********************************************************************************************************************/

static void processBitmaps (ContextAnalysisInfo *info, Method *meth)
{
	BasicBlock *bb;

	/* Process the control-flow bitmaps */
	bool change;

#ifdef KAFFE_VMDEBUG
	int step = 0;
#endif
	do
	{
		change = false;

		for (bb = LIST_FRONT (normal); bb; bb = LIST_NEXT (bb, normal))
		{
			if (BB_HAS_NEXT (bb) && bitmapFlow (info, bb, LIST_NEXT (bb, normal)))
			{
				change 			= true;
				LIST_ADD_BACK	(LIST_NEXT (bb, normal), flow);
				checkFinished 	(info, LIST_NEXT (bb, normal));
			}

			if (BB_HAS_TARGET (bb) && bitmapFlow (info, bb, bb->target))
			{
				change 			= true;
				LIST_ADD_BACK	(bb->target, flow);
				checkFinished 	(info, bb->target);
			}

			if (BB_HAS_CASES (bb))
			{
				int caseIndex;
				for (caseIndex = 0; bb->cases[caseIndex]; caseIndex++)
				{
					if (bitmapFlow (info, bb, bb->cases[caseIndex]))
					{
						change 			= true;
						LIST_ADD_BACK	(bb->cases[caseIndex], flow);
						checkFinished 	(info, bb->cases[caseIndex]);
					}
				}
			}

			dumpGraph (info, meth, step++, false);
		}

	} while (change);
}

/**********************************************************************************************************************/

BasicBlock *findBasicBlock (ContextAnalysisInfo *info,uint pc_start)
{
	BasicBlock *cur;
	for (cur = info->list_normal.first; cur; cur = cur->next_normal)
	{
		if (cur->pc_start == pc_start)
			return cur;
	}
	return NULL;
}

/**********************************************************************************************************************/

static inline bool pcInRange (uint pc, uint pc_start, uint pc_end)
{
	return (pc >= pc_start) && (pc < pc_end);
}

/**********************************************************************************************************************/

static inline bool isBlockContained (const BasicBlock *bb, uint pc_start, uint pc_end)
{
	return 	pcInRange (bb->pc_start, pc_start, pc_end) &&
			pcInRange (bb->pc_end,   pc_start, pc_end) ;
}

/**********************************************************************************************************************/

/**
 * Checks if the block can jump outside the specified pc range.
 */
static int checkExceptionBlocks (ContextAnalysisInfo *info, BasicBlock *bb, uint pc_start, uint pc_end)
{
	int count = 0;

	if (BB_HAS_NEXT (bb) && !isBlockContained (LIST_NEXT (bb, normal), pc_start, pc_end))
	{
		BasicBlock *next = LIST_NEXT (bb, normal);
		if (!next->next_temp && info->list_temp.first != next)
			LIST_ADD_FRONT (next, temp);
		count++;
	}

	if (BB_HAS_TARGET (bb) && !isBlockContained (bb->target, pc_start, pc_end))
	{
		BasicBlock *next = bb->target;
		if (!next->next_temp && info->list_temp.first != next)
			LIST_ADD_FRONT (next, temp);
		count++;
	}

	if (BB_HAS_CASES (bb))
	{
		BasicBlock **caseBlock;
		for (caseBlock = bb->cases; *caseBlock; caseBlock++)
		{
			if (!isBlockContained (*caseBlock, pc_start, pc_end))
			{
				BasicBlock *next = *caseBlock;
				if (!next->next_temp && info->list_temp.first != next)
					LIST_ADD_FRONT (next, temp);
				count++;
			}
		}
	}

	return count;
}

/**********************************************************************************************************************/

void contextAnalysisEndAnalyze (errorInfo *einfo, ContextAnalysisInfo *info, Method *meth, uint pc)
{
	if (LIST_IS_EMPTY (normal))
	{
		/* If there's only a single basic block, there's nothing to do.
		* This case is fairly frequent (simple ctors, get and set methods, etc),
		* so this saves some work
		*/
		free (info);
		return;
	}

	if (LIST_BACK(normal)->pc_end + 1 != pc)
	{
		/* Allocate the last basic block */
		newBasicBlock (info, LIST_BACK(normal)->pc_end + 1, pc - 1, PC_INVALID, BBT_NONE);
	}


	/* Add entries for the catch blocks */
	if (meth->exception_table)
	{
		uint i;
		for (i = 0; i < meth->exception_table->length; i++)
		{
			jexceptionEntry *entry 		= &(meth->exception_table->entry[i]);
			BasicBlock *bbCatch 		= newBasicBlock (info, PC_INVALID, PC_INVALID, PC_INVALID, BBT_CATCH);
			bbCatch->catch_info 		= entry;
			bbCatch->target				= findBasicBlock (info, entry->handler_pc);
			LIST_ADD_FRONT (bbCatch, cond);

			/* Scan the basic blocks to point try blocks to the correct catch */
			BasicBlock *cur;
			uint count = 0;
			LIST_CLEAR (temp);
			for (cur = LIST_FRONT(normal); cur; cur = LIST_NEXT (cur, normal))
			{
				/* Only valid if actually inside the try block */
				if (!(cur->pc_start >= bbCatch->catch_info->start_pc && cur->pc_start <= bbCatch->catch_info->end_pc))
					continue;
						
				// if the type of exception that is thrown can be detected, a better analysis can be
				// made by pointing it towards the appropriate catch block
				if (BBT_IS_THROW (cur->type) && bbCatch->catch_info->catch_type)
				{
					einfo->type = 0;
					Hjava_lang_Class *except = detectExceptionType (einfo, meth, cur->pc_start, cur->pc_end);
					if (einfo->type)
						return;
					if (except &&
#ifdef TRISHUL_EXCEPTION_EXCLUDE_RUNTIME
						!isRuntimeException (except) &&
#endif
						classExtends2 (except, bbCatch->catch_info->catch_type))
					{
						cur->target = bbCatch;
					}
				}

				// Check if a method might throw an exception that ends up here
				if (BBT_IS_CALL (cur->type))
				{
					int clauseIndex;
					for (clauseIndex = 0; clauseIndex < cur->method->ndeclared_exceptions; clauseIndex++)
					{
						constIndex exceptIndex = cur->method->declared_exceptions[clauseIndex];
						Hjava_lang_Class *except = getClass (exceptIndex, cur->method->class, einfo);
						if (except &&
#ifdef TRISHUL_EXCEPTION_EXCLUDE_RUNTIME
							!isRuntimeException (except) &&
#endif
							classExtends2 (except, bbCatch->catch_info->catch_type))
						{
							cur->catches[clauseIndex] = bbCatch;
						}
					}
				}

				if (entry->start_pc <= cur->pc_start && entry->end_pc >= cur->pc_end)
				{
					/* Check if this block jumps outside the try */
					count += checkExceptionBlocks (info, cur, entry->start_pc, entry->end_pc);
				}
				if (cur->pc_start > entry->end_pc)
					break;
			}

			if (count > 0)
			{
				// The +1 is for the exception case
				allocateBitmapBits (info, bbCatch, count + 1);
				bbCatch->cases = allocSwitchTargetElements (info, count + 1);
				uint j;
				for (j = 0, cur = LIST_FRONT(temp); cur; j++)
				{
					bbCatch->cases[j] = cur;

					BasicBlock *next = LIST_NEXT (cur, temp);
					cur->next_temp = NULL;
					cur = next;
				}
			}
		}

	}

	initializeBitmaps (info);
	processBitmaps	  (info, meth);

	dumpGraph (info, meth, -1, false);
	if (!LIST_IS_EMPTY (cond_unfinished))
		dumpGraph (info, meth, -1, true);
	assert (LIST_IS_EMPTY (cond_unfinished));

	/* Store the information with the method */
	/* TODO: release the memory allocated here */
	/* TODO: can the cond list be dropped? The only use (for initialization of bitmaps) can be done using
	 *		 the cond_unfinished list. branch_count might be unused, too.
	 */
	if (info->branch_count)
	{
		BasicBlock *bb, *cond;
		RuntimeBitmapElement 	*bitmap_ptr;
		int 					bitmap_index = 0;
		StorageRecord			*storage_ptr;

		meth->bitmap_size_bits 		= info->branch_count;

		meth->bitmap_size_elements	= bitmapElementCount (meth->bitmap_size_bits);

		meth->bitmap_data			= (RuntimeBitmapElement *) calloc (info->block_count, meth->bitmap_size_elements *
																					sizeof (RuntimeBitmapElement));
		bitmap_ptr					= meth->bitmap_data;

		meth->contexts 			= (ContextBasicBlock *) calloc (info->block_count, sizeof (*meth->contexts));
		meth->context_storage  	= (StorageRecord *) calloc (info->bitmap_size_bits, sizeof (*meth->context_storage));
		storage_ptr				= meth->context_storage;
		RecordAllocContextAnalysis (info->block_count * sizeof (*meth->contexts));
		RecordAllocStorageRecord (info->bitmap_size_bits * sizeof (*meth->context_storage));

		/* Initialize the runtime context structures */
		for (bb = LIST_FRONT(normal); bb; bb = LIST_NEXT(bb, normal), meth->context_count++)
		{
			assert (meth->context_count < info->block_count);

			meth->contexts[meth->context_count].pc_start 			= bb->pc_start;
			meth->contexts[meth->context_count].pc_end				= bb->pc_end;
			meth->contexts[meth->context_count].type				= bb->type;
			meth->contexts[meth->context_count].store_cases			= storage_ptr;
			meth->contexts[meth->context_count].store_case_count	= bb->bitmap_size_bits;
			storage_ptr += bb->bitmap_size_bits;
/*
			if (meth->context_count && meth->contexts[meth->context_count].pc_start != PC_INVALID &&
				meth->contexts[meth->context_count - 1].pc_start >= meth->contexts[meth->context_count].pc_start)
			{
				printf ("NAME: %s.%s\n", meth->class->name->data, meth->name->data);
				dumpGraph (info, meth, -1, true);
			}
*/
			/* Ensure they are sorted by pc_start */
			assert (!meth->context_count ||
					meth->contexts[meth->context_count].pc_start == PC_INVALID ||
					meth->contexts[meth->context_count - 1].pc_start == PC_INVALID ||
					meth->contexts[meth->context_count - 1].pc_start < meth->contexts[meth->context_count].pc_start);

			bb->final				= &meth->contexts[meth->context_count];
			bb->final->bitmap_index = BB_HAS_RUNTIME_BIT (bb) ? bitmap_index++ : -1;

			if (!BBT_IS_FAKE (bb->type))
			{
				einfo->type = 0;
				analyzeBlockStores (einfo, meth, bb->pc_start, bb->pc_end, &bb->final->store);
				if (einfo->type)
					return;
			}
		}

		assert (meth->context_count == info->block_count - (info->join_block ? 1 : 0));
		assert (storage_ptr == meth->context_storage + info->bitmap_size_bits);

		/* Initialize the run-time bitmaps.
		 * This must be done in a separate loop due to the initialization of the bitmap indices.
		 * Also, initialize the branches' storage records
		 */
		for (bb = LIST_FRONT(normal); bb; bb = LIST_NEXT(bb, normal))
		{
			bb->final->bitmap 	= bitmap_ptr;
			bitmap_ptr			+= meth->bitmap_size_elements;

			if (bb->catch_info)
				bb->catch_info->context = bb->final;

			for (cond = LIST_FRONT(cond); cond; cond = LIST_NEXT(cond, cond))
			{
				uint i;
				bool haveSet = false, haveUnset = false;
				for (i = 0; i < cond->bitmap_size_bits; i++)
				{
					bool val = bitmapGet (info, bb->bitmap, cond->bitmap_index + i);
					if (val) haveSet = true;
					else haveUnset = true;

					if (haveSet && haveUnset) break;
				}

				if (haveSet && haveUnset)
				{
					runtimeBitmapSet (meth, bb->final, cond->final->bitmap_index);

					/* Loop through bits again to detect which cases to add to */

					for (i = 0; i < cond->bitmap_size_bits; i++)
					{
						bool val = bitmapGet (info, bb->bitmap, cond->bitmap_index + i);
						if (val)
						{
							storageRecordCopyRecord (&cond->final->store_cases[i], &bb->final->store);
						}
					}
				}
			}
		}
	}

	dumpGraph (info, meth, -1, false);

	/* Clean up */
	freeBasicBlocks 	(info);
	freeBitmaps 		(info);
	freeSwitchTargets 	(info);

	/* This used to be in trishulDestroyCodeAnalyseData */

	/* Destroy BasicBlocks */
	for (info->basic_block_alloc_ptr = info->basic_block_alloc_first; info->basic_block_alloc_ptr; )
	{
		BasicBlockAlloc *next = info->basic_block_alloc_ptr->next;
		free (info->basic_block_alloc_ptr);
		info->basic_block_alloc_ptr = next;
	}

	destroyBitmaps (info);
	destroySwitchTargets (info);

	free 				(info);
}

/**********************************************************************************************************************/

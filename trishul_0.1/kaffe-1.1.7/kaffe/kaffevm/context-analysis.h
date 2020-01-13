#ifndef __context_analysis_h
#define __context_analysis_h
/**********************************************************************************************************************/
#include "decompile.h"
/**********************************************************************************************************************/

/**
 * The type of the control-flow instruction at the end of a BasicBlock.
 */
typedef enum
{
	BBT_NONE,
	BBT_IF_FORWARD,
	BBT_IF_BACKWARD,
	BBT_GOTO_FORWARD,
	BBT_GOTO_BACKWARD,
	BBT_RETURN,
	BBT_TABLESWITCH,
	BBT_LOOKUPSWITCH,
	BBT_JOIN,
	BBT_THROW,
	BBT_CATCH,
	BBT_METHODCALLEXCEPTION,
} BasicBlockType;

/**********************************************************************************************************************/

typedef struct ContextBasicBlock
{
	unsigned int			pc_start;
	unsigned int			pc_end;
#ifdef JIT3
	unsigned int			pc_start_orig;
	unsigned int			pc_end_orig;
#endif
	BasicBlockType			type;
	RuntimeBitmapElement	*bitmap;
	int						bitmap_index;

	/**
	 * All locations this basic block stores into.
	 */
	StorageRecord			store;

	/**
	 * All locations the following basic blocks store into, if the specific branch is taken.
	 * For switch blocks, this is the sequential list of cases, with the default case last.
	 * For an if-block, this contains the if-block first, the else-block second.
	 */
	StorageRecord			*store_cases;
	unsigned int			store_case_count;
} ContextBasicBlock;

/**********************************************************************************************************************/

#include "classMethod.h"

typedef unsigned int BitmapElement;

/**
 * This isn't added to the basic blocks above as those are long-lived, while these are
 * temporary structures.
 */
typedef struct BasicBlock
{
	/**
	 * The range of instructions covered by this basic block is <pc_start,pc_end>.
	 */
	uint			pc_start;
	uint			pc_end;
	/**
	 * The address of the control-flow instruction at the end of the block.
	 */
	uint			pc_flow;

	/**
 	 * The type of the control-flow instruction at the end of this BasicBlock.
 	 */
	BasicBlockType	type;

	/**
	 * The bitmap of flags used to track condition dependencies. This may be one
	 * or more elements, depending on the global variable bitmap_size_elements.
	 */
	BitmapElement	*bitmap;

	/**
	 * Bits bitmap[bitmap_index .. bitmap_index + bitmap_size_bits] are used by the
	 * conditional instruction in this basic block.
	 * If this basic block is not a conditional block, this value is ignored
	 */
	unsigned int	bitmap_index;
	unsigned int	bitmap_size_bits;

	/**
	 * If this is a conditional block, this points to the first BasicBlock at which all
	 * control-flow paths converge.
	 */
	struct BasicBlock	*finished;

	/**
	 * The target if this is a jump. The default case if this is a switch. NULL otherwise.
	 */
	struct BasicBlock	*target;

	union
	{
		/**
		 * The cases if this is a switch.
		 * Terminated by a NULL pointer.
		 */
		struct BasicBlock	**cases;

		/**
		 * The list of try blocks if this is a catch block.
		 */
		struct BasicBlock 	**tries;


		/**
		 * The list of catch blocks this block may jump to, if this block is a method invocation
		 */
		struct BasicBlock 	**catches;
	};

	/**
	 * The next block if no branch is taken. NULL if this is a goto or return.
	 */
	struct BasicBlock	*next_normal;

	/**
	 * Forms a list of conditional-blocks.
	 */
	struct BasicBlock	*next_cond;

	/**
	 * Forms a list of conditional-blocks whose finish-block has not yet been found.
	 */
	struct BasicBlock	*next_cond_unfinished;

	union
	{
		/**
		 * Used to form a temporary list during bitmap flow
	 	 */
		struct BasicBlock	*next_flow;

		struct BasicBlock	*next_temp;

	};

	ContextBasicBlock	*final;

	jexceptionEntry		*catch_info;

	const Method		*method;
} BasicBlock;

#define BBT_IS_GOTO(bbt) 		((bbt) == BBT_GOTO_FORWARD || (bbt) == BBT_GOTO_BACKWARD)
#define BBT_IS_IF(bbt) 			((bbt) == BBT_IF_FORWARD || (bbt) == BBT_IF_BACKWARD)
#define BBT_IS_SWITCH(bbt) 		((bbt) == BBT_TABLESWITCH || (bbt) == BBT_LOOKUPSWITCH)
#define BBT_IS_JUMP(bbt) 		(BBT_IS_GOTO (bbt) || BBT_IS_IF (bbt))
//#define BBT_IS_CONDITION(bbt)	(BBT_IS_IF(bbt) || BBT_IS_SWITCH(bbt))
#define BBT_IS_RETURN(bbt) 		((bbt) == BBT_RETURN)
#define BBT_IS_JOIN(bbt) 		((bbt) == BBT_JOIN)
#define BBT_IS_THROW(bbt) 		((bbt) == BBT_THROW)
#define BBT_IS_NONE(bbt) 		((bbt) == BBT_NONE)
#define BBT_IS_CATCH(bbt) 		((bbt) == BBT_CATCH)
#define BBT_IS_FAKE(bbt)		(BBT_IS_CATCH(bbt))
#define BBT_IS_CALL(bbt)		((bbt) == BBT_METHODCALLEXCEPTION)

#define BB_HAS_NEXT(bb)   		(BBT_IS_IF ((bb)->type) || BBT_IS_NONE ((bb)->type) || BBT_IS_CALL((bb)->type))
#define BB_HAS_JOIN(bb)			(BBT_IS_RETURN ((bb)->type) || BBT_IS_THROW ((bb)->type))
#define BB_HAS_TARGET(bb) 		(BBT_IS_JUMP ((bb)->type) || BBT_IS_SWITCH ((bb)->type) || BB_HAS_JOIN (bb) || 		\
								 BBT_IS_CATCH ((bb)->type))
#define BB_HAS_CASES(bb)		(BBT_IS_SWITCH ((bb)->type))
#define BB_HAS_TRIES(bb)		(BBT_IS_CATCH ((bb)->type) && ((bb)->tries != NULL))
#define BB_HAS_CATCHES(bb)		(BBT_IS_CALL ((bb)->type) && ((bb)->catches != NULL))
#define BB_HAS_RUNTIME_BIT(bb)	(BBT_IS_IF((bb)->type) || BBT_IS_SWITCH((bb)->type) || BB_HAS_TRIES(bb) || 			\
								 BB_HAS_CATCHES (bb))

/**
 * Releases temporary data used for code analysis.
 */
void trishulDestroyCodeAnalyseData ();

#ifdef KAFFE_VMDEBUG

void trishulAddContextDebugMethod (const char *classname, const char *methodname);
void trishulSetContextDebugMethodFlow (bool flow);

#endif

struct ContextAnalysisInfo;

/**
 * Initializes the global variables for analysis of a method.
 */
struct ContextAnalysisInfo *contextAnalysisBeginAnalyze (Method *meth);

/**
 * Signals a jump instruction (if.. or goto) has been read.
 * @param pc_target	If this is a jump instruction, the jump target.
 *					If this is a switch instruction, the number of cases.
 */
BasicBlock *contextAnalysisControlFlowInstruction (struct ContextAnalysisInfo *, uint pc, uint pc_target,
												 uint length, BasicBlockType type);

void contextAnalysisMethodCallException(struct ContextAnalysisInfo *, uint pc, uint length, const Method *invoked);

/**
 * Adds a case to a switch.
 * @param index 	The 0-based case index, or -1 for the default case.
 */
void contextAnalysisSwitchCase (struct ContextAnalysisInfo *, BasicBlock *switchBlock, int index, uint pc, uint pc_target);

/**
 * Finishes analysis of a single method.
 * Releases the memory allocated for the ContextAnalysisInfo object.
 */
void contextAnalysisEndAnalyze (errorInfo *einfo, struct ContextAnalysisInfo *, Method *meth, uint pc);

/**********************************************************************************************************************/

#endif

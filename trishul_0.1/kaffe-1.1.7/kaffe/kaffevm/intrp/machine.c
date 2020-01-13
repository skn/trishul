/*
 * machine.c
 * Java virtual machine interpreter.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "debug.h"
#define	CDBG(s) 	DBG(INT_VMCALL, s)
#define	RDBG(s) 	DBG(INT_RETURN, s)
#define	NDBG(s) 	DBG(INT_NATIVE, s)
#define	IDBG(s)		DBG(INT_INSTR, s)
#define	CHDBG(s)	DBG(INT_CHECKS, s)

#include "config.h"
#include "config-std.h"
#include "config-math.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "access.h"
#include "baseClasses.h"
#include "bytecode.h"
#include "checks.h"
#include "code-analyse.h"
#include "constants.h"
#include "classMethod.h"
#include "errors.h"
#include "exception.h"
#include "external.h"
#include "gc.h"
#include "gtypes.h"
#include "icode.h"
#include "jthread.h"
#include "locks.h"
#include "lookup.h"
#include "machine.h"
#include "md.h"
#include "object.h"
#include "slots.h"
#include "soft.h"
#include "support.h"
#include "thread.h"
/* PS */
#include "trishul.h"
#include "taint.h"
#include "context-analysis.h"
#include "polymer.h"
#include "utils.h"
#include <stdarg.h>
/* PS- */

/*
 * Define information about this engine.
 */
const char* engine_name = "Interpreter";

#define EXPLICIT_CHECK_NULL(_i, _s, _n)                       \
      cbranch_ref_const_ne_notaint((_s), 0, reference_label(_i, _n)); \
      softcall_nullpointer();                                 \
      set_label(_i, _n)
/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses */
#if defined(CREATE_NULLPOINTER_CHECKS)
#define CHECK_NULL(_i, _s, _n)                                  \
    EXPLICIT_CHECK_NULL(_i, _s, _n)
#else
#define CHECK_NULL(_i, _s, _n)
#endif

/* For JIT3 compatibility */
#define check_array_store(a,b)		softcall_checkarraystore(a,b)
#define explicit_check_null(x,obj,y)	EXPLICIT_CHECK_NULL(x,obj,y)

#if defined(KAFFE_VMDEBUG)
#define check_null(x,obj,y)		EXPLICIT_CHECK_NULL(x,obj,y)
#else
#define check_null(x,obj,y)		CHECK_NULL(x,obj,y)
#endif
#define check_div(x,obj,y)
#define check_div_long(x,obj,y)

NativeTaintInfo_t *currentTaintInfo = NULL;

void ClearContextTaint ()
{
	THREAD_DATA()->contextTaintStack = 0;
}

static int ContextCompare (uintp pc, ContextBasicBlock *bb)
{
	if (pc < bb->pc_start) 	return -1;
	if (pc > bb->pc_end) 	return 1;
	return 0;
}

static inline bool bitmap_get (const RuntimeBitmapElement *bitmap, uint bit_index)
{
	uint index = bit_index / (8 * sizeof (*bitmap));
	uint bit   = bit_index % (8 * sizeof (*bitmap));

	return ((bitmap[index] & (1 << bit)) != 0);
}

#ifdef KAFFE_VMDEBUG

static const char *bitmap_string (const Method *meth, const RuntimeBitmapElement *bitmap)
{
	static char buffer[1024];
	uint i;
	for (i = 0; i < meth->bitmap_size_bits && i < sizeof (buffer) - 1; i++)
	{
		buffer[i] = bitmap_get (bitmap, i) ? '1' : '0';
	}
	buffer[i] = 0;
	return buffer;
}

#endif

static inline void handle_new_context (const Method *meth, uintp pc,
									   const ContextBasicBlock *context_ptr,
									   taint_t *branch_context_taints,
									   taint_t start_context_taint,
									   taint_t *current_context_taint)
{
	/* Rebuild the current context taint and assign the bitmap on the go */
	*current_context_taint = start_context_taint;

	uint 						i;
	RuntimeBitmapElement 		mask = 0;
	const RuntimeBitmapElement	*ptr_src = context_ptr->bitmap - 1; /* Will be incremented in first iteration */
	for (i = 0; i < meth->bitmap_size_bits; i++, mask <<= 1)
	{
		if ((i % (8 * sizeof (RuntimeBitmapElement))) == 0)
		{
			mask = 1;
			ptr_src++;
		}

		if ((*ptr_src) & mask)
		{
			taint_trace_noins (3, ("        : including %d: %X <- %X",
							i, taintMerge2 (*current_context_taint, branch_context_taints[i]),
							branch_context_taints[i]));
			taintAdd1 (*current_context_taint, branch_context_taints[i]);
		}
	}

	taintAdd1 (*current_context_taint, trishulGetFallbackContext ());

	taint_trace_noins (3, ("CONTEXT : <%d,%d>, new='%s', fallback=%X, ctx=%X",
				context_ptr->pc_start, context_ptr->pc_end, bitmap_string (meth, context_ptr->bitmap),
				trishulGetFallbackContext (), *current_context_taint));
}

static ContextBasicBlock *findBasicBlock (const Method *meth, uintp pc)
{
	return (ContextBasicBlock *) bsearch ((void *) pc, meth->contexts,
														 meth->context_count,
														 sizeof (ContextBasicBlock),
														 (int (*)(const void*, const void*)) ContextCompare);
}

static inline void do_trishul_context_check (const Method *meth, uintp pc, uintp old_pc,
											 const ContextBasicBlock **context_ind,
									  		 taint_t *branch_context_taints,
									  		 taint_t start_context_taint,
									   		 taint_t *current_context_taint,
									   		 bool have_context_taints)
{
	if (meth->context_failed)
	{
		/* Fallback mode */
	}
	else
	{
		const ContextBasicBlock *old_context = *context_ind;

		if (pc < old_context->pc_start)
		{
			if (old_context > meth->contexts &&
				pc >= (old_context - 1)->pc_start)
			{
				(*context_ind) = old_context - 1;
			}
			else
			{
				(*context_ind) = (ContextBasicBlock *) bsearch ((void *) pc, meth->contexts,
														 old_context - meth->contexts,
														 sizeof (ContextBasicBlock),
														 (int (*)(const void*, const void*)) ContextCompare);
			}
			assert ((*context_ind) && pc >= (*context_ind)->pc_start && pc <= (*context_ind)->pc_end);
			if (have_context_taints) handle_new_context (meth, pc, *context_ind, branch_context_taints,
														start_context_taint, current_context_taint);
		}
		else if (pc > old_context->pc_end)
		{
			/* Quick check for flow into next context */
			if (old_context < meth->contexts + meth->context_count &&
				pc < (old_context + 1)->pc_end)
			{
				(*context_ind) = old_context + 1;
			}
			else
			{
				(*context_ind) = (ContextBasicBlock *) bsearch ((void *) pc, old_context,
															 meth->context_count - (old_context - meth->contexts),
															 sizeof (ContextBasicBlock),
															 (int (*)(const void*, const void*)) ContextCompare);
			}
			assert ((*context_ind) && pc >= (*context_ind)->pc_start && pc <= (*context_ind)->pc_end);
			if (have_context_taints) handle_new_context (meth, pc, *context_ind, branch_context_taints,
														start_context_taint, current_context_taint);
		}
		else if (pc < old_pc)
		{
			/* Backward jump into same block */
			if (have_context_taints) handle_new_context (meth, pc, *context_ind, branch_context_taints,
														start_context_taint, current_context_taint);
		}
	}
}

static inline void do_trishul_resync_context (const Method *meth, uintp pc,
											  const ContextBasicBlock **context_ind,
									  		  taint_t *branch_context_taints,
									  		  taint_t start_context_taint,
									   		  taint_t *current_context_taint)
{
	(*context_ind) = findBasicBlock (meth, pc);
	assert ((*context_ind) && pc >= (*context_ind)->pc_start && pc <= (*context_ind)->pc_end);
			handle_new_context (meth, pc, *context_ind, branch_context_taints,
								start_context_taint, current_context_taint);
}


/* Checks if a context needs to be popped.
 * TODO: this prints the previous instruction's name, rather then the current. The pc is correct, though.
 */
#define trishul_context_check(pc,old_pc) 																\
	do_trishul_context_check (meth, pc, old_pc, &context_ptr, 											\
								branch_context_taints, start_context_taint, &current_context_taint, have_context_taints)

/**********************************************************************************************************************/
#ifdef TRISHUL_FALLBACK
static taint_t fallbackTaint;
#endif

void trishulSetFallbackContext (taint_t context)
{
#ifdef TRISHUL_FALLBACK
	fallbackTaint = context;
#endif
}

void trishulAddFallbackContext (taint_t context)
{
#ifdef TRISHUL_FALLBACK
	taintAdd1 (fallbackTaint, context);
#endif
}

taint_t trishulGetFallbackContext ()
{
#ifdef TRISHUL_FALLBACK
	return fallbackTaint;
#endif
}
/**********************************************************************************************************************/

static inline void handle_branch (const Method *meth, uintp pc, slots *lcl, slots *sp,
								  const ContextBasicBlock *context, taint_t taint,
								  uint case_taken, taint_t *current_context_taint, taint_t *no_throw_taint,
								  int stackAdjust)
{
	if (meth->context_failed)
	{
		/* Fallback mode */
		if (meth->noTaintFlags & TRISHUL_NOTAINT_FALLBACK)
		{
			taint_trace (2,("IGNORING FALLBACK: %X | %X", *current_context_taint, taint));
		}
		else
		{
			trishulAddFallbackContext (taint);
			*current_context_taint |= taint;
			taint_trace (2,("FALLBACK CONTEXT: %X | %X", trishulGetFallbackContext (), taint));
		}
	}
	else if (taint) /* Nothing to do if there's no taint */
	{
		/* Normal mode */
		if (!context->store_cases ||
			case_taken >= context->store_case_count)
		{
			printf ("%s.%s: %d %d\n", meth->class->name->data, meth->name->data,
								case_taken, context->store_case_count);
		}
		assert (case_taken < context->store_case_count);

		const StorageRecord *stores = &context->store_cases[case_taken];
		const StorageEntry  *entry;
		if (!STORAGE_IS_EMPTY (stores) || stores->has_throw)
		{
			taint_trace(2, ("BRANCH ELSE"));

			if (stores->has_throw)
			{
				if (no_throw_taint)
				{
					taint_trace (2,("    NO THROW: <- %X (| %X)", *no_throw_taint, taint));
					*no_throw_taint |= taint;
				}
				else
				{
					taint_trace (2,("    NO THROW FALLBACK: <- (| %X)", taint));
					trishulAddFallbackContext (taint);
				}
			}

			/* Taint any locals */
			if (lcl && stores->quick_locals)
			{
				uint i;
				for (i = 0; i < sizeof (stores->quick_locals) * 8; i++)
				{
					if (stores->quick_locals & (1 << i))
					{
						taint_trace (2,("    LOCAL %d: <- %X (| %X)", i, local (i)->taint, taint));
						taintAdd1 (local (i)->taint, taint);
					}
				}
			}

			/* Handle other entries */
			for (entry = stores->first; entry; entry = entry->next)
			{
				switch (entry->type)
				{
				case STORE_LOCAL_VARIABLE:
					if (lcl)
					{
						taint_trace (2,("    LOCAL2 %d: <- %X (| %X)", entry->param.local.index,
									local (entry->param.local.index)->taint, taint));
						taintAdd1 (local (entry->param.local.index)->taint, taint);
					}
					break;
				case STORE_FIELD_STATIC:
					if (entry->param.field_static.field->noTaint)
					{
						taint_trace (0,("    STATIC: No taint"));
					}
					else
					{
						taint_trace (2,("    STATIC %s.%s: <- %X (| %X)",
								entry->param.field_static.field->clazz->name->data,
								entry->param.field_static.field->name->data,
								entry->param.field_static.field->trishul.taint, taint));
						taintAdd1 (entry->param.field_static.field->trishul.taint, taint);
					}
					break;
				case STORE_STACK:
					if (sp && entry->param.stack.depth < 0)
					{
						taint_trace (2,("    STACK2: %d: <- %X (| %X)", -(entry->param.stack.depth),
								stack(-(entry->param.stack.depth) + stackAdjust)->taint, taint));
						taintAdd1 (stack(-(entry->param.stack.depth) + stackAdjust)->taint, taint);
					}
					break;
				case STORE_FIELD_OBJECT:
					if (entry->param.field_object.field->noTaint)
					{
						taint_trace (2,("    FIELD: No taint"));
					}
					else
					{
						Hjava_lang_Object *this = (Hjava_lang_Object *)
													lcl[entry->param.field_object.index_this].v.taddr;
						if (this)
						{
							taint_trace (2,("    FIELD %s.%s @ %p: <- %X (| %X)",
								entry->param.field_object.field->clazz->name->data,
								entry->param.field_object.field->name->data, this,
								FIELD_TAINT(this,entry->param.field_object.field), taint));
							taintAdd1 (FIELD_TAINT(this,entry->param.field_object.field), taint);
						}
					}
					break;
				default:
					/* Unimplemented type */
					abort ();
				}
			}
		}
	}
}

void exceptionUnwindStack (Hjava_lang_Object *exceptObj, Method *frameMethod, uintp pc, Method *throwMethod,
							taint_t taint, UNUSED uintp framePointer, bool lastFrame)
{
	if (throwMethod && taint)
	{
		// Locate the context pointer
		ContextBasicBlock *block = findBasicBlock (frameMethod, pc);
		if (block)
		{
			taint_trace_native (2, ("UNWIND: %s.%s: %d: <%d-%d>", frameMethod->class->name->data, frameMethod->name->data,
				pc, block->pc_start, block->pc_end));

			// Try to determine which throws clause this is.
			int index = -1;
			int i;
			for (i = 0; i < throwMethod->ndeclared_exceptions; i++)
			{
				errorInfo einfo;
				Hjava_lang_Class *except = getClass (throwMethod->declared_exceptions[i], frameMethod->class, &einfo);
				if (classExtends2 (exceptObj->vtable->class, except))
				{
					// Found it
					index = i;
					break;
				}
			}

			if (lastFrame && block->bitmap_index >= 0)
			{
				/* Used in runVirtualMachine */
				THREAD_DATA()->throwContext = block;
				THREAD_DATA()->throwIndex = index;
			}


			if (index < 0)
			{
				// TODO: runtime exception
			}
			else
			{
				taint_t context_dummy 	= 0;
				taint_t no_throw_dummy 	= 0;
				handle_branch (frameMethod, pc, NULL, NULL, block, taint, index + 1,
							   &context_dummy, &no_throw_dummy, 0);
			}
		}
	}
}

/* PS - */

#ifdef DIRECT_THREADING

#define	define_insn(code)	INSN_LBL_##code: \
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_insn_alias(code)	INSN_LBL_##code: \
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_wide_insn(code)	break;					\
				case code: 				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );

#define define_insn_fini { \
		trishul_context_check(npc,pc); /* PS */ \
		pc = npc; \
		assert(npc < (uintp)(meth->c.bcode.codelen)); \
		vmExcept_setPC(mjbuf, pc); \
		npc = pc + insnLen[code[pc]]; \
		goto *insn_handlers[code[pc]]; \
	}

#define INTRP_SW_HEAD() goto *insn_handlers[code[pc]];

#define INTRP_SW_PROLOG() \
INSN_LBL_INVALID: { \
	CHDBG(dprintf("Unknown bytecode %d\n", code[pc]); ); \
		throwException(NEW_LANG_EXCEPTION(VerifyError)); \
		goto *insn_handlers[code[npc]]; }


#else
#define	define_insn(code)	case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_insn_alias(code)	case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_wide_insn(code)	break;					\
				case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define define_insn_fini		break;

#define INTRP_SW_HEAD() switch(code[pc])

#define INTRP_SW_PROLOG() \
		default: \
			CHDBG(dprintf("Unknown bytecode %d\n", code[pc]); ); \
			throwException(NEW_LANG_EXCEPTION(VerifyError)); \
			break
#endif

#if defined(KAFFE_PROFILER)
int profFlag;			 /* flag to control profiling */
#endif

void runVirtualMachine(methods *meth, slots *lcl, slots *sp, uintp npc, slots *retval, volatile VmExceptHandler *mjbuf,
						threadData *thread_data, taint_t context_taint,
						taint_t *no_throw_taint/* Combines the taints of all exceptions that are not thrown */
						);

void
virtualMachine(methods*volatile meth, slots* volatile arg, slots* volatile retval, threadData* volatile thread_data,
				taint_t old_context_taint, taint_t *no_throw_taint)
{

	void *trishulAnnotatedStore;

	/* PS: Jump to annotated method if that is available */
	if (meth->annotated)
	{
		// Store the original, for the $super function
		trishulAnnotatedStore = trishulAnnotatedInvokePush ((Method **) &meth);
	}
	else
	{
		trishulAnnotatedStore = NULL;
	}

	methods *volatile const vmeth = meth;
	Hjava_lang_Object* volatile mobj;
	VmExceptHandler mjbuf;
	accessFlags methaccflags;
	const PolymerOrder *order = NULL;

	slots* volatile lcl;
	slots* volatile sp;
	uintp volatile npc;

	int32 idx;
	jint low;

	errorInfo einfo;

	/* PS: initial context taint for new method */
	taint_t new_context_taint;
	retval->taint = 0;

	/* implement stack overflow check */
	if (KTHREAD(stackcheck)(thread_data->needOnStack) == false) {
		if (thread_data->needOnStack == STACK_LOW) {
			CHDBG(
					dprintf(
					"Panic: unhandled StackOverflowError()\n");
				 );
			KAFFEVM_ABORT();
		}

		{
			Hjava_lang_Throwable *th;
			errorInfo soeinfo;

			thread_data->needOnStack = STACK_LOW;
			th = (Hjava_lang_Throwable *)newObjectChecked (javaLangStackOverflowError, &soeinfo);
			thread_data->needOnStack = STACK_HIGH;

			throwException(th);
		}
	}

	CDBG(	dprintf("Call: %s.%s%s.\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth)); );

	/* If this is native, then call the real function */
	methaccflags = meth->accflags;

	/* PS: the context taint starts with the object's and reference's taint, if there is an object */
	if (methaccflags & ACC_STATIC)
	{
		new_context_taint = old_context_taint;
	}
	else
	{
		new_context_taint = taintMerge2 (old_context_taint, ((Hjava_lang_Object *) arg[0].v.taddr)->obj_taint);
#ifdef TRISHUL_TAINT_THIS_INCLUDES_REFERENCE
		taintAdd1 (new_context_taint, arg[0].taint);
#endif
		/*if (new_context_taint == 6 && old_context_taint != 6)
		{
			taint_trace_native (0, ("CONTEXT FROM THIS: %X %X %X: %s %s",
												old_context_taint, ((Hjava_lang_Object *) arg[0].v.taddr)->obj_taint, arg[0].taint,
												((Hjava_lang_Object *) arg[0].v.taddr)->vtable->class->name->data,
												meth->name->data));
		}*/
	}
	THREAD_DATA()->contextTaintStack = new_context_taint;

	/* PS- */


	/* PS: handle polymer actions */
	if (meth->polymerIterationChecked < polymerIteration)
	{
		// Check if it matches a filter
		polymerCheckFilters (meth);
	}

	if (meth->polymerMatch)
	{
		// Invoke the engine
		if (!polymerEngineBefore (meth, (jvalue *) arg, (jvalue *) retval, &order))
			goto annotatedExit;
	}

	/* PS: hook if required */
	// TODO: a hook now avoids polymer, is that desired?
	// TODO: remove hooks now that polymer and annotations are available?
	if (meth != trishulUnhookMethod &&
		meth->hook &&
		(meth->hook_always || hashFind(meth->hook_objects, arg[0].v.taddr) != NULL))
	{
		slots origThis;

		/* If the original is a static method, there is no stack entry for a this pointer, adjust stack for that */
		if (methaccflags & ACC_STATIC)
			arg--;

		/* Patch in tainter's this pointer */
		origThis = arg[0];
		arg[0].v.taddr 	= trishulGetEngine ();
		arg[0].taint	= 0;

		/* Calculate number of arguments */
		idx = sizeofSigMethod(meth, false);
		if (idx == -1) {
			postException(&einfo,  JAVA_LANG(InternalError));
			throwError(&einfo);
		}

		/* Add the reflected method to the stack */
		arg[idx + 1].v.taddr = meth->hook_reflected;
		arg[idx + 1].taint = 0;

		/* If it was on the stack, add the original this pointer */
		if (!(methaccflags & ACC_STATIC))
		{
			arg[idx + 2] = origThis;
		}

		virtualMachine(meth->hook, arg, retval, thread_data, new_context_taint, no_throw_taint);

		/* Restore original stack */
		arg[0] = origThis;
		if (methaccflags & ACC_STATIC) arg++;

		goto annotatedExit;
	}

#if defined(ENABLE_JVMPI)
	if (methaccflags & ACC_STATIC)
	  soft_enter_method(NULL, meth);
	else
		soft_enter_method(((jvalue*)arg)[0].l, meth);
#endif

	if (methaccflags & ACC_NATIVE)
	{
		/* PS */
		NativeTaintInfo_t taint = {arg, retval};
		NativeTaintInfo_t *oldTaintInfo = currentTaintInfo;
		currentTaintInfo = &taint;
		/* PS- */

		NDBG(	dprintf("Call to native %s.%s%s.\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth)); );
		if (methaccflags & ACC_STATIC) {
			KaffeVM_callMethodA(meth, meth, 0, 0, (jvalue*)arg, (jvalue*)retval, 1);
		}
		else {
			KaffeVM_callMethodA(meth, meth, ((jvalue*)arg)[0].l, ((jvalue*)arg)[0].taint,
								&((jvalue*)arg)[1], (jvalue*)retval, 1);
		}

		/* PS */
#if defined(TRISHUL_TAINT_OBJECTS_PASSED_TO_NATIVE_FUNCTION)
		if (new_context_taint)
		{
			int i;
			/* Calculate number of arguments */
			idx = sizeofSigMethod(meth, false);
			if (idx == -1) {
				postException(&einfo,  JAVA_LANG(InternalError));
				throwError(&einfo);
			}

			if (!(methaccflags & ACC_STATIC))
			{
				i++;
				idx++;
			}

			for (i = 0; i < idx; i++)
			{
				taintAdd1 (arg[i].taint, new_context_taint);
				if (arg[i].v.taddr) switch (*METHOD_ARG_TYPE(meth, i))
				{
				case 'L':
					taintObject ((Hjava_lang_Object *) arg[i].v.taddr, new_context_taint);
					break;
				case '[':
					taintArray ((Hjava_lang_Object *) arg[i].v.taddr, new_context_taint);
					break;
				}

			}
		}
#endif
	 	// PS: Polymer finalizer actions
	 	if (order)
	 		polymerEngineAfter (meth, (jvalue *) retval,
	 							(*THREAD_JNIENV())->ExceptionOccurred(THREAD_JNIENV()), order);

		/* Always taint the return value */
		taintAdd1 (retval->taint, new_context_taint);

		currentTaintInfo = oldTaintInfo;
		/* PS- */

#if defined(ENABLE_JVMPI)
		soft_exit_method(meth);
#endif

		goto annotatedExit;
	}

	/* Analyze method if required */
	if ((methaccflags & ACC_VERIFIED) == 0) {
		codeinfo* codeInfo;
		bool success = analyzeMethod(meth, &codeInfo, &einfo);
		tidyAnalyzeMethod(&codeInfo);
		if (success == false) {
			throwError(&einfo);
		}
	}

	/* Allocate stack space and locals. */
	lcl = alloca(sizeof(slots) * (meth->localsz + meth->stacksz));

#if defined(KAFFE_VMDEBUG)
{
	int32 *p = (int32 *) &lcl[meth->localsz + meth->stacksz];
	while (p-- > (int32*)lcl)
		*p = UNINITIALIZED_STACK_SLOT;
}
#endif

	mobj = 0;
	npc = 0;

	/* If we have any exception handlers we must prepare to catch them.
	* We also need to catch if we are synchronised (so we can release it).
	*/
	setupExceptionHandling(&mjbuf, meth, mobj, thread_data);

	if (meth->exception_table != 0) {
		if (JTHREAD_SETJMP(mjbuf.jbuf) != 0) {
			meth = vmeth;
			thread_data->exceptPtr = &mjbuf;
			npc = vmExcept_getPC(&mjbuf);
			sp = &lcl[meth->localsz];
#if defined(KAFFE_VMDEBUG)
{
	int32 *p = (int32 *) &lcl[meth->localsz + meth->stacksz];
	while (p-- > (int32*)sp)
		*p = 0xdeadbeef;
}
#endif
			sp->v.taddr = (void*)thread_data->exceptObj;
			/* PS: set exception ref's taint. */
			sp->taint = thread_data->exceptTaint;

			// PS: cleared in runVirtualMachine, so it can setup exception taint
			//thread_data->exceptObj = 0;
			runVirtualMachine(meth, lcl, sp, npc, retval, &mjbuf, thread_data, new_context_taint, no_throw_taint);
			goto end;
		}
	}

	/* Calculate number of arguments */
	idx = sizeofSigMethod(meth, false);
	if (idx == -1) {
		postException(&einfo,  JAVA_LANG(InternalError));
		throwError(&einfo);
	}
	idx += (methaccflags & ACC_STATIC ? 0 : 1);

	/* Copy in the arguments */
	sp = lcl;
	for (low = 0; low < idx; low++) {
		*(sp++) = *(arg++);
	}

	/* Sync. if required */
	if (methaccflags & ACC_SYNCHRONISED) {
		if (methaccflags & ACC_STATIC) {
			mobj = &meth->class->head;
		}
		else {
			mobj = (Hjava_lang_Object*)lcl[0].v.taddr;
		}
		/* this lock is safe for Thread.stop() */
		locks_internal_lockMutex(&mobj->lock, 0);

		/*
		* We must store the object on which we synchronized
		* in the mjbuf chain for the exception handler
		* routine to find it (and unlock it when unwinding).
		*/
		vmExcept_setSyncObj(&mjbuf, mobj);
	}

	sp = &lcl[meth->localsz - 1];
	runVirtualMachine(meth, lcl, sp, npc, retval, &mjbuf, thread_data, new_context_taint, no_throw_taint);

 end:
 	// PS: Polymer finalizer actions
 	if (order)
 		polymerEngineAfter (meth, (jvalue *) retval, (Hjava_lang_Object *) thread_data->exceptObj, order);

		 /* Unsync. if required */
		 if (mobj != 0) {
	 locks_internal_unlockMutex(&mobj->lock, NULL);
		 }

		 cleanupExceptionHandling(&mjbuf, thread_data);


#if defined(ENABLE_JVMPI)
	soft_exit_method(meth);
#endif

// PS: possibly restore annotated method
annotatedExit:
	THREAD_DATA()->contextTaintStack = old_context_taint;

	if (trishulAnnotatedStore)
	{
		trishulAnnotatedInvokePop (trishulAnnotatedStore);
	}

RDBG(	dprintf("Returning from method %s%s.\n", meth->name->data, METHOD_SIGD(meth)); );
}

void runVirtualMachine (methods *meth, slots *lcl, slots *sp, uintp npc, slots *retval,
						volatile VmExceptHandler *mjbuf, threadData *thread_data,
						taint_t current_context_taint, taint_t *no_throw_taint) {
	bytecode *code = (bytecode*)meth->c.bcode.code;

	/* PS */
	ContextBasicBlock 		context_dummy;
	const ContextBasicBlock	*context_ptr;
	taint_t					*branch_context_taints;
	taint_t 				start_context_taint = current_context_taint;
	taint_t					no_throw_taint_temp;

	/* Disable optimization to make sure fallback mechanism works */
	bool					have_context_taints = meth->context_failed;
	if (meth->contexts)
	{
		context_ptr 			= meth->contexts;
		branch_context_taints	= (taint_t *) alloca (meth->bitmap_size_bits * sizeof (taint_t));
		memset (branch_context_taints, 	0, meth->bitmap_size_bits * sizeof (taint_t));

		if (thread_data->exceptObj)
		{
			thread_data->exceptObj = 0;

			/* PS: Set exception's taint in its context taint slot */
			/* First find the new context */
			context_ptr = NULL;
			uint i;
			for (i = 0; i < meth->exception_table->length; i++)
			{
				if (npc == meth->exception_table->entry[i].handler_pc)
				{
					context_ptr = meth->exception_table->entry[i].context;
					break;
				}
			}
			assert (context_ptr && (uint) context_ptr->bitmap_index < meth->bitmap_size_bits);

			/* Set the taint. The context taint will be rebuilt when the handler begins executing */
			branch_context_taints[context_ptr->bitmap_index] = thread_data->exceptTaint;
			/* Set the taint for the instruction that threw the exception */
			if (thread_data->throwContext)
			{
				branch_context_taints[thread_data->throwContext->bitmap_index] = thread_data->exceptTaint;

				if (thread_data->throwIndex >= 0)
				{
					// Taint locals & stacks in the current context
					taint_t context_dummy 	= 0;
					taint_t no_throw_dummy 	= 0;
					handle_branch (meth, npc, lcl, sp, thread_data->throwContext,
									thread_data->exceptTaint, thread_data->throwIndex + 1,
								   	&context_dummy, &no_throw_dummy, 0);
				}

				thread_data->throwContext = NULL;
			}

			if (thread_data->exceptTaint)
				have_context_taints = true;

			/* Switch to the real context for the catch code. This will trigger the rebuild of the context taint */
			context_ptr = findBasicBlock (meth, npc);
		}

		handle_new_context (meth, 0, context_ptr, branch_context_taints, start_context_taint, &current_context_taint);
	}
	else
	{
		//assert (!thread_data->exceptObj);
		thread_data->exceptObj = NULL;

		/* A dummy context is used for the entire method, to avoid extra ifs on each instruction */
		/* TODO: it might be more convenient to just use a single basic block in the graph */
		context_ptr				= &context_dummy;
		context_dummy.pc_start 	= 0;
		context_dummy.pc_end	= meth->c.bcode.codelen;
		branch_context_taints	= NULL;
	}

	/* PS- */
#ifdef DIRECT_THREADING

#define IH(name) &&INSN_LBL_##name
#define IH_INVALID &&INSN_LBL_INVALID

static const void *insn_handlers[] = {                                                  	/* opcodes */
    IH(NOP),		IH(ACONST_NULL),	IH(ICONST_M1), 		IH(ICONST_0),		/* 0-3 */
    IH(ICONST_1),	IH(ICONST_2),		IH(ICONST_3),		IH(ICONST_4),		/* 4-7 */
    IH(ICONST_5),	IH(LCONST_0),		IH(LCONST_1),		IH(FCONST_0),		/* 8-11 */
    IH(FCONST_1),	IH(FCONST_2),		IH(DCONST_0),		IH(DCONST_1),		/* 12-15 */
    IH(BIPUSH),		IH(SIPUSH),		IH(LDC1),		IH(LDC2),		/* 16-19 */
    IH(LDC2W),		IH(ILOAD),		IH(LLOAD),		IH(FLOAD),		/* 20-23 */
    IH(DLOAD),		IH(ALOAD),		IH(ILOAD_0),		IH(ILOAD_1),		/* 24-27 */
    IH(ILOAD_2),	IH(ILOAD_3),		IH(LLOAD_0),		IH(LLOAD_1),		/* 28-31 */
    IH(LLOAD_2),	IH(LLOAD_3),		IH(FLOAD_0),		IH(FLOAD_1),		/* 32-35 */
    IH(FLOAD_2),	IH(FLOAD_3),		IH(DLOAD_0),		IH(DLOAD_1),		/* 36-39 */
    IH(DLOAD_2),	IH(DLOAD_3),		IH(ALOAD_0),		IH(ALOAD_1),		/* 40-43 */
    IH(ALOAD_2),	IH(ALOAD_3),		IH(IALOAD),		IH(LALOAD),		/* 44-47 */
    IH(FALOAD),		IH(DALOAD),		IH(AALOAD),		IH(BALOAD),		/* 48-51 */
    IH(CALOAD),		IH(SALOAD),		IH(ISTORE),		IH(LSTORE),		/* 52-55 */
    IH(FSTORE),		IH(DSTORE),		IH(ASTORE),		IH(ISTORE_0),		/* 56-59 */
    IH(ISTORE_1),	IH(ISTORE_2),		IH(ISTORE_3),		IH(LSTORE_0),		/* 60-63 */
    IH(LSTORE_1),	IH(LSTORE_2),		IH(LSTORE_3),		IH(FSTORE_0),		/* 64-67 */
    IH(FSTORE_1),	IH(FSTORE_2),		IH(FSTORE_3),		IH(DSTORE_0),   	/* 68-71 */
    IH(DSTORE_1),	IH(DSTORE_2),		IH(DSTORE_3),		IH(ASTORE_0),   	/* 72-75 */
    IH(ASTORE_1),	IH(ASTORE_2),		IH(ASTORE_3),		IH(IASTORE),		/* 76-79 */
    IH(LASTORE),	IH(FASTORE),		IH(DASTORE),		IH(AASTORE),		/* 80-83 */
    IH(BASTORE),	IH(CASTORE),		IH(SASTORE),		IH(POP),		/* 84-87 */
    IH(POP2),		IH(DUP),		IH(DUP_X1),		IH(DUP_X2),		/* 88-91 */
    IH(DUP2),		IH(DUP2_X1),		IH(DUP2_X2),		IH(SWAP),		/* 92-95 */
    IH(IADD),		IH(LADD),		IH(FADD),		IH(DADD),		/* 96-99 */
    IH(ISUB),		IH(LSUB),		IH(FSUB),		IH(DSUB),		/* 100-103 */
    IH(IMUL),		IH(LMUL),		IH(FMUL),		IH(DMUL),		/* 104-107 */
    IH(IDIV),		IH(LDIV),		IH(FDIV),		IH(DDIV),		/* 108-111 */
    IH(IREM),		IH(LREM),		IH(FREM),		IH(DREM),		/* 112-115 */
    IH(INEG),		IH(LNEG),		IH(FNEG),		IH(DNEG),		/* 116-119 */
    IH(ISHL),		IH(LSHL),		IH(ISHR),		IH(LSHR),		/* 120-123 */
    IH(IUSHR),		IH(LUSHR),		IH(IAND),		IH(LAND),		/* 124-127 */
    IH(IOR),		IH(LOR),		IH(IXOR),		IH(LXOR),		/* 128-131 */
    IH(IINC),		IH(I2L),		IH(I2F),		IH(I2D),		/* 132-135 */
    IH(L2I),		IH(L2F),		IH(L2D),		IH(F2I),		/* 136-139 */
    IH(F2L),		IH(F2D),		IH(D2I),		IH(D2L),		/* 140-143 */
    IH(D2F),		IH(INT2BYTE),		IH(INT2CHAR),		IH(INT2SHORT),		/* 144-147 */
    IH(LCMP),		IH(FCMPL),		IH(FCMPG),		IH(DCMPL),		/* 148-151 */
    IH(DCMPG),		IH(IFEQ),		IH(IFNE),		IH(IFLT),		/* 152-155 */
    IH(IFGE),		IH(IFGT),		IH(IFLE),		IH(IF_ICMPEQ),		/* 156-159 */
    IH(IF_ICMPNE),	IH(IF_ICMPLT),		IH(IF_ICMPGE),		IH(IF_ICMPGT),		/* 160-163 */
    IH(IF_ICMPLE),	IH(IF_ACMPEQ),		IH(IF_ACMPNE),		IH(GOTO),		/* 164-167 */
    IH(JSR),		IH(RET),		IH(TABLESWITCH),	IH(LOOKUPSWITCH), 	/* 168-171 */
    IH(IRETURN),	IH(LRETURN),		IH(FRETURN),		IH(DRETURN),		/* 172-175 */
    IH(ARETURN),	IH(RETURN),		IH(GETSTATIC),		IH(PUTSTATIC),		/* 176-179 */
    IH(GETFIELD),	IH(PUTFIELD),		IH(INVOKEVIRTUAL),	IH(INVOKESPECIAL), 	/* 180-183 */
    IH(INVOKESTATIC),	IH(INVOKEINTERFACE),	IH_INVALID,		IH(NEW),		/* 184-187 */
    IH(NEWARRAY),	IH(ANEWARRAY),		IH(ARRAYLENGTH),	IH(ATHROW),		/* 188-191 */
    IH(CHECKCAST),	IH(INSTANCEOF),		IH(MONITORENTER),	IH(MONITOREXIT),	/* 192-195 */
    IH(WIDE),		IH(MULTIANEWARRAY),	IH(IFNULL),		IH(IFNONNULL),		/* 196-199 */
    IH(GOTO_W),		IH(JSR_W),		IH(BREAKPOINT),		IH_INVALID,		/* 200-203 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 204-207 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 208-211 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 212-215 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 216-219 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 220-223 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 224-227 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 228-231 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 232-235 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 236-239 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 240-243 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 244-247 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 248-251 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID		/* 252-255 */
};
 #undef IH
#endif

	/* Misc machine variables */
	jvalue tmpl;
	slots tmp[1];
	slots tmp2[1];
	slots mtable[1];

	/* Variables used directly in the machine */
	int32 idx;
	jint low;
	jint high;

	/* Call, field and creation information */
	callInfo cinfo;
	fieldInfo finfo;
	Hjava_lang_Class* crinfo;
	errorInfo einfo;

	/* Finally we get to actually execute the machine */
	for (;;) {
		register uintp pc = npc;
		trishul_context_check(pc,pc); /* PS */

		assert(npc < (uintp)(meth->c.bcode.codelen));
		vmExcept_setPC(mjbuf, pc);

		npc = pc + insnLen[code[pc]];

		INTRP_SW_HEAD() {
		    INTRP_SW_PROLOG();
#include "kaffe.def"

		} // INTRP_SW_HEAD()
	}
 end:
	return;
}

/*
 * say what engine we're using
 */
const char*
getEngine(void)
{
	return "kaffe.intr";
}

void initEngine(void)
{
}

static inline void
setFrame(VmExceptHandler* eh,
	 struct _jmethodID* meth,
	 struct Hjava_lang_Object* syncobj)
{
	assert(eh);
	assert(meth);

	eh->meth = meth;
	eh->frame.intrp.pc = 0;
	eh->frame.intrp.syncobj = syncobj;
}

void
setupExceptionHandling(VmExceptHandler* eh,
		       struct _jmethodID* meth,
		       struct Hjava_lang_Object* syncobj,
		       threadData*	thread_data)
{
  setFrame(eh, meth, syncobj);

  eh->prev = thread_data->exceptPtr;
  thread_data->exceptPtr = eh;
}

void
cleanupExceptionHandling(VmExceptHandler* eh,
			 threadData* thread_data)
{
  thread_data->exceptPtr = eh->prev;
}

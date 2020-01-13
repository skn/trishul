#include "polymer.h"
#include "thread.h"
#include "trishul.h"
#include "utils.h"
#include "stringSupport.h"
#include "file.h"
#include "readClass.h"
#include "machine.h"
/**********************************************************************************************************************/
/* Java class declarations */
/**********************************************************************************************************************/
static JAVA_CLASS 	(trishul_p_Engine, trishul_p/Engine);
	static JAVA_METHOD	(trishul_p_Engine, accept, 						"(Ltrishul_p/Order;)V");
	static JAVA_METHOD	(trishul_p_Engine, query, 						"(Ltrishul_p/Action;)Ltrishul_p/Order;");
	static JAVA_FIELD	(trishul_p_Engine, securityManager, 				"Ljava/lang/Object;");

static JAVA_CLASS 	(trishul_p_ResultOrder, trishul_p/ResultOrder);
	static JAVA_METHOD	(trishul_p_ResultOrder, handleResult, 		"(Ljava/lang/Object;Z)V");

static JAVA_CLASS 	(trishul_p_EngineSecurityManager, trishul_p/EngineSecurityManager);
	static JAVA_CTOR 	(trishul_p_EngineSecurityManager, ctor, 			"(Ljava/lang/SecurityManager;"
																		"Ljava/security/CodeSource;"
																		"Ljava/security/Policy;)V");
static JAVA_CLASS 	(java_lang_SecurityManager, java/lang/SecurityManager);
	static JAVA_CTOR 			(java_lang_SecurityManager, ctor, 		"()V");
	static JAVA_STATIC_FIELD	(java_lang_SecurityManager, current, 	"Ljava/lang/SecurityManager;");

static JAVA_CLASS 	(trishul_p_Action, trishul_p/Action);
	static JAVA_METHOD	(trishul_p_Action, getThisPointer,				"()Ljava/lang/Object;");
	static JAVA_METHOD	(trishul_p_Action, getActualParameter,			"(I)Ljava/lang/Object;");
	static JAVA_METHOD	(trishul_p_Action, getFormalParameter,			"(I)Ljava/lang/Class;");
	static JAVA_METHOD	(trishul_p_Action, getParameterCount,				"()I");

static JAVA_CLASS 	(trishul_p_AbstractAction, trishul_p/AbstractAction);
	static JAVA_METHOD	(trishul_p_AbstractAction, matches,				"(Ltrishul_p/Action;)Z");

static JAVA_CLASS 	(trishul_p_ConcreteAction, trishul_p/ConcreteAction);
	static JAVA_CTOR 	(trishul_p_ConcreteAction, ctor, 					"(Ljava/lang/Object;Ljava/lang/String;"
																		"Ljava/lang/String;Ljava/lang/String;"
																		"Ljava/lang/String;II"
																		"[Ljava/lang/Class;)V");
	static JAVA_METHOD	(trishul_p_ConcreteAction, getPackageName, 		"()Ljava/lang/String;");
	static JAVA_METHOD	(trishul_p_ConcreteAction, getClassName, 			"()Ljava/lang/String;");
	static JAVA_METHOD	(trishul_p_ConcreteAction, getMethodName, 		"()Ljava/lang/String;");
	static JAVA_METHOD	(trishul_p_ConcreteAction, getSignature, 			"()Ljava/lang/String;");
	static JAVA_METHOD	(trishul_p_ConcreteAction, getActualParameters,	"()[Ljava/lang/Object;");
	static JAVA_FIELD	(trishul_p_ConcreteAction, methodHandle, 			"I");

static JAVA_CLASS 	(trishul_p_ActionPattern, trishul_p/TrishulActionPattern);
	static JAVA_FIELD	(trishul_p_ActionPattern, modifier, 				"I");
	static JAVA_FIELD	(trishul_p_ActionPattern, returnType, 			"Ljava/lang/Class;");
	static JAVA_FIELD	(trishul_p_ActionPattern, packageName, 			"Ljava/lang/String;");
	static JAVA_FIELD	(trishul_p_ActionPattern, className, 				"Ljava/lang/String;");
	static JAVA_FIELD	(trishul_p_ActionPattern, methodName, 			"Ljava/lang/String;");
	static JAVA_FIELD	(trishul_p_ActionPattern, thisTaint,	 			"Ltrishul_p/Taint;");
	static JAVA_FIELD	(trishul_p_ActionPattern, parameters, 			"[Ltrishul_p/TrishulActionPattern$Parameter;");
	static JAVA_FIELD	(trishul_p_ActionPattern, anyParameters, 			"Z");
	static JAVA_FIELD	(trishul_p_ActionPattern, anyParametersTaint, 	"Ltrishul_p/Taint;");
	static JAVA_FIELD	(trishul_p_ActionPattern, contextTaint,		 	"Ltrishul_p/Taint;");
	static JAVA_FIELD	(trishul_p_ActionPattern, handle, 				"I");

static JAVA_CLASS 	(trishul_p_ActionPatternParameter, trishul_p/TrishulActionPattern$Parameter);
	static JAVA_FIELD	(trishul_p_ActionPatternParameter, type, 			"Ljava/lang/Class;");
	static JAVA_FIELD	(trishul_p_ActionPatternParameter, taint,		 	"trishul_p/Taint");

static JAVA_CLASS 	(trishul_p_DoneAction, trishul_p/DoneAction);
	static JAVA_CTOR 	(trishul_p_DoneAction, ctor, 		"(Ljava/lang/Throwable;)V");

static JAVA_CLASS 	(trishul_p_HaltException, trishul_p/HaltException);
	static JAVA_CTOR 	(trishul_p_HaltException, ctor, 	"()V");

static JAVA_CLASS 	(trishul_p_Order, trishul_p/Order);
	static JAVA_FIELD	(trishul_p_Order, triggerAction, 	"Ltrishul_p/Action;");
	static JAVA_METHOD	(trishul_p_Order, getType, 		"()I");

static JAVA_CLASS 	(trishul_p_ReplaceOrder, trishul_p/ReplaceOrder);
	static JAVA_METHOD	(trishul_p_ReplaceOrder, getReplaceValue, 	"()Ljava/lang/Object;");

static JAVA_CLASS 	(trishul_p_ExceptionOrder, trishul_p/ExceptionOrder);
	static JAVA_METHOD	(trishul_p_ExceptionOrder, getException, 	"()Ljava/lang/Exception;");

static JAVA_CLASS 	(trishul_p_InsertionOrder, trishul_p/InsertOrder);
	static JAVA_METHOD	(trishul_p_InsertionOrder, getInsertAction, 	"()Ltrishul_p/ConcreteAction;");

static JAVA_CLASS 	(trishul_p_TaintOrder, trishul_p/TaintOrder);
	static JAVA_METHOD	(trishul_p_TaintOrder, getTaint, 			"()Ltrishul_p/Taint;");

static JAVA_CLASS 	(trishul_p_RetValTaintOrder, trishul_p/RetValTaintOrder);

static JAVA_CLASS 	(trishul_p_ParamTaintOrder, trishul_p/ParamTaintOrder);
	static JAVA_METHOD	(trishul_p_ParamTaintOrder, getParameterIndex, 	"()I");

static JAVA_CLASS 	(trishul_p_ObjectTaintOrder, trishul_p/ObjectTaintOrder);
	static JAVA_METHOD	(trishul_p_ObjectTaintOrder, getObject, 	"()Ljava/lang/Object;");

static JAVA_CLASS 	(trishul_p_CompoundOrder, trishul_p/CompoundOrder);
	static JAVA_METHOD	(trishul_p_CompoundOrder, getOrders, 	"()[Ltrishul_p/Order;");

static JAVA_CLASS 	(trishul_p_Taint, trishul_p/Taint)
	static JAVA_METHOD	(trishul_p_Taint, getType, 	"()I");
	static JAVA_METHOD	(trishul_p_Taint, getTaint, 	"()I");

static JAVA_CLASS 	(trishul_p_TaintPattern, trishul_p/TaintPattern)
	static JAVA_METHOD	(trishul_p_TaintPattern, matchAll, 	"()Z");
	static JAVA_METHOD	(trishul_p_TaintPattern, getTaint, 	"()Ltrishul_p/Taint;");

static JAVA_CLASS 	(trishul_p_CompoundEngine, trishul_p/CompoundEngine)
	static JAVA_CTOR	(trishul_p_CompoundEngine, ctor, 			"()V");
	static JAVA_METHOD	(trishul_p_CompoundEngine, addEngine, 	"(Ltrishul_p/Engine;)V");
	static JAVA_METHOD	(trishul_p_CompoundEngine, removeEngine, 	"(Ltrishul_p/Engine;)V");

static JAVA_CLASS 	(java_lang_Class, java/lang/Class)
	static JAVA_METHOD	(java_lang_Class, newInstance, 	"()Ljava/lang/Object;");
	static JAVA_FIELD	(java_lang_Class, pd, 			"Ljava/security/ProtectionDomain;");

static JAVA_CLASS 	(trishul_p_EnginePolicyEmpty, trishul_p/EnginePolicyEmpty)
	static JAVA_CTOR	(trishul_p_EnginePolicyEmpty, ctor, 			"()V");

static JAVA_CLASS 	(trishul_p_EnginePolicyURL, trishul_p/EnginePolicyURL)
	static JAVA_CTOR	(trishul_p_EnginePolicyURL, ctor, 			"(Ljava/net/URL;)V");

static JAVA_CLASS 	(trishul_p_ProtectionDomain, trishul_p/ProtectionDomain);
	static JAVA_CTOR	(trishul_p_ProtectionDomain, ctor, "(Ljava/security/ProtectionDomain;Ljava/security/CodeSource;Ljava/security/Policy;)V");

static JAVA_CLASS 	(java_security_CodeSource, java/security/CodeSource);
	static JAVA_CTOR	(java_security_CodeSource, ctor, "(Ljava/net/URL;[Ljava/security/cert/Certificate;)V");


typedef enum
{
	polymerTaintTypePrimitive,
	polymerTaintTypeObject,
	polymerTaintTypeAuto,

	polymerTaintTypeNotSpecified = 0x7FFFFFFF
} trishul_p_Taint_TaintType;

static JAVA_CLASS 	(trishul_p_SuppressException, trishul_p/SuppressException);
static JAVA_CLASS 	(trishul_p_TrishulPException, 	trishul_p/TrishulPException);

#ifdef KAFFEVM_DEBUG
static JAVA_CLASS 	(trishul_p_TaintDebugLevelOrder, trishul_p/TaintDebugLevelOrder);
	static JAVA_METHOD	(trishul_p_TaintDebugLevelOrder, getDebugLevel, 	"()I");
#endif


#define PATTERNS_NAME	"__TRISHUL_PATTERNS"
#define PATTERNS_SIG	"[Ltrishul_p/TrishulActionPattern;"
/**********************************************************************************************************************/
uint polymerIteration = 0;
/**********************************************************************************************************************/

typedef enum
{
	polymerOrderOK,
	polymerOrderHalt,
	polymerOrderInsert,
	polymerOrderReplace,
	polymerOrderSuppress,
	polymerOrderException,
	polymerOrderTaintParameter,
	polymerOrderUntaintParameter,
	polymerOrderTaintReturnValue,
	polymerOrderUntaintReturnValue,
	polymerOrderTaintObject,
	polymerOrderUntaintObject,
	polymerOrderCompound,
	polymerOrderUntaintFallback,
#ifdef KAFFEVM_DEBUG
	polymerOrderBreakpoint,
	polymerOrderTaintDebugLevel,
#endif
} PolymerOrderType;


static Hjava_lang_Object	*polymerEngine;
static bool					is_initializing;

typedef struct TaintParsed
{
	taint_t 				taint;
	trishul_p_Taint_TaintType type;
} TaintParsed;

typedef struct TaintPattern
{
	TaintParsed	taint;
	bool		matchAll;
} TaintPattern;

static inline bool taintSpecified (const TaintParsed *taint)
{
	return (taint->type != polymerTaintTypeNotSpecified);
}

static inline bool taintPatternSpecified (const TaintPattern *pat)
{
	return taintSpecified (&pat->taint);
}

typedef struct PolymerParameter
{
	Hjava_lang_Class 	*class;
	TaintPattern		taint;
} PolymerParameter;

typedef struct PolymerPattern
{
	int 				modifier;
	jclass				returnClass;
	Hjava_lang_String	*packageName;
	Hjava_lang_String	*className;
	Hjava_lang_String	*methodName;
	TaintPattern		thisTaint;
	PolymerParameter	*parameters;
	int					parameter_count;
	bool				anyParameters;
	TaintPattern		anyParametersTaint;
	TaintPattern		contextTaint;
} PolymerPattern;

// TODO: rewrite allocation
static PolymerPattern 		polymerPatterns[1024];
static uint					polymerPatternsUsed;
static PolymerParameter		polymerParameters[1024];
static uint					polymerParametersUsed;


static void polymerPatternReserve (UNUSED uint count)
{
}

static PolymerPattern *polymerAllocPattern ()
{
	if (polymerPatternsUsed < sizeof (polymerPatterns) / sizeof (polymerPatterns[0]))
	{
		return &polymerPatterns[polymerPatternsUsed++];
	}

	printf ("Not enough room for patterns\n");
	abort ();
}

static PolymerParameter *polymerAllocParameters (uint count)
{
	if (polymerParametersUsed +count < sizeof (polymerParameters) / sizeof (polymerParameters[0]))
	{
		uint old = polymerParametersUsed;
		polymerParametersUsed += count;
		return &polymerParameters[old];
	}

	printf ("Not enough room for parameter patterns\n");
	abort ();
}

/**********************************************************************************************************************/
static void polymerRegisterPatternsFromClass (Hjava_lang_Class *class);
/**********************************************************************************************************************/

/**
 * Retrieves the engine object and checks if it is an trishul_p engine.
 * If it is not, NULL is returned.
 */
static Hjava_lang_Object *polymerGetEngine ()
{
	return polymerEngine;
}

/**********************************************************************************************************************/

static bool filterCompareActionString (const HString *s1, const Hjava_lang_Object *action, const JavaMethod *method)
{
	if (s1)
	{
		HString *s2 = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (jobject) action, method->method);
		exitOnException ();
//printf ("COMPARE: %s %s\n", stringJava2Utf8ConstReplace (s1, 0, 0)->data, stringJava2Utf8ConstReplace (s2, 0, 0)->data);
		bool ok = javaStringsEqual (s1, s2);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), s2);
		return ok;
	}

	return true;
}

/**********************************************************************************************************************/

static int getClassNameStart (Hjava_lang_Class *class)
{
	if (class->packageLength)
		return class->packageLength + 1;
	return 0;
}

/**********************************************************************************************************************/

static Method *getMethodFromDispatchAddress (void *addr)
{
#ifdef INTERPRETER
	return (Method *) addr;
#else
	// If the method is virtual, it might not be the method that will actually be invoked.
	// To find the actual method after polymorphism (i.e. the one that will be invoked, whereas the specified method
	// is the declaration in the base class), there are two possibilites:
	// 1) The call points to the actual code. In this case the code is preceded by a jitCodeHeader (jit3/machine.h).
	// 2) The call points to a trampoline. In this case the code is preceded by a methodTrampoline (i386/jit.h).
	// Either way, the structure can be used to retrieve the method. Case 2) is detected by checking if func
	// points to a call instruction (0xE8, call relative). The address is not checked, as other code
	// will not start with this instruction, but with code to setup the environmet
	uint8 *ptr = (uint8 *) addr;
	if (*ptr == 0xE8)
	{
		// This is a trampoline
		methodTrampoline *trampInfo = (methodTrampoline *) addr;
		return trampInfo->meth;
	}
	else
	{
		// Jitted code
		jitCodeHeader *jit = ((jitCodeHeader *) addr) - 1;
		return jit->method;
	}

#endif
}

/**********************************************************************************************************************/

static bool checkPolymorphicMatch (const Hjava_lang_String *filterPackage, const Hjava_lang_String *filterClass,
									const Method *method)
{

	bool match = !filterPackage || (javaStringEqualsUtf8ConstSubstring (filterPackage,
													          method->class->name, 0, method->class->packageLength));
	if (match)
	{
		match = !filterClass || (javaStringEqualsUtf8ConstSubstring (filterClass,
														    method->class->name, getClassNameStart (method->class), 0));
	}

	if (match) return true;

	// For some reason, simply using super->methods[method->idx] does not work
	Hjava_lang_Class *super = method->class->superclass;
	const Method *superMethod = (super && method->idx < super->msize && super->vtable->method[method->idx]) ?
						getMethodFromDispatchAddress (super->vtable->method[method->idx]) : NULL;
/*
	if (super) printf ("SUPER: %d %d\n", method->idx, (int) super->msize);
	printf ("METH: %s.%s %s.%s\n", method->class->name->data, method->name->data,
		superMethod ? superMethod->class->name->data : "", superMethod ? superMethod->name->data : "");
*/
	if (superMethod)
	{
		return checkPolymorphicMatch (filterPackage, filterClass, superMethod);
	}

	return false;
}

/**********************************************************************************************************************/

/**
 * Checks if the supplied method matches the filter.
 */
static bool polymerCheckFilter (Method *method, PolymerPattern *filter)
{
	// Check method name
	if (filter->methodName 	&& !utf8ConstEqualJavaString (method->name, filter->methodName))
	{
		return false;
	}

	// If the method name matches, but the class & package don't, this could be a polymorphic match
	bool normalCheck = true;

	if (filter->methodName && (filter->modifier & ACC_FINAL) == 0)
	{
		if ((method->accflags & ACC_STATIC) == 0)
		{
			normalCheck = false;
			if (!checkPolymorphicMatch (filter->packageName, filter->className, method))
			{
				return false;
			}
		}
	}

	if (normalCheck)
	{
		if (!
			(!filter->className  	|| utf8ConstSubStringEqualJavaString (method->class->name,
																		  getClassNameStart (method->class), 0,
																      	  filter->className))
			&&
			(!filter->packageName 	|| utf8ConstSubStringEqualJavaString (method->class->name,
																	  	  0, method->class->packageLength,
																	  	  filter->packageName))
			)
		{
			return false;
		}
	}

	// Return type
	if (filter->returnClass &&
		!classEqualsSignature  (filter->returnClass, METHOD_RET_TYPE (method)))
	{
		return false;
	}


	// Check explicit parameters
	if (filter->parameter_count > METHOD_NARGS (method))
	{
		return false;
	}

	int index;
	for (index = 0; index < filter->parameter_count; index++)
	{
		if (filter->parameters[index].class &&
			!classEqualsSignature (filter->parameters[index].class, METHOD_ARG_TYPE(method,index)))
		{
			return false;
		}
	}

	if (!filter->anyParameters && index < METHOD_NARGS (method))
	{
		return false;
	}
	return true;
}

/**********************************************************************************************************************/

static bool checkTaintGeneric (const TaintPattern *want, taint_t actual)
{
	taint_trace_native (1, ("Checking taint: %d: %X == %X", want->matchAll, want->taint.taint, actual));
	if (want->matchAll)
	{
		return ((want->taint.taint & actual) == want->taint.taint);
	}
	else
	{
		return ((want->taint.taint & actual) != 0);
	}
}

/**********************************************************************************************************************/

static bool checkTaintObject (const TaintPattern *want, const Hjava_lang_Object *actual)
{
	// TODO: properly use type. For this, the reference taint must be available.
	if (want->taint.type != polymerTaintTypeNotSpecified)
	{
		if (!actual)
			return false;

		if (!checkTaintGeneric (want, getObjectTaint (actual)))
		{
			return false;
		}
	}
	return true;
}

/**********************************************************************************************************************/

static bool parsedPatternMatchesConcreteAction (const PolymerPattern *filter,
												Hjava_lang_Object *absAction,
												const Hjava_lang_Object *action)
{
	if (!(*THREAD_JNIENV())->IsInstanceOf (THREAD_JNIENV(), (Hjava_lang_Object *) action,
																trishul_p_ConcreteAction.class))
		return false;

	// TODO: check if absAction and filter->modifiers match
	if (absAction)
	{
		// Let the abstract action test this
		if (!(*THREAD_JNIENV())->CallBooleanMethod (THREAD_JNIENV(), absAction,
													trishul_p_AbstractAction_matches.method,
													(Hjava_lang_Object *) action))
		{
			return false;
		}
	}
	else
	{

		// Check method name
		if (!filterCompareActionString (filter->methodName,  action, &trishul_p_ConcreteAction_getMethodName) )
		{
			return false;
		}

		// If the method name matches, but the class & package don't, this could be a polymorphic match
		bool normalCheck = true;

		if (filter->methodName && (filter->modifier & ACC_FINAL) == 0)
		{
			const Method *method = (Method *) (*THREAD_JNIENV())->GetIntField (THREAD_JNIENV(),
											(Hjava_lang_Object *) action, trishul_p_ConcreteAction_methodHandle.field);

			if ((method->accflags & ACC_STATIC) == 0)
			{
				normalCheck = false;
				if (!checkPolymorphicMatch (filter->packageName, filter->className, method))
				{
					return false;
				}
			}
		}

		if (normalCheck)
		{
			if (!filterCompareActionString (filter->className, action, &trishul_p_ConcreteAction_getClassName) ||
				!filterCompareActionString (filter->packageName, action, &trishul_p_ConcreteAction_getPackageName))
			{
				return false;
			}
		}

		// TODO: Return type
	}

	// Check this taint
	if (taintPatternSpecified (&filter->thisTaint))
	{
		const Hjava_lang_Object *this = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (jobject) action,
																trishul_p_Action_getThisPointer.method);
		bool ok = checkTaintObject (&filter->thisTaint, this);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), (Hjava_lang_Object *) this);
		if (!ok)
			return false;
	}

	// Check context taint
	if (taintPatternSpecified (&filter->contextTaint))
	{
		if (!checkTaintGeneric (&filter->contextTaint, nativeContextGetTaint ()))
			return false;
	}


	// Check parameters
	if (filter->parameters || taintPatternSpecified (&filter->anyParametersTaint))
	{
		int actual_count = (*THREAD_JNIENV())->CallIntMethod (THREAD_JNIENV(), (jobject) action,
															  trishul_p_Action_getParameterCount.method);
		exitOnException ();

		int index;

		if (filter->parameter_count > actual_count)
		{
			return false;
		}

		// Check explicit parameters
		for (index = 0; index < filter->parameter_count;  index++)
		{
			if (!absAction && filter->parameters[index].class)
			{
				// Check the parameter type
				Hjava_lang_Class *formal = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (jobject) action,
																trishul_p_Action_getFormalParameter.method, index);

				bool ok = classEqualsUnboxed (formal, filter->parameters[index].class);
				(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), formal);

				if (!ok)
				{
					return false;
				}
			}

			if (taintPatternSpecified (&filter->parameters[index].taint))
			{
				Hjava_lang_Object *actual = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (jobject) action,
															trishul_p_Action_getActualParameter.method, index);
				bool ok = checkTaintObject (&filter->parameters[index].taint, actual);
				(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), actual);

				if (!ok)
				{
					return false;
				}
			}
		}

		if (!filter->anyParameters && index < actual_count)
		{
			return false;
		}

		// Check taint on any parameter
		if (taintPatternSpecified (&filter->anyParametersTaint))
		{
			bool taintMatch = false;
			for (; index < actual_count && !taintMatch; index++)
			{
				Hjava_lang_Object *actual = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (jobject) action,
																	trishul_p_Action_getActualParameter.method, index);
				exitOnException ();
				taintMatch = checkTaintObject (&filter->anyParametersTaint, actual);
				(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), actual);
			}

			if (!taintMatch)
			{
				return false;
			}
		}
	}

	return true;
}

/**********************************************************************************************************************/

bool polymerCheckFilters (Method *method)
{
	// TODO: if no filter will be specified, the state is never set to polymerActionFilterNoMatch
	// Do nothing if the engine object is not yet specified, filters may still change
	if (polymerGetEngine () && !is_initializing)
	{
		uint i;
		for (i = 0; i < polymerPatternsUsed; i++)
		{
			if (polymerCheckFilter (method, &polymerPatterns[i]))
			{
				method->polymerMatch 			= true;
				method->polymerIterationChecked	= polymerIteration;
				//printf ("MATCH:  %p %s.%s %s %p\n", method, method->class->name->data, method->name->data, method->parsed_sig->signature->data, &method->polymerMatch);
				return true;
			}
		}

		//printf ("NO MATCH: %s.%s\n", method->class->name->data, method->name->data);
		method->polymerMatch 			= false;
		method->polymerIterationChecked	= polymerIteration;
	}

	return false;
}

/**********************************************************************************************************************/

static void parseTaint (Hjava_lang_Object *javaTaint, TaintParsed *parsed)
{
	if (javaTaint)
	{
		parsed->taint = (*THREAD_JNIENV())->CallIntMethod (THREAD_JNIENV(), javaTaint, trishul_p_Taint_getTaint.method);
		parsed->type  = (*THREAD_JNIENV())->CallIntMethod (THREAD_JNIENV(), javaTaint, trishul_p_Taint_getType.method);
		exitOnException ();
	}
	else
	{
		parsed->type 	= polymerTaintTypeNotSpecified;
	}
}

/**********************************************************************************************************************/

static void parseTaintPattern (Hjava_lang_Object *javaTaintPattern, TaintPattern *parsed)
{
	if (javaTaintPattern)
	{
		parsed->matchAll = (*THREAD_JNIENV())->CallBooleanMethod (THREAD_JNIENV(), javaTaintPattern,
																	trishul_p_TaintPattern_matchAll.method);
		exitOnException ();

		Hjava_lang_Object *taintObject = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), javaTaintPattern,
																	trishul_p_TaintPattern_getTaint.method);
		exitOnException ();
		parseTaint  (taintObject, &parsed->taint);

		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taintObject);
	}
	else
	{
		parsed->taint.type = polymerTaintTypeNotSpecified;
	}
}

/**********************************************************************************************************************/

static inline bool isObjectType (const char *sig)
{
	return (*sig == '[' || *sig == 'L' );
}

/**********************************************************************************************************************/
static PolymerOrderType polymerOrderGetType (const PolymerOrder *order);

static void polymerTaintFromOrder (const PolymerOrder *order, const char *sig,
																	jvalue *value, bool doTaint)
{
	(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 2);

	Hjava_lang_Object *taintObj = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(),
						(PolymerOrder *) order, trishul_p_TaintOrder_getTaint.method);
	exitOnException ();
	TaintParsed taint;
	parseTaint (taintObj, &taint);
	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taintObj);

	switch (taint.type)
	{
	case polymerTaintTypePrimitive:
		if (doTaint) taintAdd1 (value->taint, taint.taint);
		else taintSub1 (value->taint, taint.taint);
		break;
	case polymerTaintTypeObject:
		if (!isObjectType (sig))
		{
			throwTypedException (&trishul_p_TrishulPException, "TAINT_OBJECT only allowed for object and array values");
		}
		else
		{
			if (doTaint) taintObject (value->l, taint.taint);
			else untaintObject (value->l, taint.taint);
		}
		break;
	case polymerTaintTypeAuto:
		if (isObjectType (sig))
		{
			if (doTaint) taintObject (value->l, taint.taint);
			else untaintObject (value->l, taint.taint);
		}

		if (doTaint) taintAdd1 (value->taint, taint.taint);
		else taintSub1 (value->taint, taint.taint);
		break;
	default:
		fprintf (stderr, "UNKNOWN TAINT TYPE\n");
		abort ();
	}
}

/**********************************************************************************************************************/

static jmethodID polymerStringsToMethod (Hjava_lang_String *sPackageName, Hjava_lang_String *sClassName,
										 Hjava_lang_String *sMethodName, Hjava_lang_String *sSignature,
										 bool isFullSignature, bool mustExist)
{
	// Concatenate the package and class name
	StringDecode packageName, className;
	decodeString (sPackageName, &packageName);
	decodeString (sClassName, &className);

	char buffer[1024];
	int offset = 0;
	if (packageName.length)
	{
		memcpy (buffer, packageName.data, packageName.length);
		offset += packageName.length;
		buffer[offset] = '.';
		offset++;
	}
	memcpy (buffer + offset, className.data, className.length);
	buffer[className.length + offset] = 0;

	releaseDecodedString (sPackageName, &packageName);
	releaseDecodedString (sClassName, &className);

	// Lookup the class
	jclass cls = (*THREAD_JNIENV())->FindClass(THREAD_JNIENV(), buffer);
	exitOnException ();

	// Find the method
	Utf8Const *methodName 	= stringJava2Utf8ConstReplace (sMethodName, 0, 0);
	Utf8Const *signature 	= stringJava2Utf8ConstReplace (sSignature, 0, 0);
	jmethodID meth = findMethodNoReturnType (cls, methodName, signature);
	if (meth &&
		isFullSignature &&
		!utf8ConstEqual (METHOD_SIG(meth), signature)
	)
	{
		meth = NULL;
	}
	if (!meth && mustExist)
	{
		throwNamedException (JAVA_LANG(NoSuchMethodError), "%s %s", methodName->data, signature->data);
	}
	return meth;
}

/**********************************************************************************************************************/

static jmethodID polymerConcreteActionToMethod (Hjava_lang_Object *action)
{
	Hjava_lang_String *packageName;
	Hjava_lang_String *className;
	Hjava_lang_String *methodName;
	Hjava_lang_String *signature;

	packageName = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), action,
														trishul_p_ConcreteAction_getPackageName.method);
	className 	= (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), action,
														trishul_p_ConcreteAction_getClassName.method);
	methodName 	= (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), action,
														trishul_p_ConcreteAction_getMethodName.method);
	signature 	= (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), action,
														trishul_p_ConcreteAction_getSignature.method);
	exitOnException ();

	return polymerStringsToMethod (packageName, className, methodName, signature, false, true);
}

/**********************************************************************************************************************/

static void polymerParsePatternParameters (Hjava_lang_Object *pattern, PolymerPattern *parsed)
{
	HArrayOfObject *array = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
															pattern, trishul_p_ActionPattern_parameters.field);
	if (!array)
	{
		parsed->parameter_count = 0;
		parsed->parameters 		= NULL;
	}
	else
	{
		parsed->parameter_count = ARRAY_SIZE (array);
		parsed->parameters		= polymerAllocParameters (parsed->parameter_count);

		int i;
		for (i = 0; i < parsed->parameter_count; i++)
		{
			Hjava_lang_Object *param = OBJARRAY_DATA (array)[i];

			parsed->parameters[i].class = MakeGlobalRef ((*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(), param,
										trishul_p_ActionPatternParameter_type.field));

			Hjava_lang_Object *taint = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
															param, trishul_p_ActionPatternParameter_taint.field);
			exitOnException ();
			parseTaintPattern (taint, &parsed->parameters[i].taint);
			(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taint);
		}

		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), array);
	}
}

/**********************************************************************************************************************/

static void polymerRegisterPatterns (Hjava_lang_Object **patterns, uint patternCount)
{
	uint i;

	polymerPatternReserve (patternCount);

	for (i = 0; i < patternCount; i++)
	{

		(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 9);

		Hjava_lang_Object *pattern = patterns[i];
		PolymerPattern *parsed = polymerAllocPattern ();
		parsed->modifier = (*THREAD_JNIENV())->GetIntField (THREAD_JNIENV(),
								pattern, trishul_p_ActionPattern_modifier.field);
		if (parsed->modifier & ACC_ABSTRACT)
		{
			Hjava_lang_Class *absClass1 = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(), pattern,
																	trishul_p_ActionPattern_returnType.field);
			// Ensure the class is fully loaded
			jclass absClass = (*THREAD_JNIENV())->FindClass (THREAD_JNIENV(), absClass1->name->data);
			polymerRegisterPatternsFromClass (absClass1);
			(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), absClass1);
			(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), absClass);
		}
		else
		{
			parsed->returnClass		= MakeGlobalRef ((*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
																	pattern, trishul_p_ActionPattern_returnType.field));
			parsed->packageName 	= MakeGlobalRef ((*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
																	pattern, trishul_p_ActionPattern_packageName.field));
			parsed->className 		= MakeGlobalRef ((*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
																	pattern, trishul_p_ActionPattern_className.field));
			parsed->methodName 		= MakeGlobalRef ((*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
																	pattern, trishul_p_ActionPattern_methodName.field));
		}

		polymerParsePatternParameters (pattern, parsed);

		// Any parameter + taint
		parsed->anyParameters 	= (*THREAD_JNIENV())->GetBooleanField (THREAD_JNIENV(),
																pattern, trishul_p_ActionPattern_anyParameters.field);

		Hjava_lang_Object *taint = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
														pattern, trishul_p_ActionPattern_anyParametersTaint.field);
		exitOnException ();
		parseTaintPattern (taint, &parsed->anyParametersTaint);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taint);

		// This taint
		taint = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
														pattern, trishul_p_ActionPattern_thisTaint.field);
		exitOnException ();
		parseTaintPattern (taint, &parsed->thisTaint);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taint);

		// Context taint
		taint = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
														pattern, trishul_p_ActionPattern_contextTaint.field);
		exitOnException ();
		parseTaintPattern (taint, &parsed->contextTaint);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taint);

		// Store the handle
		// TODO: is this a security risk?
		(*THREAD_JNIENV())->SetIntField	(THREAD_JNIENV(), pattern,
										 trishul_p_ActionPattern_handle.field, (int) parsed);
		exitOnException ();

		// TODO: could resolve directly if package, class and method are specified.
	}

	exitOnException ();
}

/**********************************************************************************************************************/

static void polymerRegisterPatternsFromClass (Hjava_lang_Class *class)
{
	jfieldID field = (*THREAD_JNIENV())->GetStaticFieldID (THREAD_JNIENV(), class, PATTERNS_NAME, PATTERNS_SIG);
	if (!clearException ())
	{
		HArrayOfObject *patterns = (*THREAD_JNIENV())->GetStaticObjectField (THREAD_JNIENV(), class, field);
		exitOnException ();
		polymerRegisterPatterns (OBJARRAY_DATA (patterns), ARRAY_SIZE (patterns));
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), patterns);
	}

	// TODO: use hashtable to avoid reloading same class
	if (class->superclass)
	{
		polymerRegisterPatternsFromClass (class->superclass);
	}
}

/**********************************************************************************************************************/

static void ensureLoaded ()
{
	ensureMethodLoaded (&java_lang_Class_newInstance);
	ensureFieldLoaded  (&trishul_p_ActionPattern_modifier);
	ensureFieldLoaded  (&trishul_p_ActionPattern_returnType);
	ensureFieldLoaded  (&trishul_p_ActionPattern_packageName);
	ensureFieldLoaded  (&trishul_p_ActionPattern_className);
	ensureFieldLoaded  (&trishul_p_ActionPattern_methodName);
	ensureFieldLoaded  (&trishul_p_ActionPattern_thisTaint);
	ensureFieldLoaded  (&trishul_p_ActionPattern_parameters);
	ensureFieldLoaded  (&trishul_p_ActionPattern_anyParameters);
	ensureFieldLoaded  (&trishul_p_ActionPattern_anyParametersTaint);
	ensureFieldLoaded  (&trishul_p_ActionPattern_contextTaint);
	ensureFieldLoaded  (&trishul_p_ActionPattern_handle);
	ensureFieldLoaded  (&trishul_p_ActionPatternParameter_type);
	ensureFieldLoaded  (&trishul_p_ActionPatternParameter_taint);
	ensureMethodLoaded (&trishul_p_Action_getThisPointer);
	ensureMethodLoaded (&trishul_p_Action_getActualParameter);
	ensureMethodLoaded (&trishul_p_Action_getFormalParameter);
	ensureMethodLoaded (&trishul_p_Action_getParameterCount);
	ensureMethodLoaded (&trishul_p_AbstractAction_matches);
	ensureMethodLoaded (&trishul_p_CompoundOrder_getOrders);
	ensureMethodLoaded (&trishul_p_ConcreteAction_ctor);
	ensureMethodLoaded (&trishul_p_ConcreteAction_getPackageName);
	ensureMethodLoaded (&trishul_p_ConcreteAction_getClassName);
	ensureMethodLoaded (&trishul_p_ConcreteAction_getMethodName);
	ensureMethodLoaded (&trishul_p_ConcreteAction_getSignature);
	ensureMethodLoaded (&trishul_p_ConcreteAction_getActualParameters);
	ensureFieldLoaded  (&trishul_p_ConcreteAction_methodHandle);
	ensureMethodLoaded (&trishul_p_DoneAction_ctor);
	ensureMethodLoaded (&trishul_p_ExceptionOrder_getException);
	ensureMethodLoaded (&trishul_p_HaltException_ctor);
	ensureMethodLoaded (&trishul_p_InsertionOrder_getInsertAction);
	ensureMethodLoaded (&trishul_p_ObjectTaintOrder_getObject);
	ensureMethodLoaded (&trishul_p_ParamTaintOrder_getParameterIndex);
	ensureMethodLoaded (&trishul_p_Engine_accept);
	ensureMethodLoaded (&trishul_p_Engine_query);
	ensureMethodLoaded (&trishul_p_ResultOrder_handleResult);
	ensureClassLoaded  (&trishul_p_TrishulPException);
	ensureMethodLoaded (&trishul_p_ReplaceOrder_getReplaceValue);
	ensureClassLoaded  (&trishul_p_RetValTaintOrder);
	ensureMethodLoaded (&trishul_p_Order_getType);
	ensureFieldLoaded  (&trishul_p_Order_triggerAction);
	ensureClassLoaded  (&trishul_p_SuppressException);
	ensureMethodLoaded (&trishul_p_Taint_getTaint);
	ensureMethodLoaded (&trishul_p_Taint_getType);
#ifdef KAFFEVM_DEBUG
	ensureMethodLoaded (&trishul_p_TaintDebugLevelOrder_getDebugLevel);
#endif
	ensureMethodLoaded (&trishul_p_TaintPattern_matchAll);
	ensureMethodLoaded (&trishul_p_TaintPattern_getTaint);
	ensureMethodLoaded (&trishul_p_TaintOrder_getTaint);
	ensureMethodLoaded (&trishul_p_CompoundEngine_ctor);
	ensureMethodLoaded (&trishul_p_CompoundEngine_addEngine);
	ensureMethodLoaded (&trishul_p_CompoundEngine_removeEngine);
	ensureMethodLoaded (&trishul_p_ProtectionDomain_ctor);
	ensureMethodLoaded (&trishul_p_EnginePolicyEmpty_ctor);
	ensureMethodLoaded (&trishul_p_EnginePolicyURL_ctor);
	ensureFieldLoaded  (&trishul_p_Engine_securityManager);
	ensureMethodLoaded (&trishul_p_EngineSecurityManager_ctor);
	ensureFieldLoaded  (&java_lang_SecurityManager_current);
	ensureMethodLoaded (&java_lang_SecurityManager_ctor);
	ensureFieldLoaded  (&java_lang_Class_pd);
	ensureMethodLoaded (&java_security_CodeSource_ctor);
}

/**********************************************************************************************************************/

static Hjava_lang_Object *loadEnginePolicy (UNUSED Hjava_lang_Object *this, Hjava_lang_Object *enginePolicyURL)
{
	// TODO: handle error in engine.refresh that is invoked by the ctor
	ensureMethodLoaded (&trishul_p_EnginePolicyURL_ctor);
	Hjava_lang_Object *enginePolicy = MakeGlobalRef ((*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
							trishul_p_EnginePolicyURL.class, trishul_p_EnginePolicyURL_ctor.method, enginePolicyURL));
	exitOnException ();
	return enginePolicy;
}

/**********************************************************************************************************************/

Hjava_lang_Object *loadEnginePolicyFromURL (const char *url)
{
	Hjava_lang_Object *urlObject = urlFromCString (url);
	Hjava_lang_Object *enginePolicy = loadEnginePolicy (NULL, urlObject);
	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), urlObject);
	exitOnException ();
	return enginePolicy;
}

/**********************************************************************************************************************/

static Hjava_lang_Object *getDisallowEnginePolicy (UNUSED Hjava_lang_Object *this)
{
	// Create default engine with no permissions
	Hjava_lang_Object *enginePolicy = MakeGlobalRef ((*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
						trishul_p_EnginePolicyEmpty.class, trishul_p_EnginePolicyEmpty_ctor.method));
	return enginePolicy;
}

/**********************************************************************************************************************/
/*
*/
#if defined (POLYMER_POLICY_USE_PROTECTION_DOMAIN) || defined (POLYMER_POLICY_USE_SECURITY_MANAGER)

static void initializeEngineProtection (Hjava_lang_Object *initiator,
										Hjava_lang_Object *engine, Hjava_lang_Object *enginePolicy)
{
	if (enginePolicy)
	{
		// Create the code source
		char buffer[1024];
		// TODO: what if the buffer is too small?
		snprintf (buffer, sizeof (buffer), "http://trishul/%s", engine->vtable->class->name->data);

		Hjava_lang_Object *url = urlFromCString (buffer);
		Hjava_lang_Object *codeSource = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
													java_security_CodeSource.class,
													java_security_CodeSource_ctor.method, url, NULL);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), url);


#if defined (POLYMER_POLICY_USE_SECURITY_MANAGER)

		// Disable the current security manager temporarily, to ensure creating the new security manager
		// will be allowed
		Hjava_lang_Object *currentSecMan = (*THREAD_JNIENV())->GetStaticObjectField (THREAD_JNIENV(),
																java_lang_SecurityManager.class,
															   	java_lang_SecurityManager_current.field);
		(*THREAD_JNIENV())->SetStaticObjectField (THREAD_JNIENV(), java_lang_SecurityManager.class,
															   java_lang_SecurityManager_current.field,
															   NULL);


		/* Create the security manager for this engine */
		Hjava_lang_Object *oldSecMan = initiator ? (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
													initiator, trishul_p_Engine_securityManager.field) : NULL;
		Hjava_lang_Object *secMan = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
													trishul_p_EngineSecurityManager.class,
													trishul_p_EngineSecurityManager_ctor.method,
													oldSecMan, codeSource, enginePolicy);
		(*THREAD_JNIENV())->SetObjectField (THREAD_JNIENV(), engine, trishul_p_Engine_securityManager.field, secMan);
		if (oldSecMan) (*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), oldSecMan);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), secMan);

		// Reset the existing security manager
		if (currentSecMan)
		{
			(*THREAD_JNIENV())->SetStaticObjectField (THREAD_JNIENV(), java_lang_SecurityManager.class,
																   java_lang_SecurityManager_current.field,
																   currentSecMan);
			(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), currentSecMan);
		}

#elif defined (POLYMER_POLICY_USE_PROTECTION_DOMAIN)

		Hjava_lang_Object *currentDomain;
		if (!initiatorClass)
		{
			currentDomain = NULL;
		}
		else
		{
			currentDomain = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
																initiatorClass, java_lang_Class_pd.field);
		}
		struct Hjava_security_ProtectionDomain *pd = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
														trishul_p_ProtectionDomain.class,
														trishul_p_ProtectionDomain_ctor.method,
														currentDomain, codeSource, enginePolicy);
		if (!checkException ())
			return;
		(*THREAD_JNIENV())->SetObjectField (THREAD_JNIENV(), engineClass, java_lang_Class_pd.field, pd);
		if (currentDomain) (*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), currentDomain);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), pd);
#endif
	}
}

#endif
/**********************************************************************************************************************/

Hjava_lang_Object *polymerInitializeEngineObject (Hjava_lang_Object *initiator,
												  Hjava_lang_Object *engine, 
												  Hjava_lang_Object *enginePolicy)
{
	if (classExtends (engine->vtable->class, &trishul_p_Engine))
	{
		ensureLoaded ();
		is_initializing = true;

#if defined (POLYMER_POLICY_USE_PROTECTION_DOMAIN) || defined (POLYMER_POLICY_USE_SECURITY_MANAGER)
		initializeEngineProtection (initiator, engine, enginePolicy);
		if (!checkException ())
		{
			return NULL;
		}
#endif

		if (polymerEngine)
		{
			if (!(*THREAD_JNIENV())->IsInstanceOf (THREAD_JNIENV(), polymerEngine, trishul_p_CompoundEngine.class))
			{
				// Create a new compound engine
				Hjava_lang_Object *newEngine = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
												trishul_p_CompoundEngine.class, trishul_p_CompoundEngine_ctor.method);
				(*THREAD_JNIENV())->CallVoidMethod (THREAD_JNIENV(), newEngine,
													trishul_p_CompoundEngine_addEngine.method, polymerEngine);
				exitOnException ();
				polymerEngine = newEngine;
			}

			// Add the specified engine to the compound engine
			(*THREAD_JNIENV())->CallVoidMethod (THREAD_JNIENV(), polymerEngine,
												trishul_p_CompoundEngine_addEngine.method, engine);
			exitOnException ();

		}
		else
		{
			polymerEngine = engine;
		}

		polymerRegisterPatternsFromClass (engine->vtable->class);
		is_initializing = false;
		polymerIteration++;

		return polymerEngine;
	}
	return engine;
}

/**********************************************************************************************************************/

static bool polymerCheckNativeThisPointer (Hjava_lang_Object *engine)
{
	if (engine != polymerGetEngine ())
	{
		throwTypedException (&trishul_p_TrishulPException, "Engine object has wrong this pointer");
		return false;
	}

	return true;
}

/**********************************************************************************************************************/

static jboolean sharedMatchesAction (Hjava_lang_Object *pattern,
									 Hjava_lang_Object *absAction,
									 Hjava_lang_Object *action)
{
	PolymerPattern *parsed = (PolymerPattern *) (*THREAD_JNIENV())->GetIntField	(THREAD_JNIENV(), pattern,
															 	 trishul_p_ActionPattern_handle.field);
	exitOnException ();
	assert (parsed);

	if (!classExtends (action->vtable->class, &trishul_p_ConcreteAction))
		return false;

	return parsedPatternMatchesConcreteAction (parsed, absAction, action);
}

/**********************************************************************************************************************/

static jboolean patternMatchesAction (UNUSED Hjava_lang_Object *engine, Hjava_lang_Object *pattern,
									  Hjava_lang_Object *absAction, Hjava_lang_Object *action)
{
	// TODO: this should probably be enabled, but compound policies make checking this difficult
	//if (polymerCheckNativeThisPointer (engine))
	{
		return sharedMatchesAction (pattern, absAction, action);
	}
	return false;
}

/**********************************************************************************************************************/

static jboolean abstractActionMatchesAction (UNUSED Hjava_lang_Object *this, Hjava_lang_Object *pattern,
									  		 Hjava_lang_Object *action)
{
	// TODO: check this pointer?
	return sharedMatchesAction (pattern, NULL, action);
}

/**********************************************************************************************************************/

static Hjava_lang_Object *addEngineFromClass (UNUSED Hjava_lang_Object *this,
											  Hjava_lang_Object *engineClass, Hjava_lang_Object *enginePolicy)
{
	// TODO: this should probably be enabled, but compound policies make checking this difficult
	//if (polymerCheckNativeThisPointer (this))
	{
		if (!engineClass)
		{
			throwNamedException ("java/lang/IllegalArgumentException", "engineClass must be specified");
			return NULL;
		}

		Hjava_lang_Object *engine = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), engineClass,
																		  java_lang_Class_newInstance.method);
		if (!engine)
			return NULL;
		bool failed = polymerInitializeEngineObject (this, engine, enginePolicy) == NULL;
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), engine);
		return (failed ? NULL : engine);
	}
	return NULL;
}

/**********************************************************************************************************************/

static Hjava_lang_Object *addEngineFromBytes (UNUSED Hjava_lang_Object *this, HArrayOfByte *bytes,
											  int offset, int length, Hjava_lang_Object *enginePolicy)
{
	// TODO: this should probably be enabled, but compound policies make checking this difficult
	//if (polymerCheckNativeThisPointer (this))
	{
		static jclass mcls;
		static jmethodID mmth;

		/* Quickhack to load java_lang_VMClassLoader_defineClass, even when it is not linked in */
		if (!mmth)
		{
			mcls = MakeGlobalRef ((*THREAD_JNIENV())->FindClass(THREAD_JNIENV(), "java/lang/VMClassLoader"));
			mmth = MakeGlobalRef ((*THREAD_JNIENV())->GetStaticMethodID(THREAD_JNIENV(),
	    			mcls, "defineClass", "(Ljava/lang/ClassLoader;Ljava/lang/String;[BIILjava/security/ProtectionDomain;)Ljava/lang/Class;"));
		}


		// Hook into the classloader to load the class from the array of bytes.
		Hjava_lang_Object *cls = (*THREAD_JNIENV())->CallStaticObjectMethod (THREAD_JNIENV(), mcls, mmth, NULL, NULL, bytes, offset, length, NULL);
		Hjava_lang_Object *pol = addEngineFromClass (this, cls, enginePolicy);
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), cls);
		return pol;
	}
	return NULL;
}

/**********************************************************************************************************************/

static void removeEngine (UNUSED Hjava_lang_Object *this, Hjava_lang_Object *engine)
{
	// TODO: this should probably be enabled, but compound policies make checking this difficult
	//if (polymerCheckNativeThisPointer (this))
	{
		if (!engine)
		{
			throwNamedException ("java/lang/NullPointerException", "");
		}
		else if (!(*THREAD_JNIENV())->IsInstanceOf (THREAD_JNIENV(), polymerEngine, trishul_p_CompoundEngine.class))
		{
			throwNamedException ("java/lang/IllegalArgumentException", "Engine is not a compound engine");
		}
		else
		{
			// TODO: an exception if the engine was not found?

			// Remove the specified engine from the compound engine
			(*THREAD_JNIENV())->CallVoidMethod (THREAD_JNIENV(), polymerEngine,
												trishul_p_CompoundEngine_removeEngine.method, engine);
			exitOnException ();
		}
	}
}

/**********************************************************************************************************************/

static PolymerOrder *doQuery (Hjava_lang_Object *this, const Hjava_lang_Object *action)
{
#ifdef POLYMER_POLICY_USE_SECURITY_MANAGER
	(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 4);

	// Install new security manager
	Hjava_lang_Object *currentSecMan = (*THREAD_JNIENV())->GetStaticObjectField (THREAD_JNIENV(),
																java_lang_SecurityManager.class,
															   	java_lang_SecurityManager_current.field);
	Hjava_lang_Object *secMan = (*THREAD_JNIENV())->GetObjectField (THREAD_JNIENV(),
																this,
																trishul_p_Engine_securityManager.field);
	(*THREAD_JNIENV())->SetStaticObjectField (THREAD_JNIENV(), java_lang_SecurityManager.class,
															   java_lang_SecurityManager_current.field,
															   secMan);
	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), secMan);
#endif


	PolymerOrder *ret = (PolymerOrder *) (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), this,
												  trishul_p_Engine_query.method, action);
#ifdef POLYMER_POLICY_USE_SECURITY_MANAGER
	// Install old security manager
	(*THREAD_JNIENV())->SetStaticObjectField (THREAD_JNIENV(), java_lang_SecurityManager.class,
															   java_lang_SecurityManager_current.field,
															   currentSecMan);
	if (currentSecMan) (*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), currentSecMan);
#endif

	return ret;
}

/**********************************************************************************************************************/

const BuiltinFunctionInfo_t DISPATCH_trishul_p_Engine[] =
{
	{
		"__TRISHUL_MATCHES",
		"(Ltrishul_p/TrishulActionPattern;Ltrishul_p/AbstractAction;Ltrishul_p/Action;)Z",
		patternMatchesAction
	},

	{
		"addEngine",
		"(Ljava/lang/Class;Ljava/security/Engine;)Ltrishul_p/Engine$EngineHandle;",
		addEngineFromClass
	},

	{
		"addEngine",
		"([BIILjava/security/Engine;)Ltrishul_p/Engine$EngineHandle;",
		addEngineFromBytes
	},

	{
		"loadEnginePolicy",
		"(Ljava/net/URL;)Ljava/security/Engine;",
		loadEnginePolicy
	},

	{
		"getDisallowEnginePolicy",
		"()Ljava/security/Engine;",
		getDisallowEnginePolicy
	},

	{
		"removeEngine",
		"(Ltrishul_p/Engine$EngineHandle;)V",
		removeEngine
	},

	{
		"doQuery",
		"(Ltrishul_p/Action;)Ltrishul_p/Order;",
		doQuery
	},
	{NULL, NULL, NULL}
};

/**********************************************************************************************************************/

const BuiltinFunctionInfo_t DISPATCH_trishul_p_AbstractAction[] =
{
	{"__TRISHUL_MATCHES",		"(Ltrishul_p/TrishulActionPattern;Ltrishul_p/Action;)Z",	abstractActionMatchesAction},
	{NULL, NULL, NULL}
};

/**********************************************************************************************************************/

void ClearContextTaint ();

static Hjava_lang_Object *jitParametersToObjectArray (const Method *method, void *args)
{
	ClearContextTaint ();

	uint i;
	Hjava_lang_Object *array = (*THREAD_JNIENV())->NewObjectArray (THREAD_JNIENV(), METHOD_NARGS (method),
		(*THREAD_JNIENV())->FindClass (THREAD_JNIENV(), "java/lang/Object"), NULL);

	void 	**argPtr 	= (void **) args;
	taint_t *taintPtr	= (taint_t *) (argPtr + METHOD_NARGS (method));

#ifdef STACK_LIMIT
	taintPtr++;
#endif

	if (!(method->accflags & ACC_STATIC))
		taintPtr++; // Skip this-pointer taint

	for (i = 0; i < METHOD_NARGS (method); i++, argPtr++, taintPtr++)
	{
		jvalue val;
		switch (METHOD_ARG_TYPE(method, i)[0])
		{
		case 'D':
		case 'J':
			// For double-sized values, taint and value are switched
			val.l 		= argPtr[1];
			val.j 		<<= 32;
			val.l 		= argPtr[0];
			val.taint 	= *taintPtr;
			taintPtr++;
			argPtr++;
			break;
		default:
			val.l 		= *argPtr;
			val.taint 	= *taintPtr;
			break;
		}

		OBJARRAY_DATA(array)[i] = boxIfRequired (METHOD_ARG_TYPE (method, i), &val);
	}

	return array;
}

/**********************************************************************************************************************/

static HArrayOfObject *concreteActionLoadParameters (Method *method, void *args)
{
#ifdef JIT3
	return (HArrayOfObject *) jitParametersToObjectArray (method, args);
#else
	return (HArrayOfObject *) parametersToObjectArray (method, args);
#endif
}

/**********************************************************************************************************************/

const BuiltinFunctionInfo_t DISPATCH_trishul_p_ConcreteAction[] =
{
	{"loadParameters",		"(II)[Ljava/lang/Object;",	concreteActionLoadParameters},
	{NULL, NULL, NULL}
};

/**********************************************************************************************************************/

static void polymerDone (const Hjava_lang_Object *x)
{
	Hjava_lang_Object *action = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(), trishul_p_DoneAction_ctor.class->class,
										trishul_p_DoneAction_ctor.method, x);
	Hjava_lang_Object *order = (Hjava_lang_Object *) doQuery (polymerGetEngine (), action);
	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), action);
	if (order)
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), order);
}

/**********************************************************************************************************************/

void polymerTerminating	()
{
	if (polymerGetEngine ())
	{
		Hjava_lang_Object *x = (*THREAD_JNIENV())->ExceptionOccurred(THREAD_JNIENV());
		polymerDone (x);
		if (x)
		{
			(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), x);
		}
	}
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/* Polymer engine engine implementation *******************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

static const PolymerOrder *polymerEngineInvoke (const Method *method,
											         ValueType *args, Hjava_lang_Object **actionRet)
{
	// TODO: part of this can be cached

	// Ensure capacity the local references for the temporary objects
	// TODO: Push/PopLocalFrame might be better, but Pop seems to pop all push frames in a method,
	// rather than the last.
	(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 9 + 3 * METHOD_NARGS (method));

	// Create the action object
	// public Action(Object o, String packageName, String className, String methodName,
	//	String signature, Object[] params, String runMethodName)
	jvalue action_args[8];
	memset (action_args, 0, sizeof (action_args)); // Clear all taints

	if (!(method->accflags & ACC_STATIC))
	{
#ifdef INTERPRETER
		action_args[0].l = args[0].l;
		args++;
#else
		uintp *argsTyped = (uintp *) args;
		action_args[0].i = *argsTyped;
		args = argsTyped + 1;
#endif
	}
	action_args[1].l = utf8ConstSubString2Java (method->class->name, 0, method->class->packageLength);
	action_args[2].l = utf8ConstSubString2Java (method->class->name, getClassNameStart (method->class), 0);
	action_args[3].l = utf8Const2Java (method->name);
	action_args[4].l = utf8Const2Java (method->parsed_sig->signature);
	action_args[5].l = (void *) method;
	action_args[6].l = args;
	action_args[7].l = formalParametersToClassArray (method); // TODO: this would be a perfect caching candidate
	Hjava_lang_Object *action = (*THREAD_JNIENV())->NewObjectA (THREAD_JNIENV(), trishul_p_ConcreteAction.class,
										trishul_p_ConcreteAction_ctor.method, action_args);
	*actionRet = MakeGlobalRef (action);

	// Call the Engine.query method
	return doQuery (polymerGetEngine (), *actionRet);
}

/**********************************************************************************************************************/

static void polymerHandleResult (const Method *method,
								 const PolymerOrder *order,
						         jvalue *retval, Hjava_lang_Object *x)
{
	assert ((*THREAD_JNIENV())->IsInstanceOf (THREAD_JNIENV(), (Hjava_lang_Object *) order,
												trishul_p_ResultOrder.class));
	(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 2);

	bool exception = (x != NULL);
	Hjava_lang_Object *result = exception ?  x : boxIfRequired (METHOD_RET_TYPE (method), retval);
	(*THREAD_JNIENV())->CallVoidMethod (THREAD_JNIENV(), (Hjava_lang_Object *) order,
										trishul_p_ResultOrder_handleResult.method,
										result, exception);
}

/**********************************************************************************************************************/

static void polymerAcceptOrder (const PolymerOrder *order)
{
	(*THREAD_JNIENV())->CallVoidMethod (THREAD_JNIENV(), polymerGetEngine (),
										trishul_p_Engine_accept.method, order);
	exitOnException ();
}

/**********************************************************************************************************************/

static PolymerOrderType polymerOrderGetType (const PolymerOrder *order)
{
	Hjava_lang_Object *obj = (Hjava_lang_Object *) order;
	int retval = (*THREAD_JNIENV())->CallIntMethod (THREAD_JNIENV(), obj, trishul_p_Order_getType.method);
	exitOnException ();
	return retval;
}

/**********************************************************************************************************************/

static void polymerHalt ()
{
	Hjava_lang_Object *x = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(), trishul_p_HaltException.class,
															trishul_p_HaltException_ctor.method);
	polymerDone (x);
	throwExceptionObject (x);
	exitOnException ();
}

/**********************************************************************************************************************/

static void polymerExecuteInsertion (const PolymerOrder *order)
{
	(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 9);

	Hjava_lang_Object *insertionAction = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(),
							(PolymerOrder *) order, trishul_p_InsertionOrder_getInsertAction.method);

	jmethodID insertionMethod = polymerConcreteActionToMethod (insertionAction);
	exitOnException ();

	// Retrieve the this pointer
	Hjava_lang_Object *thisPtr = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), insertionAction,
															trishul_p_Action_getThisPointer.method);
	exitOnException ();

	// Retrieve the params
	HArrayOfObject *params = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), insertionAction,
															trishul_p_ConcreteAction_getActualParameters.method);
	exitOnException ();

	// Create the parameters object
	jvalue *params_unboxed = (jvalue *) malloc (sizeof (jvalue) * ARRAY_SIZE (params));
	objectArrayToParameters (insertionMethod, params, params_unboxed);

	// Invoke it
	jvalue retval;
	KaffeVM_callMethodA (insertionMethod, METHOD_NATIVECODE(insertionMethod), thisPtr, 0, params_unboxed, &retval, 0);
	free (params_unboxed);


	Hjava_lang_Object *e = (*THREAD_JNIENV())->ExceptionOccurred (THREAD_JNIENV());
	polymerHandleResult (insertionMethod, order, &retval, e);
	if (e)
	{
		(*THREAD_JNIENV())->DeleteLocalRef(THREAD_JNIENV(), e);
		(*THREAD_JNIENV())->ExceptionClear(THREAD_JNIENV());
	}
}

/**********************************************************************************************************************/

static void polymerGetReplacedResult (const Method *method, const PolymerOrder *order, jvalue *retval)
{
	Hjava_lang_Object *value = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(),
						(PolymerOrder *) order, trishul_p_ReplaceOrder_getReplaceValue.method);
	unboxIfRequired (METHOD_RET_TYPE (method), value, retval);
}

/**********************************************************************************************************************/

static void polymerThrowSuppressException ()
{
	throwTypedException (&trishul_p_SuppressException, "Not allowed");
}

/**********************************************************************************************************************/

static Hjava_lang_Throwable *polymerGetException (const PolymerOrder *order)
{
	Hjava_lang_Throwable *value = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(),
						(Hjava_lang_Object *) order, trishul_p_ExceptionOrder_getException.method);
	exitOnException ();

	return value;
}

/**********************************************************************************************************************/

static void polymerTaintParameter (const Method *method, const PolymerOrder *order,
								   ValueType *args, Hjava_lang_Object *action, bool doTaint)
{
	HArrayOfObject *params = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), action,
																 trishul_p_ConcreteAction_getActualParameters.method);
	exitOnException ();

	int i = (*THREAD_JNIENV())->CallIntMethod (THREAD_JNIENV(), (PolymerOrder *) order,
											   trishul_p_ParamTaintOrder_getParameterIndex.method);
	exitOnException ();

	if (i >= ARRAY_SIZE (params))
	{
		(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), params);
		throwTypedException (&trishul_p_TrishulPException,
							 "Taint of parameter %d requested, but only %d parameters are present",
							 i, ARRAY_SIZE (params));
		return;
	}

#ifdef JIT3
	void 	**argPtr 	= (void **) args;
	taint_t *taintPtr	= (taint_t *) (argPtr + METHOD_NARGS (method));

#ifdef STACK_LIMIT
	taintPtr++;
#endif
	if (!(method->accflags & ACC_STATIC))
		taintPtr++; // Skip this-pointer taint

	jvalue argValue;
	argValue.taint = taintPtr[i];
	switch (METHOD_ARG_TYPE(method, i)[0])
	{
	case 'D':
	case 'J':
		// For double-sized values, taint and value are switched
		argValue.l 		= argPtr[i + 1];
		argValue.j 		<<= 32;
		argValue.l 		= argPtr[i];
		break;
	default:
		argValue.l 		= argPtr[i];
		break;
	}


	// Unbox if required, and copy result into original kaffe argument argValue
	unboxIfRequired (METHOD_ARG_TYPE (method, i), OBJARRAY_DATA(params)[i], &argValue);

	polymerTaintFromOrder (order, METHOD_ARG_TYPE (method, i), &argValue, doTaint);

	// Copy the taint value back
	taintPtr[i] = argValue.taint;

#else

	uint argIndex = i;

	if (!(method->accflags & ACC_STATIC))
		argIndex++; // Skip this-pointer

	// Unbox if required, and copy result into original kaffe argument argValue
	unboxIfRequired (METHOD_ARG_TYPE (method, i), OBJARRAY_DATA(params)[i], &args[argIndex]);

	polymerTaintFromOrder (order, METHOD_ARG_TYPE (method, i), &args[argIndex], doTaint);

#endif

	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), params);
}

/**********************************************************************************************************************/

static void polymerTaintObject (const PolymerOrder *order, bool doTaint)
{
	taint_trace_native (1, ("%s order", doTaint ? "TaintObject" : "UntaintObject"));

	jvalue value;
	value.l = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (PolymerOrder *) order,
													trishul_p_ObjectTaintOrder_getObject.method);
	exitOnException ();

	polymerTaintFromOrder (order, "Ljava_lang_Object;", &value, doTaint);

	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), value.l);
}

/**********************************************************************************************************************/

static void polymerTaintContext (const PolymerOrder *order, bool doTaint)
{
	taint_trace_native (1, ("%s order", doTaint ? "TaintContext" : "UntaintContext"));

	(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 2);

	Hjava_lang_Object *taintObj = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(),
						(PolymerOrder *) order, trishul_p_TaintOrder_getTaint.method);
	exitOnException ();
	TaintParsed taint;
	parseTaint (taintObj, &taint);
	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), taintObj);

	trishulSetFallbackContext (taintMinus (trishulGetFallbackContext (), taint.taint));
}

/**********************************************************************************************************************/

static void polymerTaintReturnValue (const Method *method, const PolymerOrder *order,
									 jvalue *retval, bool doTaint)
{
	taint_trace_native (1, ("%s order: %s.%s %s", doTaint ? "TaintRetVal" : "UntaintRetVal",
							method->class->name->data, method->name->data, METHOD_SIGD(method)));
	polymerTaintFromOrder (order, METHOD_RET_TYPE (method), retval, doTaint);
}

/**********************************************************************************************************************/
static bool polymerHandleOrderBefore (const Method *method,
						  			 	   ValueType *arguments, jvalue *retval,
						  			 	   const PolymerOrder *order,
						  			 	   Hjava_lang_Object *action,
						  			 	   bool *again);

static bool polymerHandleCompoundOrderBefore (const Method *method,
						  			 	     	   void *arguments, jvalue *retval,
						  			 	     	   const PolymerOrder *compound,
						  			 	     	   Hjava_lang_Object *action)
{
	HArrayOfObject *sugs = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (PolymerOrder *) compound,
													trishul_p_CompoundOrder_getOrders.method);
	int i;

	bool invoke = true;

	for (i = 0; i < ARRAY_SIZE(sugs); i++)
	{
		const PolymerOrder *sug = (const PolymerOrder *) OBJARRAY_DATA(sugs)[i];
		bool again = false;

		if (!polymerHandleOrderBefore (method, arguments, retval, sug, action, &again))
			invoke = false;

		if (again)
		{
			const PolymerOrder *dummy;
			if (!polymerEngineBefore (method, arguments, retval, &dummy))
			 	invoke = false;
		}
	}

	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), sugs);
	return invoke;
}

/**********************************************************************************************************************/

static bool polymerHandleOrderBefore (const Method *method,
						  			 	   ValueType *arguments, jvalue *retval,
						  			 	   const PolymerOrder *order,
						  			 	   Hjava_lang_Object *action,
						  			 	   bool *again)
{
	switch (polymerOrderGetType (order))
	{
	case polymerOrderOK:
		polymerAcceptOrder 		(order);
		break;
	case polymerOrderHalt:
		polymerAcceptOrder 		(order);
		polymerHalt 					();
		break;
	case polymerOrderInsert:
		polymerAcceptOrder 		(order);
		polymerExecuteInsertion 		(order);
		*again = true;
		break;
	case polymerOrderReplace:
		polymerAcceptOrder 		(order);
		polymerGetReplacedResult 		(method, order, retval);
		return false;
	case polymerOrderSuppress:
		polymerAcceptOrder 		(order);
		polymerThrowSuppressException 	();
		return false;
	case polymerOrderException:
		{
			polymerAcceptOrder	(order);
			Hjava_lang_Object *x = (Hjava_lang_Object *) polymerGetException (order);
			taint_trace_native (1, ("ExceptionOrder: %s", x->vtable->class->name->data));
			throwExceptionObject (x);
		}
		return false;
	case polymerOrderTaintReturnValue:
		polymerAcceptOrder			(order);
		break;
	case polymerOrderUntaintReturnValue:
		polymerAcceptOrder			(order);
		break;
	case polymerOrderUntaintFallback:
		polymerAcceptOrder			(order);
		break;
	case polymerOrderTaintParameter:
		polymerAcceptOrder			(order);
		polymerTaintParameter   		(method, order, arguments, action, true);
		break;
	case polymerOrderUntaintParameter:
		polymerAcceptOrder			(order);
		polymerTaintParameter   		(method, order, arguments, action, false);
		break;
	case polymerOrderTaintObject:
		polymerAcceptOrder			(order);
		polymerTaintObject				(order, true);
		break;
	case polymerOrderUntaintObject:
		polymerAcceptOrder			(order);
		polymerTaintObject				(order, false);
		break;
	case polymerOrderCompound:
		polymerAcceptOrder			(order);
		return polymerHandleCompoundOrderBefore (method, arguments, retval, order, action);
#ifdef KAFFEVM_DEBUG
	case polymerOrderBreakpoint:
		abort (); // TODO: proper breakpoint like int 3?
		break;
	case polymerOrderTaintDebugLevel:
		setTaintDebugLevel ((*THREAD_JNIENV())->CallIntMethod (THREAD_JNIENV(), (Hjava_lang_Object *) order,
							trishul_p_TaintDebugLevelOrder_getDebugLevel.method));
		break;
#endif
	}
	return true;
}

/**********************************************************************************************************************/

bool polymerEngineBefore (const Method *method,
						  ValueType *arguments, jvalue *retval,
						  const PolymerOrder **order)
{
	bool 				again;
	bool				invoke = true;
	Hjava_lang_Object 	*action;
	//printf ("POLYMER BEFORE: %s.%s\n", method->class->name->data, method->name->data);

	do
	{
		again 	= false;
		action 	= NULL;

		*order = polymerEngineInvoke (method, arguments, &action);
		if (*order)
		{
			(*THREAD_JNIENV())->SetObjectField (THREAD_JNIENV(), (PolymerOrder *) *order,
											   trishul_p_Order_triggerAction.field, action);
			invoke = polymerHandleOrderBefore (method, arguments, retval,
													*order, action, &again);
		}
	} while (again);

	//printf ("POLYMER BEFORE2: %p\n", *order);
	return invoke;
}

/**********************************************************************************************************************/

static void polymerHandleCompoundOrderAfter (const Method *method, jvalue *retval,
												  Hjava_lang_Object *x, const PolymerOrder *compound)
{
	HArrayOfObject *sugs = (*THREAD_JNIENV())->CallObjectMethod (THREAD_JNIENV(), (PolymerOrder *) compound,
													trishul_p_CompoundOrder_getOrders.method);
	int i;

	for (i = 0; i < ARRAY_SIZE(sugs); i++)
	{
		const PolymerOrder *sug = (const PolymerOrder *) OBJARRAY_DATA(sugs)[i];
		polymerEngineAfter (method, retval, x, sug);
	}

	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), sugs);
}

/**********************************************************************************************************************/

void polymerEngineAfter (const Method *method, jvalue *retval,
						 Hjava_lang_Object *x, const PolymerOrder *order)
{
	//printf ("POLYMER AFTER: %p %X %X\n", order, ((uint *) retval)[0], ((uint *) retval)[1]);
	switch (polymerOrderGetType (order))
	{
	case polymerOrderOK:
		polymerHandleResult (method, order, retval, x);
		break;
	case polymerOrderTaintReturnValue:
		polymerTaintReturnValue (method, order, retval, true);
		break;
	case polymerOrderUntaintReturnValue:
		polymerTaintReturnValue (method, order, retval, false);
		break;
	case polymerOrderCompound:
		polymerHandleCompoundOrderAfter (method, retval, x, order);
		break;
	default:
		break;
	}

	(*THREAD_JNIENV())->DeleteLocalRef (THREAD_JNIENV(), (Hjava_lang_Object *) order);
}

/**********************************************************************************************************************/

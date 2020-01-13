#include "trishul.h"
#include "code-analyse.h"
#include "context-analysis.h"
#include "decompile.h"
#include "exception.h"
#include "hashtab.h"
#include "java_lang_reflect_Method.h"
#include "java_lang_Object.h"
#include "lookup.h"
#include "polymer.h"
#include "thread.h"
#include "utf8const.h"
#include "utils.h"
#include <openssl/sha.h>
/**********************************************************************************************************************/
#define TRISHUL_ANNOTATIONS_PACKAGE 	"trishul/annotations/"
#define PREFIX_METHOD_HOOK 				"methodHook"
#define PREFIX_CTOR_HOOK 				"ctorHook"
#define PREFIX_FIELD_NOTAINT_VAR		"notaintvar"
#define PREFIX_FIELD_NOTAINT_METHOD		"notaintmethod"
#define PREFIX_FIELD_NOTAINT_FALLBACK	"notaintfallback"
#define SIG_TRISHUL_ANNOTATION_THIS		"Ltrishul/TrishulAnnotation$This;"

static JAVA_CLASS (trishul_Engine,					trishul/Engine);
static JAVA_CLASS (trishul_InvalidEngineException,	trishul/InvalidEngineException);
/**********************************************************************************************************************/
static Hjava_lang_Object 		*engineObject;
/**********************************************************************************************************************/
taint_t							*current_context_taint;
/**********************************************************************************************************************/
static int hash_ptr (const void *ptr1)						{return (int) ptr1;}
static int compare_ptr (const void *ptr1, const void *ptr2)	{return (ptr2 - ptr1);}

#ifdef KAFFE_VMDEBUG
static hashtab_t				debug_classes;
static int 						taint_debug_level = 3;
#endif
/**********************************************************************************************************************/
#ifdef KAFFE_VMDEBUG

void do_taint_trace (int level, const char *insName, uint pc,
					 const Method *meth, const char *msg, ...)
{
	if (isTaintDebugClass (meth->class) && level <= getTaintDebugLevel ())
	{
		printf ("[%s.%s:%u]: %s: ", meth->class->name->data, meth->name->data, pc, insName);
		va_list vl;
		va_start (vl, msg);
		vprintf (msg, vl);
		printf ("\n");
	}
}

#endif
/**********************************************************************************************************************/

#ifdef KAFFE_VMDEBUG
void taintInitialize (Hjava_lang_Class *class)
{
	debug_classes = hashInit (hash_ptr, compare_ptr, NULL, NULL);
	hashAdd (debug_classes, (Hjava_lang_Class *) class);
}
#endif

/**********************************************************************************************************************/

void taintRelease ()
{
#ifdef KAFFE_VMDEBUG
	if (debug_classes)
	{
		hashDestroy (debug_classes);
	}
#endif
	trishulDestroyCodeAnalyseData ();
}

/**********************************************************************************************************************/
#ifdef KAFFE_VMDEBUG

void addTaintDebugClass (const Hjava_lang_Class *class)
{
	hashAdd (debug_classes, (Hjava_lang_Class *) class);
}

/**********************************************************************************************************************/

bool isTaintDebugClass (const Hjava_lang_Class *class)
{
	if (!debug_classes) return false;
	return (hashFind (debug_classes, class) != NULL);
}

/**********************************************************************************************************************/

void setTaintDebugLevel (int level)
{
	taint_debug_level = level;
}

/**********************************************************************************************************************/

int getTaintDebugLevel ()
{
	return taint_debug_level;
}

#endif /* KAFFE_VMDEBUG */
/**********************************************************************************************************************/

static Hjava_lang_Object *invoke_reflected_method (Hjava_lang_Object *methodReflect,
												   Hjava_lang_Object *this,
												   HArrayOfObject *params)
{
	typedef Hjava_lang_Object *JNICALL
	(*Java_java_lang_reflect_Method_invoke0) (JNIEnv* env, Hjava_lang_Object *_this,
											  Hjava_lang_Object *_obj, HArrayOfObject *_argobj);

	static Java_java_lang_reflect_Method_invoke0 invoker;

	/* Quickhack to load Java_java_lang_reflect_Method_invoke0, even when it is not linked in */
	if (!invoker)
	{
		jclass mcls;
		jmethodID mmth;
		mcls = (*THREAD_JNIENV())->FindClass(THREAD_JNIENV(), "java/lang/reflect/Method");
		mmth = (*THREAD_JNIENV())->GetMethodID(THREAD_JNIENV(),
    			mcls, "invoke0", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    	invoker = (Java_java_lang_reflect_Method_invoke0) getMethodCodeStart (mmth);
	}

	return invoker (THREAD_JNIENV(), methodReflect, this, params);
}

/**********************************************************************************************************************/

void addEngineClass (Hjava_lang_Class *class, const char *enginePolicyURL)
{
	if (!classExtends (class, &trishul_Engine))
	{
		/* Throw a tainter exception */
		throwTypedException (&trishul_InvalidEngineException, "Not a valid engine: %s", CLASS_CNAME(class));
	}
	else
	{
		/* Create the tainter object. This is not done using NewObject, as that returns the object only after
		 * invoking the ctor, which makes the this-pointer tests fail during the constructor.
		 */

		/* Locate the default constructor */
		jmethodID ctor = (*THREAD_JNIENV())->GetMethodID (THREAD_JNIENV(), class, "<init>", "()V");
		exitOnException ();

		/* Create the object */
		Hjava_lang_Object *engine = (*THREAD_JNIENV())->AllocObject (THREAD_JNIENV(), class);
		exitOnException ();
		engine = (*THREAD_JNIENV())->NewGlobalRef(THREAD_JNIENV(), engine);

		/* Call the constructor */
		(*THREAD_JNIENV())->CallVoidMethod (THREAD_JNIENV(), engine, ctor);
		exitOnException ();

		/* Initialize trishul_p */
		Hjava_lang_Object *enginePolicy = enginePolicyURL ? loadEnginePolicyFromURL (enginePolicyURL) : NULL;
		engineObject = polymerInitializeEngineObject (NULL, engine, enginePolicy);
		exitOnException ();
	}
}

/**********************************************************************************************************************/

Hjava_lang_Object *trishulGetEngine ()
{
	return engineObject;
}

/**********************************************************************************************************************/
/* Trishul native classes */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* Generic tainting functions, used by trishul.Tainter and trishul.taint.test.TaintTest */
/**********************************************************************************************************************/

#define trishul_Engine_IMPL(sig,type,name,format)																	\
	static type taint##sig (type value, jint taint)																	\
	{																												\
		taint_trace_native (1, ("Tainting "#name" %"#format" with %.2X", value, taint));							\
		nativeRetValSetTaint (#sig[0], taint);																		\
		return value;																								\
	}																												\
																													\
	static jint getTaint##sig (type value)																			\
	{																												\
		taint_trace_native (3, ("Taint of "#name" %"#format": %.2X", value, nativeParamGetTaint(0)));				\
		return nativeParamGetTaint(0);																				\
	}																												\
																													\
	static void trishul_taint_test_TaintTest_checkTaint##sig (const Hjava_lang_String *desc, 						\
															  type value, jint wantTaint)							\
	{																												\
		taint_trace_native (3, ("%s: Checking taint of "#name" %"#format" for %.2X", 								\
								desc == NULL ? "" : stringJava2C(desc), value, wantTaint));							\
		int taint = nativeParamGetTaint(1);																			\
		if (taint == wantTaint) taint_trace_native (3, ("OK: %s: taint=%.2X == wantTaint=%.2X",						\
													  desc == NULL ? "" : stringJava2C (desc), taint, wantTaint));	\
		else taint_trace_native (0, ("ERROR: %s: taint=%.2X != wantTaint=%.2X", 									\
									 	 desc == NULL ? "" : stringJava2C (desc), taint, wantTaint));				\
	}

trishul_Engine_IMPL(I,jint,int,d)
trishul_Engine_IMPL(J,jlong,long,lld)
trishul_Engine_IMPL(D,jdouble,double,f)
trishul_Engine_IMPL(F,jfloat,float,f)
trishul_Engine_IMPL(B,jbyte,byte,d)
trishul_Engine_IMPL(C,jchar,char,d)
trishul_Engine_IMPL(S,jshort,short,d)
trishul_Engine_IMPL(Z,jboolean,boolean,d)
trishul_Engine_IMPL(A,void *,ref,p)

#undef trishul_Engine_IMPL
/**********************************************************************************************************************/
/* Implementation of trishul.taint.test.TaintTest */
/**********************************************************************************************************************/

void taintArray (Hjava_lang_Object *array, jint taint)
{
	taint_trace_native (1, ("Tainting array: %p: %X |= %X", array, array->obj_taint, taint));
	taintAdd1 (array->obj_taint, taint);
}

void setArrayTaint (Hjava_lang_Object *array, jint taint)
{
	taint_trace_native (1, ("Tainting array: %p: %X", array, taint));
	array->obj_taint = taint;
}

jint getArrayTaint (const Hjava_lang_Object *obj)
{
	taint_trace_native (3, ("%X", obj->obj_taint));
	return obj->obj_taint;
}

static Hjava_lang_Object *nativeUntaintArray (Hjava_lang_Object *obj, jint untaint)
{
	untaintObject (obj, untaint);
	nativeRetValSubTaint ('A', untaint);
	return obj;
}

// TODO: bounds checking?

void taintArrayElement (Hjava_lang_Object *array, jint index, jint taint)
{
	taint_trace_native (4, ("Tainting array element: %p[%d]: %X |= %X",
						array, index, array->member_taint[index], taint));
	taintAdd1 (array->member_taint[index], taint);

	// TODO: this is done to mimic instruction behaviour. Is this correct?
#ifdef TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE
	taintAdd1 (array->obj_taint, taint);
#endif
}


void untaintArrayElement (Hjava_lang_Object *array, jint index, jint taint)
{
	taint_trace_native (4, ("Untainting array element: %p[%d]: %X -= %X",
						array, index, array->member_taint[index], taint));
	taintSub1 (array->member_taint[index], taint);
}

void setArrayElementTaint (Hjava_lang_Object *array, jint index, jint taint)
{
	taint_trace_native (4, ("Tainting array element: %p[%d]: %X", array, index, taint));
	array->member_taint[index] = taint;

	// TODO: this is done to mimic instruction behaviour. Is this correct?
#ifdef TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE
	taintAdd1 (array->obj_taint, taint);
#endif
}

jint getArrayElementTaint (const Hjava_lang_Object *obj, jint index)
{
	taint_trace_native (4, ("[%d] %X", index, obj->member_taint[index]));
	// TODO: this is done to mimic instruction behaviour. Is this correct?
	return taintMerge2 (obj->member_taint[index], obj->obj_taint);
}

void trishul_taint_test_TaintTest_checkArrayTaint (const Hjava_lang_String *desc,
												   const Hjava_lang_Object *value, jint wantTaint)
{
	taint_trace_native (3, ("%s: Checking taint of array %p for %.2X", stringJava2C(desc), value, wantTaint));
	int taint = value->obj_taint;
	if (taint == wantTaint) taint_trace_native (3, ("OK: %s: taint=%.2X == wantTaint=%.2X",
	stringJava2C (desc), taint, wantTaint));
	else taint_trace_native (0, ("ERROR: %s: taint=%.2X != wantTaint=%.2X",
									 stringJava2C (desc), taint, wantTaint));
}

void taintObject (Hjava_lang_Object *obj, jint taint)
{
	taint_trace_native (1, ("Tainting object: %s (%p): %X |= %X", obj->vtable->class->name->data, obj, obj->obj_taint, taint));
	obj->obj_taint = taintMerge2 (obj->obj_taint, taint);
}

void setObjectTaint (Hjava_lang_Object *obj, jint taint)
{
	taint_trace_native (1, ("Setting object taint: %s (%p): %X", obj->vtable->class->name->data, obj, taint));
	obj->obj_taint = taint;
}

void untaintObject (Hjava_lang_Object *obj, jint untaint)
{
	taint_trace_native (1, ("Untainting object: %s (%p): %X -= %X", obj->vtable->class->name->data, obj, obj->obj_taint, untaint));
	taintSub1 (obj->obj_taint, untaint);
}

static Hjava_lang_Object *nativeUntaintObject (Hjava_lang_Object *obj, jint untaint)
{
	untaintObject (obj, untaint);
	nativeRetValSubTaint ('A', untaint);
	return obj;
}

jint getObjectTaint (const Hjava_lang_Object *obj)
{
	taint_trace_native (3, ("%X", obj->obj_taint));
	return obj->obj_taint;
}

jint getContextTaint ()
{
	taint_t taint = nativeContextGetTaint ();
	taint_trace_native (3, ("%X", taint));
	return taint;
}

void trishul_taint_test_TaintTest_checkObjectTaint (const Hjava_lang_String *desc,
													const Hjava_lang_Object *value, jint wantTaint)
{
	taint_trace_native (3, ("%s: Checking taint of object %p for %.2X", stringJava2C(desc), value, wantTaint));
	int taint = value->obj_taint;
	if (taint == wantTaint) taint_trace_native (3, ("OK: %s: taint=%.2X == wantTaint=%.2X",
	stringJava2C (desc), taint, wantTaint));
	else taint_trace_native (0, ("ERROR: %s: taint=%.2X != wantTaint=%.2X",
									 stringJava2C (desc), taint, wantTaint));
}

#define GET_CONFIG_OPTION(option)
static jboolean getConfigOption (const Hjava_lang_String *nameJava)
{
	const char *name = stringJava2C (nameJava);
	if (!strcmp (name, "TRISHUL_TAINT_OBJECTS_PASSED_TO_NATIVE_FUNCTION"))
	{
#ifdef TRISHUL_TAINT_OBJECTS_PASSED_TO_NATIVE_FUNCTION
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_THIS_INCLUDES_REFERENCE"))
	{
#ifdef TRISHUL_TAINT_THIS_INCLUDES_REFERENCE
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_OBJECT_ON_MEMBER_STORE"))
	{
#ifdef TRISHUL_TAINT_OBJECT_ON_MEMBER_STORE
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE"))
	{
#ifdef TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_MEMBER_ON_OBJECT_LOAD"))
	{
#ifdef TRISHUL_TAINT_MEMBER_ON_OBJECT_LOAD
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD"))
	{
#ifdef TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_OBJECT_ON_REFERENCE_STORE"))
	{
#ifdef TRISHUL_TAINT_OBJECT_ON_REFERENCE_STORE
		return true;
#else
		return false;
#endif
	}
	else if (!strcmp (name, "TRISHUL_TAINT_PACKED_REGISTERS"))
	{
#ifdef TRISHUL_TAINT_PACKED_REGISTERS
		return true;
#else
		return false;
#endif
	}
	else
	return false;
}

const BuiltinFunctionInfo_t DISPATCH_trishul_test_taint_TaintTest[] =
{
	{"taint",			"(II)I",									taintI},
	{"taint",			"(JI)J",									taintJ},
	{"taint",			"(DI)D",									taintD},
	{"taint",			"(FI)F",									taintF},
	{"taint",			"(Ljava/lang/Object;I)Ljava/lang/Object;",	taintA},
	{"taint",			"(BI)B",									taintB},
	{"taint",			"(CI)C",									taintC},
	{"taint",			"(SI)S",									taintS},
	{"taint",			"(ZI)Z",									taintZ},
	{"taintObject",		"(Ljava/lang/Object;I)V",					taintObject},
	{"taintArray",		"(Ljava/lang/Object;I)V",					taintArray},
	{"getTaint",		"(I)I",										getTaintI},
	{"getTaint",		"(J)I",										getTaintJ},
	{"getTaint",		"(D)I",										getTaintD},
	{"getTaint",		"(F)I",										getTaintF},
	{"getTaint",		"(Ljava/lang/Object;)I",					getTaintA},
	{"getTaint",		"(B)I",										getTaintB},
	{"getTaint",		"(C)I",										getTaintC},
	{"getTaint",		"(S)I",										getTaintS},
	{"getTaint",		"(Z)I",										getTaintZ},
	{"getObjectTaint",	"(Ljava/lang/Object;)I",					getObjectTaint},
	{"getArrayTaint",	"(Ljava/lang/Object;)I",					getArrayTaint},
	{"getFallbackTaint","()I",										trishulGetFallbackContext},
	{"getContextTaint", "()I",										getContextTaint},
	{"checkTaint",		"(Ljava/lang/String;II)V",					trishul_taint_test_TaintTest_checkTaintI},
	{"checkTaint",		"(Ljava/lang/String;JI)V",					trishul_taint_test_TaintTest_checkTaintJ},
	{"checkTaint",		"(Ljava/lang/String;DI)V",					trishul_taint_test_TaintTest_checkTaintD},
	{"checkTaint",		"(Ljava/lang/String;FI)V",					trishul_taint_test_TaintTest_checkTaintF},
	{"checkTaint",		"(Ljava/lang/String;Ljava/lang/Object;I)V",	trishul_taint_test_TaintTest_checkTaintA},
	{"checkTaint",		"(Ljava/lang/String;BI)V",					trishul_taint_test_TaintTest_checkTaintB},
	{"checkTaint",		"(Ljava/lang/String;CI)V",					trishul_taint_test_TaintTest_checkTaintC},
	{"checkTaint",		"(Ljava/lang/String;SI)V",					trishul_taint_test_TaintTest_checkTaintS},
	{"checkTaint",		"(Ljava/lang/String;ZI)V",					trishul_taint_test_TaintTest_checkTaintZ},
	{"checkArrayTaint",	"(Ljava/lang/String;Ljava/lang/Object;I)V",	trishul_taint_test_TaintTest_checkArrayTaint},
	{"checkObjectTaint","(Ljava/lang/String;Ljava/lang/Object;I)V",	trishul_taint_test_TaintTest_checkObjectTaint},
	{"getConfigOption",	"(Ljava/lang/String;)Z",					getConfigOption},
	{NULL, NULL, NULL}
};

/**********************************************************************************************************************/
/* Implementation of trishul.Engine */
/**********************************************************************************************************************/
/* TODO: define behaviour when multiple object-hooks are set on a single method
 *       error, allow multiple hooks, overwrite?
 */

/* Checks if the specified engine object is the actual engine, to prevent
 * applications from creating their own.
 */
static bool engineCheckThisPointer (Hjava_lang_Object *thisPointer)
{
	if (thisPointer != engineObject)
	{
		throwTypedException (&trishul_InvalidEngineException, "Engine object has wrong this pointer");
		return false;
	}
	return true;
}

Method *trishulUnhookMethod;

static void trishul_Engine_invokeConstructor (Hjava_lang_Object *thisPointer,
											   Hjava_lang_Object *ctorReflect,
											   Hjava_lang_Object *this,
											   HArrayOfObject *params)
{
	if (engineCheckThisPointer (thisPointer))
	{
		Method *ctor = (*THREAD_JNIENV())->FromReflectedMethod(THREAD_JNIENV(), ctorReflect);

		Method *oldUnhook = trishulUnhookMethod;
		trishulUnhookMethod = ctor;

		jvalue *params_unboxed = (jvalue *) malloc (sizeof (jvalue) * ARRAY_SIZE (params));
		objectArrayToParameters (ctor, params, params_unboxed);
		KaffeVM_callMethodA (ctor, METHOD_NATIVECODE(ctor), this, nativeParamGetTaint (2),
							 params_unboxed, NULL, 0);
		free (params_unboxed);

		trishulUnhookMethod = oldUnhook;
	}
}

static Hjava_lang_Object *trishul_Engine_invokeMethod (Hjava_lang_Object *thisPointer,
													   Hjava_lang_Object *methodReflect,
													   Hjava_lang_Object *this,
													   HArrayOfObject *params)
{
	if (engineCheckThisPointer (thisPointer))
	{
		Hjava_lang_Object *retval;

		Method *method = (*THREAD_JNIENV())->FromReflectedMethod(THREAD_JNIENV(), methodReflect);

		Method *oldUnhook = trishulUnhookMethod;
		trishulUnhookMethod = method;

		// TODO: the 9 here is purely guesswork and will fail
		(*THREAD_JNIENV())->EnsureLocalCapacity (THREAD_JNIENV(), 9);

		retval = invoke_reflected_method (methodReflect, this, params);

		trishulUnhookMethod = oldUnhook;

		return retval;
	}

	return NULL;
}

/*
 * member may also be a Constructor, but this should work
 */
static void trishul_Engine_registerHookInt (Hjava_lang_Object *thisPointer,
											 Hjava_lang_reflect_Method *member,
											 Hjava_lang_Object *on_object,
											 Hjava_lang_reflect_Method *hook)
{
	//if (engineCheckThisPointer (thisPointer))
	{
		Method *member_meth = (*THREAD_JNIENV())->FromReflectedMethod(THREAD_JNIENV(), member);
		Method *hook_meth   = (*THREAD_JNIENV())->FromReflectedMethod(THREAD_JNIENV(), hook);

		member_meth->hook = hook_meth;
		if (!on_object)
		{
			member_meth->hook_always = true;
		}
		else
		{
			if (!member_meth->hook_objects)
			{
				// TODO: shouldn't this be destroyed somewhere?
				member_meth->hook_objects = hashInit (hash_ptr, compare_ptr, NULL, NULL);
			}
			hashAdd (member_meth->hook_objects, on_object);
		}

		member_meth->hook_reflected = member;

		taint_trace_native (1, ("Hook: %s.%s -> %s.%s",
								CLASS_CNAME(member_meth->class), METHOD_NAMED(member_meth),
								CLASS_CNAME(hook_meth->class), METHOD_NAMED(hook_meth)));
	}
}


#define trishul_Engine_IMPL(sig,type,name,format) 																	\
	static type trishul_Engine_taint##sig (Hjava_lang_Object *thisPointer, type value, jint taint)					\
	{																												\
		if (engineCheckThisPointer (thisPointer)) 																	\
		{																											\
			taint_trace_native (1, ("Tainting "#name" %"#format" with %.2X", value, taint));						\
			nativeRetValSetTaint (#sig[0], taint);																	\
			return value;																							\
		}																											\
		else return 0;																								\
	}																												\
																													\
	static jint trishul_Engine_getTaint##sig (Hjava_lang_Object *thisPointer, type value)							\
	{																												\
		if (engineCheckThisPointer (thisPointer)) 																	\
		{																											\
			taint_trace_native (3, ("Taint of "#name" %"#format": %.2X", value, nativeParamGetTaint (1)));			\
			return nativeParamGetTaint (1);																			\
		}																											\
		else return 0;																								\
	}

trishul_Engine_IMPL(I,jint,int,d)
trishul_Engine_IMPL(J,jlong,long,lld)
trishul_Engine_IMPL(D,jdouble,double,f)
trishul_Engine_IMPL(F,jfloat,float,f)
trishul_Engine_IMPL(B,jbyte,byte,d)
trishul_Engine_IMPL(C,jchar,char,d)
trishul_Engine_IMPL(S,jshort,short,d)
trishul_Engine_IMPL(Z,jboolean,boolean,d)
trishul_Engine_IMPL(A,void *,ref,p)

#undef trishul_Engine_IMPL

static void trishul_Engine_taintObject (Hjava_lang_Object *thisPointer, Hjava_lang_Object *obj, jint taint)
{
	if (engineCheckThisPointer (thisPointer)) taintObject (obj, taint);
}

static int trishul_Engine_getObjectTaint (Hjava_lang_Object *thisPointer, Hjava_lang_Object *obj)
{
	if (engineCheckThisPointer (thisPointer))
	{
		return getObjectTaint (obj);
	}
	else return 0;
}

static void trishul_Engine_taintArray (Hjava_lang_Object *thisPointer, Hjava_lang_Object *array, jint taint)
{
	if (engineCheckThisPointer (thisPointer)) taintArray (array, taint);
}

static int trishul_Engine_getArrayTaint (Hjava_lang_Object *thisPointer, Hjava_lang_Object *obj)
{
	if (engineCheckThisPointer (thisPointer))
	{
		return getArrayTaint (obj);
	}
	else return 0;
}

const BuiltinFunctionInfo_t DISPATCH_trishul_Engine[] =
{
	{
		"invokeConstructor",
		"(Ljava/lang/reflect/Constructor;Ljava/lang/Object;[Ljava/lang/Object;)V",
		trishul_Engine_invokeConstructor
	},
	{
		"invokeMethod",
		"(Ljava/lang/reflect/Method;Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;",
		trishul_Engine_invokeMethod
	},
	{
		"registerHookInt",
		"(Ljava/lang/reflect/Member;Ljava/lang/Object;Ljava/lang/reflect/Method;)V",
		trishul_Engine_registerHookInt
	},
	{"taint",			"(II)I",									trishul_Engine_taintI},
	{"taint",			"(JI)J",									trishul_Engine_taintJ},
	{"taint",			"(DI)D",									trishul_Engine_taintD},
	{"taint",			"(FI)F",									trishul_Engine_taintF},
	{"taint",			"(BI)B",									trishul_Engine_taintB},
	{"taint",			"(CI)C",									trishul_Engine_taintC},
	{"taint",			"(SI)S",									trishul_Engine_taintS},
	{"taint",			"(ZI)Z",									trishul_Engine_taintZ},
	{"taint",			"(Ljava/lang/Object;I)Ljava/lang/Object;",	trishul_Engine_taintA},
	{"taintArray",		"(Ljava/lang/Object;I)V",					trishul_Engine_taintArray},
	{"taintObject",		"(Ljava/lang/Object;I)V",					trishul_Engine_taintObject},
	{"getTaint",		"(I)I",										trishul_Engine_getTaintI},
	{"getTaint",		"(J)I",										trishul_Engine_getTaintJ},
	{"getTaint",		"(D)I",										trishul_Engine_getTaintD},
	{"getTaint",		"(F)I",										trishul_Engine_getTaintF},
	{"getTaint",		"(Ljava/lang/Object;)I",					trishul_Engine_getTaintA},
	{"getTaint",		"(B)I",										trishul_Engine_getTaintB},
	{"getTaint",		"(C)I",										trishul_Engine_getTaintC},
	{"getTaint",		"(S)I",										trishul_Engine_getTaintS},
	{"getTaint",		"(Z)I",										trishul_Engine_getTaintZ},
	{"getObjectTaint",	"(Ljava/lang/Object;)I",					trishul_Engine_getObjectTaint},
	{"getArrayTaint",	"(Ljava/lang/Object;)I",					trishul_Engine_getArrayTaint},
	{NULL, NULL, NULL}
};

/**********************************************************************************************************************/

/**
 * Looks up the name of the class that contains annotations for the specified class.
 */
Utf8Const *trishulAnnotationClassName (Hjava_lang_Class *class)
{
	// Construct UTF-8 version of the prefixed name
	char buffer[1024];
	int len = snprintf (buffer, sizeof (buffer),
						TRISHUL_ANNOTATIONS_PACKAGE "%s", class->name->data);
	if (len == sizeof (buffer))
	{
		fprintf (stderr, "Class name to long for annotations class\n");
		abort ();
	}
	return utf8ConstNew (buffer, len);
}

/**********************************************************************************************************************/

static Utf8Const *stripAnnotationParametersFromSignature (Method *method, const char *sig)
{
	int sigLength = strlen (SIG_TRISHUL_ANNOTATION_THIS);

	if (strncmp (sig + 1, SIG_TRISHUL_ANNOTATION_THIS, sigLength))
	{
		utf8ConstAddRef (METHOD_SIG (method));
		return METHOD_SIG (method);
	}
	else
	{
		const char *rest = sig + 1 + sigLength;
		unsigned int len = strlen (rest) + 1;
		char buffer[1024];
		char *ptr;
		if (len < sizeof (buffer))
		{
			ptr = buffer;
		}
		else
		{
			ptr = (char *) malloc (len);
		}

		strncpy (ptr + 1, rest, len - 1);
		ptr[0] = '(';
		ptr[len] = 0;
		Utf8Const *utf8 = utf8ConstNew (ptr, len);
		if (len >= sizeof (buffer))
		{
			free (ptr);
		}
		return utf8;
	}
}

/**********************************************************************************************************************/

void trishulProcessAnnotations (Hjava_lang_Class *class)
{
	short i;

	/* Loop through all annotation methods, and look up the original method.
	 * TODO: the other way around might be quicker. Even quicker might be to do this when the Method object is
	 * created,
	 */
	for (i = 0; i < class->annotations->method_count; i++)
	{
		Method *annotated = &class->annotations->methods[i];

		char *pos = strchr (annotated->name->data, '$');
		if (pos)
		{
			char		*realName 		= pos + 1;
			char 		*prefix			= annotated->name->data;
			int 		prefixLength 	= pos - prefix;
			Utf8Const	*sig 			= NULL;
			bool		noTaintFlags	= 0;
			if (!strncmp (prefix, PREFIX_METHOD_HOOK, prefixLength))
			{
				sig = stripAnnotationParametersFromSignature (annotated, METHOD_SIGD (annotated));
			}
			else if (!strncmp (prefix, PREFIX_CTOR_HOOK, prefixLength))
			{
				sig = stripAnnotationParametersFromSignature (annotated, METHOD_SIGD (annotated));
				realName = "<init>";
			}
			else if (!strncmp (prefix, PREFIX_FIELD_NOTAINT_METHOD, prefixLength))
			{
				sig = stripAnnotationParametersFromSignature (annotated, METHOD_SIGD (annotated));
				noTaintFlags = TRISHUL_NOTAINT_METHOD;
			}
			else if (!strncmp (prefix, PREFIX_FIELD_NOTAINT_FALLBACK, prefixLength))
			{
				sig = stripAnnotationParametersFromSignature (annotated, METHOD_SIGD (annotated));
				noTaintFlags = TRISHUL_NOTAINT_FALLBACK;
			}
			else
			{
				fprintf (stderr, "Unknown prefix %.*s in annotation class for %s\n",
						 prefixLength, prefix, class->name->data);
				abort ();
			}

			errorInfo error;
			Utf8Const 	*name = utf8ConstNew (realName, strlen (realName));
			Method 		*orig = findMethod (class, name, sig, &error);
			if (!orig)
			{
				fprintf (stderr, "Unknown annotation method %s %s in annotation class for %s\n",
						 name->data, sig->data, class->name->data);
				throwError(&error);
				exitOnException ();
				abort ();
			}

			if (noTaintFlags)
			{
				orig->noTaintFlags |= noTaintFlags;
			}
			else
			{
				orig->annotated = annotated;
			}

			utf8ConstRelease (name);
			utf8ConstRelease (sig);
		}
	}

	/**
	 * Loop through all annotated variables.
	 */
	for (i = 0; i < class->annotations->field_count; i++)
	{
		Field *annotated = &class->annotations->fields[i];

		char *pos = strchr (annotated->name->data, '$');
		if (pos)
		{
			char		*realName 		= pos + 1;
			char 		*prefix			= annotated->name->data;
			int 		prefixLength 	= pos - prefix;
			if (!strncmp (prefix, PREFIX_FIELD_NOTAINT_VAR, prefixLength))
			{
				errorInfo error;
				Utf8Const 	*name = utf8ConstNew (realName, strlen (realName));
				Field		*orig = lookupClassField (class, name, annotated->accflags & ACC_STATIC, &error);
				if (!orig)
				{
					fprintf (stderr, "Unknown annotation field %s in annotation class for %s\n",
							 name->data, class->name->data);
					abort ();
				}

				orig->noTaint = true;
				utf8ConstRelease (name);
			}
			else
			{
				fprintf (stderr, "Unknown prefix %.*s in annotation class for %s\n",
						 prefixLength, prefix, class->name->data);
				abort ();
			}
		}
	}
}

/**********************************************************************************************************************/
// TODO: Should this be thread local?
Method *trishulAnnotatedOriginalStack = NULL;
#define METHOD_INVALID  ((Method *) -1)
/**********************************************************************************************************************/

void *trishulAnnotatedInvokePush (Method **original)
{
	if (*original != trishulAnnotatedOriginalStack)
	{
		Method *old = trishulAnnotatedOriginalStack;
		trishulAnnotatedOriginalStack = *original;
		*original = (*original)->annotated;

		return (old ? old : METHOD_INVALID);
	}
	return NULL;
}

/**********************************************************************************************************************/

void trishulAnnotatedInvokePop (void *stored)
{
	trishulAnnotatedOriginalStack = ((stored == METHOD_INVALID) ? NULL : (Method *) stored);
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

static Hjava_lang_Object *trishul_TrishulAnnotation_super (Hjava_lang_Object *this, HArrayOfObject *params)
{
	assert (trishulAnnotatedOriginalStack && trishulAnnotatedOriginalStack != METHOD_INVALID);

	Method *meth = trishulAnnotatedOriginalStack;
	jvalue *params_unboxed = (jvalue *) malloc (sizeof (jvalue) * ARRAY_SIZE (params));
	objectArrayToParameters (meth, params, params_unboxed);
	jvalue retval;
	KaffeVM_callMethodA (meth, METHOD_NATIVECODE(meth), this, nativeParamGetTaint (0),
						 params_unboxed, &retval, 0);
	free (params_unboxed);
	return retval.l; // Might also be a primitive, as superI etc reuse this function
}

/**********************************************************************************************************************/

#define trishul_TrishulAnnotation_IMPL(sig,type,name,format) 														\
	static type trishul_TrishulAnnotation_taint##sig (type value, jint taint)										\
	{																												\
		taint_trace_native (1, ("Tainting "#name" %"#format" with %.2X", value, taint));							\
		nativeRetValSetTaint (#sig[0], taint);																		\
		return value;																								\
	}																												\
																													\
	static jint trishul_TrishulAnnotation_getTaint##sig (type value)												\
	{																												\
		taint_trace_native (3, ("Taint of "#name" %"#format": %.2X", value, nativeParamGetTaint (1)));				\
		return nativeParamGetTaint (1);																				\
	}

trishul_TrishulAnnotation_IMPL(I,jint,int,d)
trishul_TrishulAnnotation_IMPL(J,jlong,long,lld)
trishul_TrishulAnnotation_IMPL(D,jdouble,double,f)
trishul_TrishulAnnotation_IMPL(F,jfloat,float,f)
trishul_TrishulAnnotation_IMPL(B,jbyte,byte,d)
trishul_TrishulAnnotation_IMPL(C,jchar,char,d)
trishul_TrishulAnnotation_IMPL(S,jshort,short,d)
trishul_TrishulAnnotation_IMPL(Z,jboolean,boolean,d)
trishul_TrishulAnnotation_IMPL(A,void *,ref,p)

#undef trishul_Engine_IMPL

const BuiltinFunctionInfo_t DISPATCH_trishul_TrishulAnnotation[] =
{
	{"taint",			"(II)I",									trishul_TrishulAnnotation_taintI},
	{"taint",			"(JI)J",									trishul_TrishulAnnotation_taintJ},
	{"taint",			"(DI)D",									trishul_TrishulAnnotation_taintD},
	{"taint",			"(FI)F",									trishul_TrishulAnnotation_taintF},
	{"taint",			"(BI)B",									trishul_TrishulAnnotation_taintB},
	{"taint",			"(CI)C",									trishul_TrishulAnnotation_taintC},
	{"taint",			"(SI)S",									trishul_TrishulAnnotation_taintS},
	{"taint",			"(ZI)Z",									trishul_TrishulAnnotation_taintZ},
	{"taint",			"(Ljava/lang/Object;I)Ljava/lang/Object;",	trishul_TrishulAnnotation_taintA},
	{"getTaint",		"(I)I",										trishul_TrishulAnnotation_getTaintI},
	{"getTaint",		"(J)I",										trishul_TrishulAnnotation_getTaintJ},
	{"getTaint",		"(D)I",										trishul_TrishulAnnotation_getTaintD},
	{"getTaint",		"(F)I",										trishul_TrishulAnnotation_getTaintF},
	{"getTaint",		"(Ljava/lang/Object;)I",					trishul_TrishulAnnotation_getTaintA},
	{"getTaint",		"(B)I",										trishul_TrishulAnnotation_getTaintB},
	{"getTaint",		"(C)I",										trishul_TrishulAnnotation_getTaintC},
	{"getTaint",		"(S)I",										trishul_TrishulAnnotation_getTaintS},
	{"getTaint",		"(Z)I",										trishul_TrishulAnnotation_getTaintZ},

//	protected final static native void taintObject (Object obj, int taint);
	{"taintObject",		"(Ljava/lang/Object;I)V",					taintObject},
	{"setObjectTaint",	"(Ljava/lang/Object;I)V",					setObjectTaint},
//	protected final static native <Type> Type untaintObject (Type obj, int untaint);
	{"untaintObject",	"(Ljava/lang/Object;I)Ljava/lang/Object;",	nativeUntaintObject},
//	protected final static native int  getObjectTaint (Object obj);
	{"getObjectTaint",	"(Ljava/lang/Object;)I",					getObjectTaint},

//	protected final static native void taintArray (Object array, int taint);
	{"taintArray",		"(Ljava/lang/Object;I)V",					taintArray},
	{"setArrayTaint",	"(Ljava/lang/Object;I)V",					setArrayTaint},
	{"untaintArray",	"(Ljava/lang/Object;I)Ljava/lang/Object;",	nativeUntaintArray},
//	protected final static native int  getArrayTaint (Object array);
	{"getArrayTaint",	"(Ljava/lang/Object;)I",					getArrayTaint},

//	protected final static native void taintArrayElement (Object array, int index, int taint);
	{"taintArrayElement",		"(Ljava/lang/Object;II)V",			taintArrayElement},
	{"setArrayElementTaint",	"(Ljava/lang/Object;II)V",			setArrayElementTaint},
	{"untaintArrayElement",		"(Ljava/lang/Object;II)V",			untaintArrayElement},
//	protected final static native int  getArrayElementTaint (Object array, int index);
	{"getArrayElementTaint",	"(Ljava/lang/Object;I)I",			getArrayElementTaint},

//	protected final static native Object $super (This $this, Object... arguments);
	{
		"$super",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)Ljava/lang/Object;",
		trishul_TrishulAnnotation_super
	},
	{
		"$superX",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)Ljava/lang/Object;",
		trishul_TrishulAnnotation_super
	},
	{
		"$superI",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)I",
		trishul_TrishulAnnotation_super
	},
	{
		"$superJ",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)J",
		trishul_TrishulAnnotation_super
	},
	{
		"$superD",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)D",
		trishul_TrishulAnnotation_super
	},
	{
		"$superF",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)F",
		trishul_TrishulAnnotation_super
	},
	{
		"$superC",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)C",
		trishul_TrishulAnnotation_super
	},
	{
		"$superS",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)S",
		trishul_TrishulAnnotation_super
	},
	{
		"$superZ",
		"(Ltrishul/TrishulAnnotation$This;[Ljava/lang/Object;)Z",
		trishul_TrishulAnnotation_super
	},

	{NULL, NULL, NULL}
};

/**********************************************************************************************************************/
static const NativeLibraryDigests *nativeDigests;
void trishulSetNativeLibraryDigests (const NativeLibraryDigests *digests)
{
	nativeDigests = digests;
}

/**********************************************************************************************************************/

void trishulCheckNativeLoad (const char *path)
{
	if (access (path, 0) >= 0)
	{
		// Check if a digest is present
		if (nativeDigests)
		{
			uint i;
			const char *name = strrchr (path, '/');
			for (i = 0; nativeDigests[i].name; i++)
			{
				if (!strcmp (name + 1, nativeDigests[i].name))
				{
					// Found, check the digest
					unsigned char buffer[0x10000];
					uint8 md[20];
					SHA_CTX ctx;
					SHA1_Init (&ctx);
					FILE *f = fopen (path, "rb");
					if (!f) break;
					for (;;)
					{
						int read = fread (buffer, 1, sizeof (buffer), f);
						if (!read)
							break;
						SHA1_Update (&ctx, buffer, read);
					}

					fclose (f);
					SHA1_Final (md, &ctx);
					if (memcmp (md, nativeDigests[i].digest, sizeof (md)))
						break;
					return;
				}
			}
		}

		// Not found, this is not allowed
		//fprintf (stderr, "Illegal attempt to load native library %s\n", path);
		//abort ();
	}
}

/**********************************************************************************************************************/
#ifdef KAFFE_VMDEBUG
/* Contains a string describing the current instruction, for debug purposed. Filled by taint_trace. */
char trishulCurrentInstruction[256];
#endif
/**********************************************************************************************************************/

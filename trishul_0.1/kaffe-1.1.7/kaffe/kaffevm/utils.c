#include "utils.h"
#include "thread.h"
#include "trishul.h"
#include "stringSupport.h"
#include <openssl/evp.h>
/**********************************************************************************************************************/

void decodeString (Hjava_lang_String *string, StringDecode *out)
{
	out->length = (*THREAD_JNIENV())->GetStringUTFLength	(THREAD_JNIENV(), string);
	out->data 	= (*THREAD_JNIENV())->GetStringUTFChars 	(THREAD_JNIENV(), string, &out->isCopy);
}

/**********************************************************************************************************************/

void releaseDecodedString (Hjava_lang_String *string, StringDecode *s)
{
	(*THREAD_JNIENV())->ReleaseStringUTFChars (THREAD_JNIENV(), string, s->data);
}

/**********************************************************************************************************************/

bool checkException ()
{
	Hjava_lang_Object *e;

	/* Display exception stack trace */
	if ((e = (*THREAD_JNIENV())->ExceptionOccurred(THREAD_JNIENV())) != NULL)
	{
		(*THREAD_JNIENV())->DeleteLocalRef(THREAD_JNIENV(), e);
		(*THREAD_JNIENV())->ExceptionDescribe(THREAD_JNIENV());
		(*THREAD_JNIENV())->ExceptionClear(THREAD_JNIENV());
		return false;
	}
	else
	{
		return true;
	}
}

/**********************************************************************************************************************/

bool clearException ()
{
	Hjava_lang_Object *e;

	/* Display exception stack trace */
	if ((e = (*THREAD_JNIENV())->ExceptionOccurred(THREAD_JNIENV())) != NULL)
	{
		(*THREAD_JNIENV())->DeleteLocalRef(THREAD_JNIENV(), e);
		(*THREAD_JNIENV())->ExceptionClear(THREAD_JNIENV());
		return true;
	}
	else
	{
		return false;
	}
}

/**********************************************************************************************************************/

void exitOnException ()
{
	if (!checkException ())
		exit (1);
}

/**********************************************************************************************************************/

bool classExtends (Hjava_lang_Class *class, JavaClass *super)
{
	ensureClassLoaded (super);
	return classExtends2 (class, super->class);
}

/**********************************************************************************************************************/

bool isRuntimeException (Hjava_lang_Class *class)
{
	while (class)
	{
		if (!strcmp (class->name->data, "java/lang/RuntimeException")) return true;
		class = class->superclass;
	}
	return false;
}

/**********************************************************************************************************************/

bool classExtends2 (Hjava_lang_Class *class, Hjava_lang_Class *super)
{
	while (class)
	{
		if (class == super) return true;
		class = class->superclass;
	}
	return false;
}

/**********************************************************************************************************************/

void ensureClassLoaded (JavaClass *class)
{
	if (!class->class)
	{
		class->class = MakeGlobalRef ((*THREAD_JNIENV())->FindClass(THREAD_JNIENV(), class->name));
		if (!class->class)
		{
			throwNamedException ("java/lang/NoClassDefFoundError", class->name);
			exitOnException ();
		}
	}
}

/**********************************************************************************************************************/

void ensureMethodLoaded (JavaMethod *method)
{
	if (!method->method)
	{
		ensureClassLoaded (method->class);

		if (method->isStatic)
		{
			method->method = MakeGlobalRef ((*THREAD_JNIENV())->GetStaticMethodID (THREAD_JNIENV(), method->class->class,
																				method->name, method->sig));
		}
		else
		{
			method->method = MakeGlobalRef ((*THREAD_JNIENV())->GetMethodID (THREAD_JNIENV(), method->class->class,
																				method->name, method->sig));
		}
		if (!method->method)
		{
			throwNamedException ("java/lang/NoSuchMethodError", "%s.%s %s",
									method->class->name, method->name, method->sig);
			exitOnException ();
		}
	}
}

/**********************************************************************************************************************/

void ensureFieldLoaded (JavaField *field)
{
	if (!field->field)
	{
		ensureClassLoaded (field->class);

		if (field->isStatic)
		{
			field->field = MakeGlobalRef ((*THREAD_JNIENV())->GetStaticFieldID (THREAD_JNIENV(),
												field->class->class, field->name, field->type));
		}
		else
		{
			field->field = MakeGlobalRef ((*THREAD_JNIENV())->GetFieldID (THREAD_JNIENV(),
												field->class->class, field->name, field->type));
		}
		if (!field->field)
		{
			throwNamedException ("java/lang/NoSuchFieldError", "%s.%s",
									field->class->name, field->name);
			exitOnException ();
		}
	}
}

/**********************************************************************************************************************/

typedef struct
{
	const char	*sig;
	JavaClass	class;
	JavaMethod	ctor;
	JavaMethod	accessor;
} BoxInfo;

#define BOX_INFO(cls,type,name)																						\
BoxInfo BOX_##type =																								\
{																													\
	#type,																											\
	{#cls, NULL},																									\
	{&BOX_##type.class, "<init>", "(" #type ")V", NULL, false},														\
	{&BOX_##type.class, #name "Value", "()" #type, NULL, false}														\
};

static BOX_INFO (java/lang/Integer, 	I, int);
static BOX_INFO (java/lang/Boolean, 	Z, boolean);
static BOX_INFO (java/lang/Short, 		S, short);
static BOX_INFO (java/lang/Byte, 		B, byte);
static BOX_INFO (java/lang/Character, 	C, char);
static BOX_INFO (java/lang/Float, 		F, float);
static BOX_INFO (java/lang/Double, 		D, double);
static BOX_INFO (java/lang/Long, 		J, long);

static BoxInfo *BOXES[] =
{
	&BOX_I, &BOX_Z, &BOX_S, &BOX_B, &BOX_C, &BOX_F, &BOX_D, &BOX_J
};

/**********************************************************************************************************************/

bool classEqualsSignature (const jclass class, const char *sig)
{
	// TODO: better solution without loading
	errorInfo einfo;
	return (getClassFromSignaturePart (sig, NULL, &einfo) == class);
}

/**********************************************************************************************************************/

static jclass unbox_class (const jclass class)
{
	unsigned int i;
	for (i = 0; i < sizeof (BOXES) / sizeof (BOXES[0]); i++)
	{
		BoxInfo *box = BOXES[i];
		ensureClassLoaded (&box->class);
		if (class == box->class.class)
		{
			return getClassFromSignaturePart (box->sig, NULL , NULL);
		}
	}
	return class;
}

/**********************************************************************************************************************/

bool classEqualsUnboxed (const jclass c1, const jclass c2)
{
	return (unbox_class (c1) == unbox_class (c2));
}

/**********************************************************************************************************************/

// TODO: make tables for names, type ids
static Hjava_lang_Object *boxPrimitive (BoxInfo *box, jvalue *value)
{
	ensureMethodLoaded (&box->ctor);
	Hjava_lang_Object *obj = (*THREAD_JNIENV())->NewObjectA (THREAD_JNIENV(), box->class.class,
																		box->ctor.method, value);
	exitOnException ();
	if (value->taint)
		taintObject (obj, value->taint);
	return obj;
}

/**********************************************************************************************************************/

static void unboxPrimitive (BoxInfo *box, Hjava_lang_Object *obj, jvalue *value)
{
	// TODO: check if taint is copied back into value
	ensureMethodLoaded (&box->accessor);
	KaffeVM_callMethodA (box->accessor.method, METHOD_NATIVECODE(box->accessor.method), obj, 0, NULL, value, 0);
	exitOnException ();
}

/**********************************************************************************************************************/

Hjava_lang_Object *boxIfRequired (const char *type, jvalue *val)
{
	switch (*type)
	{
	// Primitive argument, must be boxed
	case 'I': return boxPrimitive (&BOX_I, val);
	case 'Z': return boxPrimitive (&BOX_Z, val);
	case 'S': return boxPrimitive (&BOX_S, val);
	case 'B': return boxPrimitive (&BOX_B, val);
	case 'C': return boxPrimitive (&BOX_C, val);
	case 'F': return boxPrimitive (&BOX_F, val);
	case 'D': return boxPrimitive (&BOX_D, val);
	case 'J': return boxPrimitive (&BOX_J, val);

	// Reference argument, simply copy into array
	case '[':
	case 'L':
		return val->l;

	// Void
	case 'V':
		return NULL;

	// Error.
	default:
		abort ();
		return NULL;
	}
}

/**********************************************************************************************************************/

void unboxIfRequired (const char *type, Hjava_lang_Object *object, jvalue *variable)
{
	switch (*type)
	{
	// Primitive argument, must be boxed
	case 'I': unboxPrimitive (&BOX_I, object, variable); return;
	case 'Z': unboxPrimitive (&BOX_Z, object, variable); return;
	case 'S': unboxPrimitive (&BOX_S, object, variable); return;
	case 'B': unboxPrimitive (&BOX_B, object, variable); return;
	case 'C': unboxPrimitive (&BOX_C, object, variable); return;
	case 'F': unboxPrimitive (&BOX_F, object, variable); return;
	case 'D': unboxPrimitive (&BOX_D, object, variable); return;
	case 'J': unboxPrimitive (&BOX_J, object, variable); return;

	// Reference argument, simply copy into array
	case '[':
	case 'L':
		variable->l = object;
		return;

	// Void
	case 'V':
		variable->l = NULL;
		return;

	// Error.
	default:
		abort ();
	}
}

/**********************************************************************************************************************/

Hjava_lang_Object *parametersToObjectArray (const Method *method, jvalue *args)
{
	// TODO: remove class reference?
	uint i;
	Hjava_lang_Object *array = (*THREAD_JNIENV())->NewObjectArray (THREAD_JNIENV(), METHOD_NARGS (method),
		(*THREAD_JNIENV())->FindClass (THREAD_JNIENV(), "java/lang/Object"), NULL);
	for (i = 0; i < METHOD_NARGS (method); i++)
	{
		OBJARRAY_DATA(array)[i] = boxIfRequired (METHOD_ARG_TYPE (method, i), &args[i]);

		if (METHOD_ARG_TYPE (method, i)[0] == 'J' || METHOD_ARG_TYPE (method, i)[0] == 'D')
			args++;
	}

	return array;
}

/**********************************************************************************************************************/

Hjava_lang_Object *formalParametersToClassArray (const Method *method)
{
	// TODO: remove class reference?
	uint i;
	Hjava_lang_Object *array = (*THREAD_JNIENV())->NewObjectArray (THREAD_JNIENV(), METHOD_NARGS (method),
		(*THREAD_JNIENV())->FindClass (THREAD_JNIENV(), "java/lang/Class"), NULL);
	for (i = 0; i < METHOD_NARGS (method); i++)
	{
		errorInfo einfo; memset (&einfo, 0, sizeof (einfo));
		OBJARRAY_DATA(array)[i] = (Hjava_lang_Object *)
							getClassFromSignaturePart (METHOD_ARG_TYPE (method, i), NULL , &einfo);
	}

	return array;
}

/**********************************************************************************************************************/

void objectArrayToParameters(const Method *method, HArrayOfObject *array, jvalue *args)
{
	uint i;
	for (i = 0; i < METHOD_NARGS (method); i++)
	{
		unboxIfRequired (METHOD_ARG_TYPE (method, i), OBJARRAY_DATA(array)[i], &args[i]);
	}
}

/**********************************************************************************************************************/

static Method* findMethodLocalNoReturnType (Hjava_lang_Class* class, Utf8Const* name, Utf8Const* signature)
{
	Method* mptr;
	int n;

	/*
	 * Lookup method - this could be alot more efficient but never mind.
	 * Also there is no attempt to honour PUBLIC, PRIVATE, etc.
	 */
	n = CLASS_NMETHODS(class);
	for (mptr = CLASS_METHODS(class); --n >= 0; ++mptr) {
		if (utf8ConstEqual (name, mptr->name) &&
			!strncmp (signature->data, METHOD_SIG(mptr)->data, strlen (signature->data))) {
			return (mptr);
		}
	}

	return NULL;
}

/**********************************************************************************************************************/

Method *findMethodNoReturnType (Hjava_lang_Class* class, Utf8Const* name, Utf8Const* signature)
{
	/*
	 * Lookup method - this could be alot more efficient but never mind.
	 * Also there is no attempt to honour PUBLIC, PRIVATE, etc.
	 */
	for (; class != NULL; class = class->superclass) {
		Method* mptr = findMethodLocalNoReturnType(class, name, signature);
		if (mptr != NULL) {
			return mptr;
		}
	}

	return (NULL);
}

/**********************************************************************************************************************/

void throwExceptionObject (Hjava_lang_Object *x)
{
	errorInfo 	einfo; memset (&einfo, 0, sizeof (einfo));
	einfo.type = KERR_RETHROW;
	einfo.throwable = (Hjava_lang_Throwable *) x;
	throwError(&einfo);
}

/**********************************************************************************************************************/

void throwTypedException (JavaClass *class, const char *format, ...)
{
	ensureClassLoaded (class);

	errorInfo 	einfo;
	va_list 	args;
   	va_start (args, format);
	vpostExceptionMessage (&einfo, class->name, format, args);
	throwError(&einfo);
}

/**********************************************************************************************************************/

void throwNamedException (const char *className, const char *format, ...)
{
	errorInfo 	einfo;
	va_list 	args;
   	va_start (args, format);
	vpostExceptionMessage (&einfo, className, format, args);
	throwError(&einfo);
}

/**********************************************************************************************************************/

jobject MakeGlobalRef (jobject local)
{
	if (!local) return NULL;
	jobject global = (*THREAD_JNIENV())->NewGlobalRef(THREAD_JNIENV(), local);
	(*THREAD_JNIENV())->DeleteLocalRef(THREAD_JNIENV(), local);
	return global;
}

/**********************************************************************************************************************/
/* Dispatcher for native trishul functions */
/**********************************************************************************************************************/

extern const BuiltinFunctionInfo_t DISPATCH_trishul_Engine[];
extern const BuiltinFunctionInfo_t DISPATCH_trishul_TrishulAnnotation[];
extern const BuiltinFunctionInfo_t DISPATCH_trishul_test_taint_TaintTest[];
extern const BuiltinFunctionInfo_t DISPATCH_trishul_p_Engine[];
extern const BuiltinFunctionInfo_t DISPATCH_trishul_p_AbstractAction[];
extern const BuiltinFunctionInfo_t DISPATCH_trishul_p_ConcreteAction[];

static BuiltinClassInfo_t BUILTIN_CLASSES[] =
{
	{"trishul/TrishulAnnotation",		NULL, DISPATCH_trishul_TrishulAnnotation},
	{"trishul/Tainter",					NULL, DISPATCH_trishul_Engine},
	{"trishul/test/taint/TaintTest", 	NULL, DISPATCH_trishul_test_taint_TaintTest},
	{"trishul_p/Engine", 				NULL, DISPATCH_trishul_p_Engine},
	{"trishul_p/AbstractAction", 		NULL, DISPATCH_trishul_p_AbstractAction},
	{"trishul_p/ConcreteAction",		NULL, DISPATCH_trishul_p_ConcreteAction},
};

/**********************************************************************************************************************/

void *lookupBuiltinFunction (Method *m)
{
	unsigned int i, j;
	for (i = 0; i < sizeof (BUILTIN_CLASSES) / sizeof (BUILTIN_CLASSES[0]); i++)
	{
		BuiltinClassInfo_t *classInfo = &BUILTIN_CLASSES[i];

		if (!classInfo->class)
		{
			// Try to load the class on first use, if it is the requested class
			if (!strcmp (classInfo->name, m->class->name->data))
			{
				errorInfo error; memset (&error, 0, sizeof (error));
				Utf8Const *nameUtf8 = utf8ConstNew (classInfo->name, strlen (classInfo->name));
				classInfo->class = loadClass (nameUtf8, NULL, &error);
				if (!classInfo->class)
				{
					checkException ();
					fprintf (stderr, "Unable to load native class %s: %s\n", classInfo->name, error.mess);
					abort ();
				}
				utf8ConstRelease (nameUtf8);
			}
			else
			{
				continue;
			}
		}

		if (m->class == classInfo->class)
		{
			for (j = 0; classInfo->functions[j].name; j++)
			{
				const BuiltinFunctionInfo_t *funcInfo = &classInfo->functions[j];
				if (!strcmp (funcInfo->name, m->name->data) &&
					!strcmp (funcInfo->sig, m->parsed_sig->signature->data))
				{
					return funcInfo->func;
				}

			}
		}
	}

	return NULL;
}

/**********************************************************************************************************************/

bool javaStringsEqual (const HString *s1, const HString *s2)
{
	if (STRING_SIZE(s1) != STRING_SIZE(s2))
		return false;

	int i;
	for (i = 0; i < STRING_SIZE(s1); i++)
	{
		if (STRING_DATA(s1)[i] != STRING_DATA(s2)[i])
		{
			return false;
		}
	}

	return true;
}

/**********************************************************************************************************************/

bool javaStringEqualsUtf8ConstSubstring (const HString *s1, const Utf8Const *s2, int s2_start, int s2_length)
{
	if (s2_length == 0)
	{
		s2_length = strlen (s2->data + s2_start);
	}
	//printf ("COMPARE: %s %.*s\n", stringJava2Utf8ConstReplace (s1, 0, 0)->data, s2_length, s2->data + s2_start);

	if (STRING_SIZE(s1) != s2_length)
		return false;
	int i;
	for (i = 0; i < s2_length; i++)
	{
		if (STRING_DATA(s1)[i] != s2->data[s2_start + i])
			return false;
	}
	return true;
}

/**********************************************************************************************************************/

static JAVA_CLASS 	(java_net_URL, java/net/URL)
	static JAVA_CTOR	(java_net_URL, ctor, 	"(Ljava/lang/String;)V");

Hjava_lang_Object *urlFromCString (const char *string)
{
	ensureMethodLoaded (&java_net_URL_ctor);
	Utf8Const *urlUTF8 = utf8ConstNew (string, strlen (string));
	Hjava_lang_Object *url = (*THREAD_JNIENV())->NewObject (THREAD_JNIENV(),
						java_net_URL.class, java_net_URL_ctor.method, utf8Const2Java (urlUTF8));
	exitOnException ();
	utf8ConstRelease (urlUTF8);
	return url;
}

/**********************************************************************************************************************/
#ifdef TRISHUL_MEASURE_ANALYSIS_SIZE

uint recordAllocContextAnalysis = 0;
void RecordAllocContextAnalysis (uint size) {recordAllocContextAnalysis += size;}

uint recordAllocStorageRecord = 0;
void RecordAllocStorageRecord (uint size) {recordAllocStorageRecord += size;}

#endif
/**********************************************************************************************************************/

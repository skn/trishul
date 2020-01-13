#ifndef __utils_h
#define	__utils_h
/**********************************************************************************************************************/
#include "classMethod.h"
/**********************************************************************************************************************/

typedef struct
{
	int 		length;
	const char	*data;
	jboolean	isCopy;
} StringDecode;

/**********************************************************************************************************************/

void decodeString (Hjava_lang_String *string, StringDecode *out);

void releaseDecodedString (Hjava_lang_String *string, StringDecode *s);

bool checkException ();
bool clearException ();

void exitOnException ();

typedef struct
{
	const char 	*name;
	jclass 		class;
} JavaClass;

typedef struct
{
	JavaClass	*class;
	const char	*name;
	const char	*sig;
	jmethodID	method;
	bool		isStatic;
} JavaMethod;

typedef struct
{
	JavaClass	*class;
	const char	*name;
	const char	*type;
	jfieldID	field;
	bool		isStatic;
} JavaField;

#define JAVA_CLASS(objname,javaname) 			JavaClass  objname = {#javaname, NULL};
#define JAVA_METHOD(clsname,name,sig) 			JavaMethod clsname##_##name = {&clsname,#name,sig,NULL,false};
#define JAVA_STATIC_METHOD(clsname,name,sig) 	JavaMethod clsname##_##name = {&clsname,#name,sig,NULL,true};
#define JAVA_CTOR(clsname,name,sig) 			JavaMethod clsname##_##name = {&clsname,"<init>",sig,NULL,false};
#define JAVA_FIELD(clsname,name,type) 			JavaField  clsname##_##name = {&clsname,#name,type,NULL,false};
#define JAVA_STATIC_FIELD(clsname,name,type) 	JavaField  clsname##_##name = {&clsname,#name,type,NULL,true};


void ensureClassLoaded (JavaClass *class);

void ensureMethodLoaded (JavaMethod *method);

void ensureFieldLoaded (JavaField *field);

bool classExtends (Hjava_lang_Class *class, JavaClass *super);
bool classExtends2 (Hjava_lang_Class *class, Hjava_lang_Class *super);
bool isRuntimeException (Hjava_lang_Class *class);

bool classEqualsSignature (const jclass class, const char *sig);

bool classEqualsUnboxed (const jclass c1, const jclass c2);

/**
 * Looks up a method for which te return type is not yet known, and thus not included in the signature.
 */
Method *findMethodNoReturnType (Hjava_lang_Class *class, Utf8Const* name, Utf8Const* signature);

/**
 * Creates an array of Java objects containt the method's parameters. Any primitive parameters are
 * boxed automatically.
 *
 * @retun The array, which is allocated on the garbage collected heap.
 */
Hjava_lang_Object *parametersToObjectArray (const Method *method, jvalue *args);

Hjava_lang_Object *formalParametersToClassArray (const Method *method);

/**
 * Extracts an array of jvalue items from an array of Java objects containing the method's parameters.
 * Any primitive parameters are unboxed automatically.
 *
 * @param args  Receives the extracted parameters. This array must be large enough to hold all parameters,
 *  			i.e. at least the size of array.
 */
void objectArrayToParameters(const Method *method, HArrayOfObject *array, jvalue *args);

/**
 * Ensures the value is an object, boxing a primitive value if required. If a void value is passed, NULL is returned.
 */
Hjava_lang_Object *boxIfRequired (const char *type, jvalue *val);

/**
 * Unboxes the object if it is a primitive wrapper.
 * @param value Receives the unboxed primitive, NULL if the object was a Void,
 *				or the original object if unboxing was not needed.
 */
void unboxIfRequired (const char *type, Hjava_lang_Object *object, jvalue *variable);

/**
 * Throws an already created exception.
 */
void throwExceptionObject (Hjava_lang_Object *x);

/**
 * Throws an exception containing a printf-style formatted string.
 */
void throwTypedException (JavaClass *class, const char *format, ...);
void throwNamedException (const char *className, const char *format, ...);

/**
 * Makes a global reference from a local reference.
 * It is safe to call this method with a NULL reference.
 */
jobject MakeGlobalRef (jobject local);

typedef struct BUILTINFUNCTIONINFO
{
	const char	*name;
	const char	*sig;
	void		*func;
} BuiltinFunctionInfo_t;

typedef struct BUILTINCLASSINFO
{
	const char					*name;
	const Hjava_lang_Class		*class;
	const BuiltinFunctionInfo_t	*functions;
} BuiltinClassInfo_t;

void *lookupBuiltinFunction (Method *m);

bool javaStringsEqual (const HString *s1, const HString *s2);
bool javaStringEqualsUtf8ConstSubstring (const HString *s1, const Utf8Const *s2, int s2_start, int s2_length);
Hjava_lang_Object *urlFromCString (const char *string);

/**********************************************************************************************************************/
//#define TRISHUL_MEASURE_ANALYSIS_SIZE
//#define TRISHUL_MEASURE_CODE_SIZE
//#define TRISHUL_MEASURE_NO_POLYMER
//#define TRISHUL_MEASURE_ANALYSIS_TIME
//#define TRISHUL_MEASURE_FALLBACK
/**********************************************************************************************************************/
#ifdef TRISHUL_MEASURE_ANALYSIS_SIZE
	void RecordAllocContextAnalysis	(uint size);
	void RecordAllocStorageRecord 	(uint size);
#else
	#define RecordAllocContextAnalysis(size)
	#define RecordAllocStorageRecord(size)
#endif
/**********************************************************************************************************************/
#endif /* __utils_h */

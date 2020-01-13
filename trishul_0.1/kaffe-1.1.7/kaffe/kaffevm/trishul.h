#ifndef __trishul_h
#define	__trishul_h
/**********************************************************************************************************************/
#include "object.h"
#include "slots.h"
#include "trishul2.h"
/**********************************************************************************************************************/

#ifdef KAFFE_VMDEBUG
	/* Sets the class to use for debugging purposes.
	 */
	void taintInitialize (Hjava_lang_Class *mainClass);
#endif

void taintRelease ();

#ifdef KAFFE_VMDEBUG

void addTaintDebugClass 	(const Hjava_lang_Class *);
bool isTaintDebugClass 		(const Hjava_lang_Class *);

void setTaintDebugLevel (int level);
int getTaintDebugLevel ();

#endif

#ifdef KAFFE_VMDEBUG
	void do_taint_trace (int level, const char *insName, uint pc,
						 const Method *meth, const char *msg, ...);

	#define taint_trace(level,x)  ((isTaintDebugClass (meth->class) && level <= getTaintDebugLevel ()) 				\
			? (printf ("[%s.%s %s:%u]: %s: ", meth->class->name->data, meth->name->data, METHOD_SIGD(meth), 		\
							pc, trishulCurrentInstruction), printf x, printf ("\n")) : (void) 0)

	#define taint_trace_noins(level,x)  ((isTaintDebugClass (meth->class) && level <= getTaintDebugLevel ()) 		\
			? (printf ("[%s.%s %s:%u]: ", meth->class->name->data, meth->name->data, METHOD_SIGD(meth), pc)			\
							, printf x, printf ("\n")) : (void) 0)

	#define taint_trace_native(level,x) \
	do { if (level <= getTaintDebugLevel ()) \
	{ \
		DBGIF({ \
				printf ("[%s]: NATIVE: ", __func__); \
				printf x; \
				printf ("\n"); \
	}); \
	}} while (0)

	#define taint_trace_native_class(level,class,x) \
	do { if (level <= getTaintDebugLevel () && isTaintDebugClass (class)) \
	{ \
		DBGIF({ \
				printf ("[%s]: NATIVE: ", __func__); \
				printf x; \
				printf ("\n"); \
	}); \
	}} while (0)

#else
	#define taint_trace(level,x) 						0
	#define do_taint_trace 								(void)
	#define taint_trace_noins(level,x)					0
	#define taint_trace_native(level,x)					0
	#define taint_trace_native_class(level,class,x)		0
#endif

/**********************************************************************************************************************/

#define taintMerge2(t1,t2) 					((taint_t) ((t1) | (t2)))
#define taintMerge3(t1,t2,t3) 				((taint_t) ((t1) | (t2) | (t3)))
#define taintMerge4(t1,t2,t3,t4) 			((taint_t) ((t1) | (t2) | (t3) | (t4)))
#define taintMerge5(t1,t2,t3,t4,t5) 		((taint_t) ((t1) | (t2) | (t3) | (t4) | (t5)))
#define taintMerge6(t1,t2,t3,t4,t5,t6) 		((taint_t) ((t1) | (t2) | (t3) | (t4) | (t5) | (t6)))

#define taintAdd1(target,t1)				((target) |= ((taint_t) (t1)))
#define taintAdd2(target,t1,t2)				((target) |= ((taint_t) (t1) | (t2)))
#define taintAdd3(target,t1,t2,t3)			((target) |= ((taint_t) (t1) | (t2) | (t3)))
#define taintAdd4(target,t1,t2,t3,t4)		((target) |= ((taint_t) (t1) | (t2) | (t3) | (t4)))
#define taintAdd5(target,t1,t2,t3,t4,t5)	((target) |= ((taint_t) (t1) | (t2) | (t3) | (t4) | (t5)))

#define taintSub1(target,t1)				((target) &= ~((taint_t) (t1)))
#define taintMinus(t1,t2)					((taint_t) ((t1) & ~(t2)))

/**
 * Initializes the engine to a newly created object of the specified class.
 * The class must extend trishul/Engine and contain a default constructor, otherwise an
 * exception is thrown.
 */
void addEngineClass (Hjava_lang_Class *, const char *enginePolicyURL);

/**
 * Retrieves the engine object.
 */
Hjava_lang_Object *trishulGetEngine ();

/* If not NULL, the hooked method that is currently invoked through invokeMethod. It will not be hooked. */
extern Method *trishulUnhookMethod;

void taintArray (Hjava_lang_Object *array, jint taint);
void untaintArray (Hjava_lang_Object *array, jint untaint);
jint getArrayTaint (const Hjava_lang_Object *obj);
void taintArrayElement (Hjava_lang_Object *array, jint index, jint taint);
void untaintArrayElement (Hjava_lang_Object *array, jint index, jint untaint);
jint getArrayElementTaint (const Hjava_lang_Object *obj, jint index);
void taintObject (Hjava_lang_Object *object, jint taint);
void untaintObject (Hjava_lang_Object *object, jint untaint);
jint getObjectTaint (const Hjava_lang_Object *obj);

/**
 * Looks up the name of the class that contains annotations for the specified class.
 */
Utf8Const *trishulAnnotationClassName(Hjava_lang_Class *class);

/**
 * Process the annotations class in class->annotations.
 */
void trishulProcessAnnotations (Hjava_lang_Class *class);

void *trishulAnnotatedInvokePush (Method **original);
void  trishulAnnotatedInvokePop  (void *stored);
extern Method *trishulAnnotatedOriginalStack;

extern taint_t *current_context_taint;

void trishulCheckNativeLoad (const char *path);

typedef struct NativeLibraryDigests
{
	unsigned char digest[20];
	const char *name;
} NativeLibraryDigests;

void trishulSetNativeLibraryDigests (const NativeLibraryDigests *digests);

#ifdef KAFFE_VMDEBUG
	/* Contains a string describing the current instruction, for debug purposed. Filled by taint_trace. */
	extern char trishulCurrentInstruction[256];
#endif

/**********************************************************************************************************************/
#endif /* __trishul_h */

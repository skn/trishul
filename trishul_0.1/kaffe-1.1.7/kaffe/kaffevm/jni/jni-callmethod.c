/*
 * jni-callmethod.c
 * Java Native Interface - CallXMethod JNI functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#include "config.h"
#include "baseClasses.h"
#include "threadData.h"
#include "classMethod.h"
#include "exception.h"
#include "jni.h"
#include "jni_i.h"
#include "jnirefs.h"
#include "jni_funcs.h"

/*
 * Find the function to be called when meth is invoked on obj
 */
static inline void*
getMethodFunc (Method* meth, Hjava_lang_Object *obj)
{
  if (obj && CLASS_IS_INTERFACE (meth->class)) {
    register void ***implementors;
    register Hjava_lang_Class *clazz;

    assert (meth->idx >= 0);

    implementors = meth->class->implementors;
    clazz = OBJECT_CLASS(obj);

    assert (implementors != NULL && clazz->impl_index <= (int)implementors[0]);

    return implementors[clazz->impl_index][meth->idx + 1];
  } else {
    return meth->idx >= 0 ? obj->vtable->method[meth->idx] : METHOD_NATIVECODE (meth);
  }
}

/*
 * The JNI functions.
 */



#define NONTAINTMETHODS1(type,name) \
type KaffeJNI_CallNonvirtual##name##MethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args) \
{return KaffeJNI_CallNonvirtual##name##MethodTaintV (env, obj, 0, cls, meth, args);} \
type KaffeJNI_CallNonvirtual##name##Method(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...) \
{va_list args; va_start(args, meth);return KaffeJNI_CallNonvirtual##name##MethodTaintV (env, obj, 0, cls, meth, args);} \
type KaffeJNI_CallNonvirtual##name##MethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args) \
{return KaffeJNI_CallNonvirtual##name##MethodTaintA (env, obj, 0, cls, meth, args);} \
type KaffeJNI_Call##name##MethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args) \
{return KaffeJNI_Call##name##MethodTaintV (env, obj, 0, meth, args);} \
type KaffeJNI_Call##name##MethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args) \
{return KaffeJNI_Call##name##MethodTaintA (env, obj, 0, meth, args);}

#define NONTAINTMETHODS2(type,name) \
type KaffeJNI_Call##name##Method(JNIEnv* env, jobject obj, jmethodID meth, ...) \
{va_list args; type r; va_start(args, meth);r = KaffeJNI_Call##name##MethodTaintV (env, obj, 0, meth, args); va_end (args); return r;} \

#define NONTAINTMETHODS(type,name) NONTAINTMETHODS1(type,name) NONTAINTMETHODS2(type,name)


NONTAINTMETHODS(jobject,Object)
NONTAINTMETHODS(jbyte,Byte)
NONTAINTMETHODS(jint,Int)
NONTAINTMETHODS(jshort,Short)
NONTAINTMETHODS(jboolean,Boolean)
NONTAINTMETHODS(jchar,Char)
NONTAINTMETHODS(jlong,Long)
NONTAINTMETHODS(jfloat,Float)
NONTAINTMETHODS(jdouble,Double)

NONTAINTMETHODS1(void,Void)
void KaffeJNI_CallVoidMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args; va_start(args, meth);
	KaffeJNI_CallVoidMethodTaintV (env, obj, 0, meth, args);
	va_end (args);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning VOID
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_CallVoidMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  /*
   * callMethodA will have to unveil the objects contained in the argument lists.
   * This is not really pretty but more efficient because we do not parse the signature
   * twice.
   */
  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, NULL);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallVoidMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;

  BEGIN_EXCEPTION_HANDLING_VOID();

  va_start(args, meth);
  KaffeJNI_CallVoidMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallVoidMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  /*
   * callMethodA will have to unveil the objects contained in the argument lists.
   * This is not really pretty but more efficient because we do not parse the signature
   * twice.
   */
  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, NULL, 0);

  END_EXCEPTION_HANDLING();
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning Object
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_CallNonvirtualObjectMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  obj_local = unveil(obj);
  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

jobject
KaffeJNI_CallNonvirtualObjectMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jobject r;

  BEGIN_EXCEPTION_HANDLING(NULL);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualObjectMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jobject
KaffeJNI_CallNonvirtualObjectMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning boolean
 * ------------------------------------------------------------------------------
 */

jboolean
KaffeJNI_CallNonvirtualBooleanMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

jboolean
KaffeJNI_CallNonvirtualBooleanMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jboolean r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualBooleanMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jboolean
KaffeJNI_CallNonvirtualBooleanMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning byte
 * ------------------------------------------------------------------------------
 */
jbyte
KaffeJNI_CallNonvirtualByteMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

jbyte
KaffeJNI_CallNonvirtualByteMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jbyte r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualByteMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_CallNonvirtualByteMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning char
 * ------------------------------------------------------------------------------
 */

jchar
KaffeJNI_CallNonvirtualCharMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

jchar
KaffeJNI_CallNonvirtualCharMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jchar r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualCharMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_CallNonvirtualCharMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning short
 * ------------------------------------------------------------------------------
 */

jshort
KaffeJNI_CallNonvirtualShortMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

jshort
KaffeJNI_CallNonvirtualShortMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jshort r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualShortMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_CallNonvirtualShortMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning int
 * ------------------------------------------------------------------------------
 */

jint
KaffeJNI_CallNonvirtualIntMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

jint
KaffeJNI_CallNonvirtualIntMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jint r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualIntMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_CallNonvirtualIntMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning Object
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_CallObjectMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}


jobject
KaffeJNI_CallObjectMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jobject r;

  BEGIN_EXCEPTION_HANDLING(NULL);

  va_start(args, meth);
  r = KaffeJNI_CallObjectMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jobject
KaffeJNI_CallObjectMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.l);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning boolean
 * ------------------------------------------------------------------------------
 */

jboolean
KaffeJNI_CallBooleanMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

jboolean
KaffeJNI_CallBooleanMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jboolean r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallBooleanMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jboolean
KaffeJNI_CallBooleanMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning byte
 * ------------------------------------------------------------------------------
 */

jbyte
KaffeJNI_CallByteMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
   o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

jbyte
KaffeJNI_CallByteMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jbyte r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallByteMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_CallByteMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), obj, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning char
 * ------------------------------------------------------------------------------
 */

jchar
KaffeJNI_CallCharMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

jchar
KaffeJNI_CallCharMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jchar r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallCharMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_CallCharMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning short
 * ------------------------------------------------------------------------------
 */

jshort
KaffeJNI_CallShortMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

jshort
KaffeJNI_CallShortMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jshort r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallShortMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_CallShortMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning int
 * ------------------------------------------------------------------------------
 */

jint
KaffeJNI_CallIntMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

jint
KaffeJNI_CallIntMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jint r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallIntMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_CallIntMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning long
 * ------------------------------------------------------------------------------
 */

jlong
KaffeJNI_CallLongMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

jlong
KaffeJNI_CallLongMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jlong r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallLongMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_CallLongMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning float
 * ------------------------------------------------------------------------------
 */

jfloat
KaffeJNI_CallFloatMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

jfloat
KaffeJNI_CallFloatMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jfloat r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallFloatMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_CallFloatMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning double
 * ------------------------------------------------------------------------------
 */

jdouble
KaffeJNI_CallDoubleMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

jdouble
KaffeJNI_CallDoubleMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jmethodID meth, ...)
{
  va_list args;
  jdouble r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallDoubleMethodTaintV(env, obj, objTaint, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jdouble
KaffeJNI_CallDoubleMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning long
 * ------------------------------------------------------------------------------
 */

jlong
KaffeJNI_CallNonvirtualLongMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

jlong
KaffeJNI_CallNonvirtualLongMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jlong r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualLongMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_CallNonvirtualLongMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning float
 * ------------------------------------------------------------------------------
 */

jfloat
KaffeJNI_CallNonvirtualFloatMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

jfloat
KaffeJNI_CallNonvirtualFloatMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jfloat r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualFloatMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_CallNonvirtualFloatMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning double
 * ------------------------------------------------------------------------------
 */

jdouble
KaffeJNI_CallNonvirtualDoubleMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

jdouble
KaffeJNI_CallNonvirtualDoubleMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jdouble r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualDoubleMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jdouble
KaffeJNI_CallNonvirtualDoubleMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning void
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_CallNonvirtualVoidMethodTaintV(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, va_list args)
{
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, NULL);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallNonvirtualVoidMethodTaint(JNIEnv* env, jobject obj, taint_t objTaint, jclass cls, jmethodID meth, ...)
{
  va_list args;

  BEGIN_EXCEPTION_HANDLING_VOID();

  va_start(args, meth);
  KaffeJNI_CallNonvirtualVoidMethodTaintV(env, obj, objTaint, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallNonvirtualVoidMethodTaintA(JNIEnv* env UNUSED, jobject obj, taint_t objTaint, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, objTaint, args, NULL, 0);

  END_EXCEPTION_HANDLING();
}

/* ------------------------------------------------------------------------------
 * Static method calls returning Object
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_CallStaticObjectMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

jobject
KaffeJNI_CallStaticObjectMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jobject r;

  BEGIN_EXCEPTION_HANDLING(NULL);

  va_start(args, meth);
  r = KaffeJNI_CallStaticObjectMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jobject
KaffeJNI_CallStaticObjectMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning boolean
 * ------------------------------------------------------------------------------
 */

jboolean
KaffeJNI_CallStaticBooleanMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

jboolean
KaffeJNI_CallStaticBooleanMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jboolean r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticBooleanMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jboolean
KaffeJNI_CallStaticBooleanMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning byte
 * ------------------------------------------------------------------------------
 */

jbyte
KaffeJNI_CallStaticByteMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

jbyte
KaffeJNI_CallStaticByteMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jbyte r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticByteMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_CallStaticByteMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning char
 * ------------------------------------------------------------------------------
 */

jchar
KaffeJNI_CallStaticCharMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

jchar
KaffeJNI_CallStaticCharMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jchar r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticCharMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_CallStaticCharMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning short
 * ------------------------------------------------------------------------------
 */

jshort
KaffeJNI_CallStaticShortMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

jshort
KaffeJNI_CallStaticShortMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jshort r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticShortMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_CallStaticShortMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning int
 * ------------------------------------------------------------------------------
 */

jint
KaffeJNI_CallStaticIntMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

jint
KaffeJNI_CallStaticIntMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jint r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticIntMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_CallStaticIntMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning long
 * ------------------------------------------------------------------------------
 */

jlong
KaffeJNI_CallStaticLongMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

jlong
KaffeJNI_CallStaticLongMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jlong r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticLongMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_CallStaticLongMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning float
 * ------------------------------------------------------------------------------
 */

jfloat
KaffeJNI_CallStaticFloatMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

jfloat
KaffeJNI_CallStaticFloatMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jfloat r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticFloatMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_CallStaticFloatMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning double
 * ------------------------------------------------------------------------------
 */

jdouble
KaffeJNI_CallStaticDoubleMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

jdouble
KaffeJNI_CallStaticDoubleMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jdouble r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticDoubleMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}


jdouble
KaffeJNI_CallStaticDoubleMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning void
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_CallStaticVoidMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, 0, args, &retval);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallStaticVoidMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;

  BEGIN_EXCEPTION_HANDLING_VOID();

  va_start(args, meth);
  KaffeJNI_CallStaticVoidMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallStaticVoidMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, 0, args, NULL, 0);

  END_EXCEPTION_HANDLING();
}

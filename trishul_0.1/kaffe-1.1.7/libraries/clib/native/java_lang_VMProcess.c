/* java_lang_VMProcess.c -- native code for java.lang.VMProcess
   Copyright (C) 1998, 1999, 2000, 2002, 2004 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

#include <config.h>

#include "java_lang_VMProcess.h"
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include "jsyscall.h"

#include "target_native.h"
#include "target_native_misc.h"
#include "trishul.h"

/* Internal functions */
static char *copy_string (JNIEnv * env, jobject string);
static char *copy_elem (JNIEnv * env, jobject stringArray, jint i);

/* Some O/S's don't declare 'environ' */
#if HAVE_CRT_EXTERNS_H
/* Darwin does not have a variable named environ
   but has a function which you can get the environ
   variable with.  */
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif /* HAVE_CRT_EXTERNS_H */

/*
 * Internal helper function to copy a String in UTF-8 format.
 */
static char *
copy_string (JNIEnv * env, jobject string)
{
  char errbuf[64];
  const char *utf;
  jclass clazz;
  char *copy;

  /* Check for null */
  if (string == NULL)
    {
      clazz = (*env)->FindClass (env, "java/lang/NullPointerException");
      if ((*env)->ExceptionOccurred (env))
	return NULL;
      (*env)->ThrowNew (env, clazz, NULL);
      (*env)->DeleteLocalRef (env, clazz);
      return NULL;
    }

  /* Extract UTF-8 */
  utf = (*env)->GetStringUTFChars (env, string, NULL);
  if ((*env)->ExceptionOccurred (env))
    return NULL;

  /* Copy it */
  if ((copy = strdup (utf)) == NULL)
    {
      TARGET_NATIVE_MISC_FORMAT_STRING1 (errbuf, sizeof (errbuf),
					 "strdup: %s", strerror (errno));
      clazz = (*env)->FindClass (env, "java/lang/InternalError");
      if ((*env)->ExceptionOccurred (env))
	return NULL;
      (*env)->ThrowNew (env, clazz, errbuf);
      (*env)->DeleteLocalRef (env, clazz);
    }

  /* Done */
  (*env)->ReleaseStringUTFChars (env, string, utf);
  return copy;
}

/*
 * Internal helper function to copy a String[] element in UTF-8 format.
 */
static char *
copy_elem (JNIEnv * env, jobject stringArray, jint i)
{
  jobject elem;
  char *rtn;

  elem = (*env)->GetObjectArrayElement (env, stringArray, i);
  if ((*env)->ExceptionOccurred (env))
    return NULL;
  if ((rtn = copy_string (env, elem)) == NULL)
    return NULL;
  (*env)->DeleteLocalRef (env, elem);
  return rtn;
}

/*
 * private final native void nativeSpawn(String[], String[], File)
 *	throws java/io/IOException
 */
JNIEXPORT void JNICALL
Java_java_lang_VMProcess_nativeSpawn (JNIEnv * env, jobject this,
				      jobjectArray cmdArray,
				      jobjectArray envArray, jobject dirFile)
{
  int fds[4];
  jobject streams[3] = { NULL, NULL, NULL };
  jobject dirString = NULL;
  char **newEnviron = NULL;
  jsize cmdArrayLen = 0;
  jsize envArrayLen = 0;
  char **strings = NULL;
  int num_strings = 0;
  char *dir = NULL;
  pid_t pid = -1;
  char errbuf[64];
  jmethodID method;
  jclass clazz;
  int i;
  int err;

  /* Check for null */
  if (cmdArray == NULL)
    goto null_pointer_exception;

  /* Invoke dirFile.getPath() */
  if (dirFile != NULL)
    {
      clazz = (*env)->FindClass (env, "java/io/File");
      if ((*env)->ExceptionOccurred (env))
	return;
      method = (*env)->GetMethodID (env,
				    clazz, "getPath", "()Ljava/lang/String;");
      if ((*env)->ExceptionOccurred (env))
	return;
      dirString = (*env)->CallObjectMethodTaint (env, dirFile, nativeParamGetTaint (4), method);
      if ((*env)->ExceptionOccurred (env))
	return;
      (*env)->DeleteLocalRef (env, clazz);
    }

  /*
   * Allocate array of C strings. We put all the C strings we need to
   * handle the command parameters, the new environment, and the new
   * directory into a single array for simplicity of (de)allocation.
   */
  cmdArrayLen = (*env)->GetArrayLength (env, cmdArray);
  if (cmdArrayLen == 0)
    goto null_pointer_exception;
  if (envArray != NULL)
    envArrayLen = (*env)->GetArrayLength (env, envArray);
  if ((strings = malloc (((cmdArrayLen + 1)
			  + (envArray != NULL ? envArrayLen + 1 : 0)
			  + (dirString !=
			     NULL ? 1 : 0)) * sizeof (*strings))) == NULL)
    {
      TARGET_NATIVE_MISC_FORMAT_STRING1 (errbuf,
					 sizeof (errbuf), "malloc: %s",
					 strerror (errno));
      goto out_of_memory;
    }

  /* Extract C strings from the various String parameters */
  for (i = 0; i < cmdArrayLen; i++)
    {
      if ((strings[num_strings++] = copy_elem (env, cmdArray, i)) == NULL)
	goto done;
    }
  strings[num_strings++] = NULL;	/* terminate array with NULL */
  if (envArray != NULL)
    {
      newEnviron = strings + num_strings;
      for (i = 0; i < envArrayLen; i++)
	{
	  if ((strings[num_strings++] = copy_elem (env, envArray, i)) == NULL)
	    goto done;
	}
      strings[num_strings++] = NULL;	/* terminate array with NULL */
    }
  if (dirString != NULL)
    {
      if ((dir = copy_string (env, dirString)) == NULL)
	goto done;
    }

  err = KFORKEXEC(strings, NULL, fds, &pid, dir);
  if (err != 0)
    {
      TARGET_NATIVE_MISC_FORMAT_STRING1 (errbuf,
					 sizeof (errbuf), "pipe: %s",
					 strerror (err));
      goto system_error;
    }

  /* Create Input/OutputStream objects around parent file descriptors */
  clazz = (*env)->FindClass (env, "gnu/java/nio/channels/FileChannelImpl");
  if ((*env)->ExceptionOccurred (env))
    goto done;
  method = (*env)->GetMethodID (env, clazz, "<init>", "(II)V");
  if ((*env)->ExceptionOccurred (env))
    goto done;
  for (i = 0; i < 3; i++)
    {
      /* Mode is WRITE (2) for in and READ (1) for out and err. */
      const int fd = fds[i];
      const int mode = (i == 0) ? 2 : 1;
      jclass sclazz;
      jmethodID smethod;

      jobject channel = (*env)->NewObject (env, clazz, method, fd, mode);
      if ((*env)->ExceptionOccurred (env))
	goto done;

      if (mode == 2)
	sclazz = (*env)->FindClass (env, "java/io/FileOutputStream");
      else
	sclazz = (*env)->FindClass (env, "java/io/FileInputStream");
      if ((*env)->ExceptionOccurred (env))
	goto done;

      smethod = (*env)->GetMethodID (env, sclazz, "<init>",
				     "(Lgnu/java/nio/channels/FileChannelImpl;)V");
      if ((*env)->ExceptionOccurred (env))
	goto done;

      streams[i] = (*env)->NewObject (env, sclazz, smethod, channel);
      if ((*env)->ExceptionOccurred (env))
	goto done;

      (*env)->DeleteLocalRef (env, sclazz);
    }
  (*env)->DeleteLocalRef (env, clazz);

  {
    char c;

    write(fds[3], &c, 1);
  }

  /* Invoke VMProcess.setProcessInfo() to update VMProcess object */
  method = (*env)->GetMethodID (env,
				(*env)->GetObjectClass (env, this),
				"setProcessInfo",
				"(Ljava/io/OutputStream;Ljava/io/InputStream;Ljava/io/InputStream;J)V");
  if ((*env)->ExceptionOccurred (env))
    goto done;
  (*env)->CallVoidMethodTaint (env, this, nativeParamGetTaint (0), method,
			  streams[0], streams[1], streams[2], (jlong) pid);
  if ((*env)->ExceptionOccurred (env))
    goto done;
  (*env)->DeleteLocalRef (env, clazz);

done:

  for (i = 0; i < 3; i++)
    {
      if (fds[i] != -1 && streams[i] == NULL)
	close(fds[i]);
    }

  /* Free C strings */
  while (num_strings > 0)
    free (strings[--num_strings]);
  free (strings);

  /* Done */
  return;

null_pointer_exception:
  clazz = (*env)->FindClass (env, "java/lang/NullPointerException");
  if ((*env)->ExceptionOccurred (env))
    goto done;
  (*env)->ThrowNew (env, clazz, NULL);
  (*env)->DeleteLocalRef (env, clazz);
  goto done;

out_of_memory:
  clazz = (*env)->FindClass (env, "java/lang/InternalError");
  if ((*env)->ExceptionOccurred (env))
    goto done;
  (*env)->ThrowNew (env, clazz, errbuf);
  (*env)->DeleteLocalRef (env, clazz);
  goto done;

system_error:
  clazz = (*env)->FindClass (env, "java/io/IOException");
  if ((*env)->ExceptionOccurred (env))
    goto done;
  (*env)->ThrowNew (env, clazz, errbuf);
  (*env)->DeleteLocalRef (env, clazz);
  goto done;
}

/*
 * private static final native boolean nativeReap()
 */
JNIEXPORT jboolean JNICALL
Java_java_lang_VMProcess_nativeReap (JNIEnv * env, jclass clazz)
{
  char ebuf[64];
  jfieldID field;
  jint status;
  pid_t pid;

  /* Try to reap a child process, but don't block */
  if ((pid = waitpid ((pid_t) - 1, &status, WNOHANG)) == 0)
    return JNI_FALSE;

  /* Check result from waitpid() */
  if (pid == (pid_t) - 1)
    {
      if (errno == ECHILD || errno == EINTR)
	return JNI_FALSE;
      TARGET_NATIVE_MISC_FORMAT_STRING2 (ebuf,
					 sizeof (ebuf), "waitpid(%ld): %s",
					 (long) pid, strerror (errno));
      clazz = (*env)->FindClass (env, "java/lang/InternalError");
      if ((*env)->ExceptionOccurred (env))
	return JNI_FALSE;
      (*env)->ThrowNew (env, clazz, ebuf);
      (*env)->DeleteLocalRef (env, clazz);
      return JNI_FALSE;
    }

  /* Get exit code; for signal termination return negative signal value XXX */
  if (WIFEXITED (status))
    status = (jint) (jbyte) WEXITSTATUS (status);
  else if (WIFSIGNALED (status))
    status = -(jint) WTERMSIG (status);
  else
    return JNI_FALSE;		/* process merely stopped; ignore */

  /* Return process pid and exit status */
  field = (*env)->GetStaticFieldID (env, clazz, "reapedPid", "J");
  if ((*env)->ExceptionOccurred (env))
    return JNI_FALSE;
  (*env)->SetStaticLongField (env, clazz, field, (jlong) pid);
  if ((*env)->ExceptionOccurred (env))
    return JNI_FALSE;
  field = (*env)->GetStaticFieldID (env, clazz, "reapedExitValue", "I");
  if ((*env)->ExceptionOccurred (env))
    return JNI_FALSE;
  (*env)->SetStaticIntField (env, clazz, field, status);
  if ((*env)->ExceptionOccurred (env))
    return JNI_FALSE;

  /* Done */
  return JNI_TRUE;
}

/*
 * private static final native void nativeKill(long)
 */
JNIEXPORT void JNICALL
Java_java_lang_VMProcess_nativeKill (JNIEnv * env, jclass clazz, jlong pid)
{
  char ebuf[64];

  if (kill ((pid_t) pid, SIGKILL) == -1)
    {
      TARGET_NATIVE_MISC_FORMAT_STRING2 (ebuf,
					 sizeof (ebuf), "kill(%ld): %s",
					 (long) pid, strerror (errno));
      clazz = (*env)->FindClass (env, "java/lang/InternalError");
      if ((*env)->ExceptionOccurred (env))
	return;
      (*env)->ThrowNew (env, clazz, ebuf);
      (*env)->DeleteLocalRef (env, clazz);
    }
}

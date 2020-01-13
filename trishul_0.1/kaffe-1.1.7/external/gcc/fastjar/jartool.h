/* $Id: jartool.h,v 1.1 2006/01/03 15:27:01 robilad Exp $

   $Log: jartool.h,v $
   Revision 1.1  2006/01/03 15:27:01  robilad
   Merged in fastjar

   2006-01-03 Dalibor Topic  <robilad@kaffe.org>

           * Makefile.am: (SUBDIRS) Added external subdir.
           (DIST_SUBDIRS) Added external subdir.

           * THIRDPARTY: Added information on fastjar. Adapted information on
           zlib.

           * configure.ac: (--with-internal-zlib) Replaced by
           (--with-system-zlib), so that kaffe now uses the merged in
           zlib by default, like gcc does.
           (--disable-fastjar) New option.
           Added fastjar subdir for configuration, and the Makefile for output.

           * kaffe/scripts/jar.in: Delegate to fastjar

           * libraries/clib/Makefile.am (SUBDIRS): Removed zlib.

           * libraries/clib/zip/Makefile.am (COND_INTERNAL_ZLIB) Replaced by
           (COND_SYSTEM_ZLIB). Adapted to use internal zlib from external/gcc
           directory.

           * libraries/javalib/Makefile.am: Regenerated.

           * libraries/javalib/vmspecific/org/kaffe/tools/jar/Jar.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFile.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileInputStream.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileOutputStream.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileReader.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileWriter.java:
           Removed.

           * libraries/clib/zlib : Moved over to external/gcc/zlib.

           * external/gcc/fastjar: New files. Merged in from gcc 4.0.2.
           Slightly adapted the build system to behave well when merged into Kaffe.

   Revision 1.4  2000/08/24 15:23:35  cory
   Set version number since I think we can let this one out.

   Revision 1.3  2000/08/23 19:42:17  cory
   Added support for more Unix platforms.  The following code has been hacked
   to work on AIX, Solaris, True 64, and HP-UX.
   Added bigendian check.  Probably works on most big and little endian platforms
   now.

   Revision 1.2  1999/12/06 03:47:20  toast
   fixing version string

   Revision 1.1.1.1  1999/12/06 03:08:24  toast
   initial checkin..



   Revision 1.6  1999/05/10 09:16:08  burnsbr
   *** empty log message ***

   Revision 1.5  1999/04/27 10:04:20  burnsbr
   configure support

   Revision 1.4  1999/04/26 02:36:15  burnsbr
   changed RDSZ to 4096 from 512

   Revision 1.3  1999/04/23 12:00:29  burnsbr
   modified zipentry struct


*/

/*
  jartool.h - generic defines, struct defs etc.
  Copyright (C) 1999  Bryan Burns
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __FASTJAR_JARTOOL_H__
#define __FASTJAR_JARTOOL_H__

#include <sys/types.h>
#include "config.h"

#define ACTION_NONE 0
#define ACTION_CREATE 1
#define ACTION_EXTRACT 2
#define ACTION_UPDATE 3
#define ACTION_LIST 4

#define TRUE 1
#define FALSE 0

/* Amount of bytes to read at a time.  You can change this to optimize for
   your system */
#define RDSZ 4096

/* Change these to match your system:
   ub1 == unsigned 1 byte word
   ub2 == unsigned 2 byte word
   ub4 == unsigned 4 byte word
*/
#if SIZEOF_CHAR == 1
typedef unsigned char ub1;
#else
typedef u_int8_t ub1;
#endif

#if SIZEOF_SHORT == 2
typedef unsigned short ub2;
#elif SIZEOF_INT == 2
typedef unsigned int ub2;
#else
typedef u_int16_t ub2;
#endif

#if SIZEOF_INT == 4
typedef unsigned int ub4;
#elif SIZEOF_LONG == 4
typedef unsigned long ub4;
#elif defined(HAVE_LONG_LONG) && SIZEOF_LONG_LONG == 4
typedef unsigned long long ub4;
#else
typedef u_int32_t ub4;
#endif

struct zipentry {
  ub2 mod_time;
  ub2 mod_date;
  ub4 crc;
  ub4 csize;
  ub4 usize;
  ub4 offset;
  ub1 compressed;
  ub2 flags;
  char *filename;
  
  struct zipentry *next_entry;
};

typedef struct zipentry zipentry;

#endif /* __FASTJAR_JARTOOL_H__ */

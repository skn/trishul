nativeexeclib_LTLIBRARIES = libjavanet.la

libjavanet_la_SOURCES = javanet.c \
			javanet.h \
			java_net_VMInetAddress.c \
			java_net_VMNetworkInterface.c \
			gnu_java_net_VMPlainDatagramSocketImpl.c \
                        gnu_java_net_VMPlainSocketImpl.c

libjavanet_la_LIBADD = $(top_builddir)/native/jni/classpath/jcl.lo

AM_LDFLAGS = @CLASSPATH_MODULE@
AM_CPPFLAGS = @CLASSPATH_INCLUDES@
AM_CFLAGS = @WARNING_CFLAGS@ @STRICT_WARNING_CFLAGS@ @ERROR_CFLAGS@

include $(top_srcdir)/doxygen-include.am

SUBDIRS = src

ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=bootstrap doc COPYING player_wrappers README.txt ChangeLog

noinst_HEADERS=\
	jni/jni.h \
	jni/linux/jni_md.h \
	jni/win32/jni_md.h \
	jni/darwin/jni_md.h

MOSTLYCLEANFILES = $(DX_CLEANFILES)

DISTCHECK_CONFIGURE_FLAGS = --enable-bdjava

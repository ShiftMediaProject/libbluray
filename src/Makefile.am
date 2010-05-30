SUBDIRS=libbdnav . examples

AM_CFLAGS = -I$(top_builddir)/src

if USING_DLOPEN
LIBBLURAY_LDFLAGS=@DLOPEN_LDFLAGS@
endif

lib_LTLIBRARIES=libbluray.la
libbluray_ladir= $(includedir)/libbluray
libbluray_la_SOURCES= libbluray/bluray.c file/dl_posix.c
libbluray_la_HEADERS= libbluray/bluray.h
libbluray_la_LIBADD = libbdnav/libbdnav.la
libbluray_la_LDFLAGS= -version-info 0:0:0 $(LIBBLURAY_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbluray.pc

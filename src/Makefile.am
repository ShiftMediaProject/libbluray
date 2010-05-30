SUBDIRS= . examples

AM_CFLAGS = -I$(top_builddir)/src

if USING_DLOPEN
LIBBLURAY_LDFLAGS=@DLOPEN_LDFLAGS@
endif

lib_LTLIBRARIES=libbluray.la
libbluray_ladir= $(includedir)/libbluray
libbluray_la_SOURCES= libbluray/bluray.c \
	file/dl_posix.c \
	util/logging.c \
	util/strutl.c \
	file/file_posix.c \
	file/dir_posix.c \
	libbluray/bdnav/navigation.c \
	libbluray/bdnav/mpls_parse.c \
	libbluray/bdnav/clpi_parse.c \
	libbluray/bdnav/mobj_parse.c \
	libbluray/bdnav/index_parse.c

libbluray_la_HEADERS= libbluray/bluray.h
libbluray_la_LDFLAGS= -version-info 0:0:0 $(LIBBLURAY_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbluray.pc

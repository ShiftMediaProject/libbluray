# library version info
# NOTE: this is not the same as the library's release number
LIB_CURRENT=0
LIB_REVISION=0
LIB_AGE=0
LIB_VERSION_INFO="$(LIB_CURRENT):$(LIB_REVISION):$(LIB_AGE)"

SUBDIRS= . examples

SET_FEATURES = -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112L -D_REENTRANT
SET_WARNINGS = @SET_WARNINGS@
SET_OPTIMIZATIONS = @SET_OPTIMIZATIONS@
SET_DEBUG_OPTS = @SET_DEBUG_OPTS@
SET_INCLUDES = -I$(top_srcdir) -Ifile -Ilibbluray/bdnav

AM_CFLAGS = -std=c99 $(SET_FEATURES) $(SET_WARNINGS) $(SET_OPTIMIZATIONS) \
	$(SET_DEBUG_OPTS) $(SET_INCLUDES)
CFLAGS=$(shell test -z "$$CFLAGS" || echo "$$CFLAGS")

if USING_DLOPEN
LIBBLURAY_LDFLAGS=@DLOPEN_LDFLAGS@
endif

lib_LTLIBRARIES=libbluray.la
libbluray_ladir= $(includedir)/libbluray
libbluray_la_SOURCES=libbluray/bluray.h \
	libbluray/bluray.c \
	libbluray/bdnav \
	libbluray/bdnav/bdparse.h \
	libbluray/bdnav/navigation.h \
	libbluray/bdnav/mobj_parse.c \
	libbluray/bdnav/hdmv_insn.h \
	libbluray/bdnav/index_parse.c \
	libbluray/bdnav/navigation.c \
	libbluray/bdnav/mpls_parse.h \
	libbluray/bdnav/clpi_parse.c \
	libbluray/bdnav/mobj_parse.h \
	libbluray/bdnav/mpls_parse.c \
	libbluray/bdnav/index_parse.h \
	libbluray/bdnav/clpi_parse.h \
	file/dir_posix.c \
	file/file.h \
	file/file_posix.c \
	file/dir.h \
	file/dl.h \
	file/dl_posix.c \
	util/attributes.h \
	util/strutl.c \
	util/strutl.h \
	util/macro.h \
	util/logging.c \
	util/bits.h \
	util/logging.h
libbluray_la_HEADERS= libbluray/bluray.h libbluray/bdnav/navigation.h \
	libbluray/bdnav/mpls_parse.h libbluray/bdnav/clpi_parse.h file/file.h
libbluray_la_LDFLAGS= -version-info $(LIB_VERSION_INFO) $(LIBBLURAY_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbluray.pc

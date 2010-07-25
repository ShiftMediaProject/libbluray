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

SOURCES_bdj = libbluray/bdj/bdj.h \
	libbluray/bdj/bdj_private.h \
	libbluray/bdj/bdj_util.h \
	libbluray/bdj/bdj_util.c \
	libbluray/bdj/bdj.c \
	libbluray/bdj/bdjo_parser.h \
	libbluray/bdj/bdjo_parser.c \
	libbluray/bdj/common.h \
	libbluray/bdj/native/org_videolan_Libbluray.h \
	libbluray/bdj/native/org_videolan_Libbluray.c

lib_LTLIBRARIES=libbluray.la
libbluray_ladir= $(includedir)/libbluray
libbluray_la_SOURCES=libbluray/bluray.h \
	libbluray/bluray.c \
	libbluray/register.h \
	libbluray/register.c \
	libbluray/bdnav \
	libbluray/bdnav/bdparse.h \
	libbluray/bdnav/navigation.h \
	libbluray/bdnav/index_parse.c \
	libbluray/bdnav/navigation.c \
	libbluray/bdnav/sound_parse.c \
	libbluray/bdnav/sound_parse.h \
	libbluray/bdnav/mpls_parse.h \
	libbluray/bdnav/clpi_parse.c \
	libbluray/bdnav/mpls_parse.c \
	libbluray/bdnav/index_parse.h \
	libbluray/bdnav/clpi_parse.h \
	libbluray/hdmv \
	libbluray/hdmv/hdmv_insn.h \
	libbluray/hdmv/hdmv_vm.h \
	libbluray/hdmv/hdmv_vm.c \
	libbluray/hdmv/mobj_parse.h \
	libbluray/hdmv/mobj_parse.c \
	libbluray/hdmv/mobj_print.c \
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
libbluray_la_HEADERS= libbluray/bluray.h file/filesystem.h
libbluray_la_LDFLAGS= -version-info $(LIB_VERSION_INFO) -ldl

if USING_BDJAVA
libbluray_la_SOURCES+=$(SOURCES_bdj)
AM_CFLAGS+=@BDJAVA_CFLAGS@

all-local:
	ant -f libbluray/bdj/build.xml
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbluray.pc

LIB_VERSION_INFO="@LT_VERSION_INFO@"

SUBDIRS= . examples

SET_FEATURES = -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200809L -D_REENTRANT
SET_WARNINGS = @SET_WARNINGS@
SET_OPTIMIZATIONS = @SET_OPTIMIZATIONS@
SET_DEBUG_OPTS = @SET_DEBUG_OPTS@
SET_INCLUDES = -I$(top_srcdir) -Ifile -Ilibbluray/bdnav

AM_CFLAGS = -std=c99 $(SET_FEATURES) $(SET_WARNINGS) $(SET_OPTIMIZATIONS) \
        $(SET_DEBUG_OPTS) $(SET_INCLUDES) $(LIBXML2_CFLAGS)
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

EXTRA_DIST = libbluray/bdj/build.xml libbluray/bdj/java

lib_LTLIBRARIES=libbluray.la
libbluray_ladir= $(includedir)/libbluray
libbluray_la_SOURCES=libbluray/bluray.h \
	libbluray/bluray.c \
	libbluray/register.h \
	libbluray/register.c \
	libbluray/keys.h \
	libbluray/bdnav \
	libbluray/bdnav/bdparse.h \
	libbluray/bdnav/uo_mask_table.h \
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
	libbluray/bdnav/meta_data.h \
	libbluray/bdnav/meta_parse.c \
	libbluray/bdnav/meta_parse.h \
	libbluray/decoders \
	libbluray/decoders/pes_buffer.h \
	libbluray/decoders/pes_buffer.c \
	libbluray/decoders/m2ts_demux.h \
	libbluray/decoders/m2ts_demux.c \
	libbluray/decoders/overlay.h \
	libbluray/decoders/pg.h \
	libbluray/decoders/pg_decode.h \
	libbluray/decoders/pg_decode.c \
	libbluray/decoders/ig.h \
	libbluray/decoders/ig_decode.h \
	libbluray/decoders/ig_decode.c \
	libbluray/decoders/graphics_processor.h \
	libbluray/decoders/graphics_processor.c \
	libbluray/decoders/graphics_controller.h \
	libbluray/decoders/graphics_controller.c \
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
	file/dl.h \
	file/dl_posix.c \
	file/filesystem.h \
	file/filesystem.c \
	util/attributes.h \
	util/strutl.c \
	util/strutl.h \
	util/macro.h \
	util/mutex.h \
	util/logging.c \
	util/log_control.h \
	util/bits.h \
	util/logging.h
libbluray_la_HEADERS= libbluray/bluray.h file/filesystem.h util/log_control.h libbluray/keys.h libbluray/decoders/overlay.h libbluray/bdnav/meta_data.h libbluray/bdnav/clpi_data.h libbluray/bluray-version.h
libbluray_la_LDFLAGS= -version-info $(LIB_VERSION_INFO)
libbluray_la_LIBADD= $(LIBXML2_LIBS) $(DLOPEN_LIBS)

if USING_BDJAVA
libbluray_la_SOURCES+=$(SOURCES_bdj)
AM_CFLAGS+=@BDJAVA_CFLAGS@

all-local:
	ant -f $(top_srcdir)/src/libbluray/bdj/build.xml
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbluray.pc

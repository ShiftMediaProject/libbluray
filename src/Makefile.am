SET_FEATURES = -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112L -D_REENTRANT
SET_INCLUDES = -I$(top_srcdir) -Ifile -Ilibbluray -Ilibbluray/bdnav

AM_CFLAGS = -std=c99 $(SET_FEATURES) $(SET_INCLUDES) $(LIBXML2_CFLAGS) $(FT2_CFLAGS)

EXTRA_DIST = \
       libbluray/bdj/build.xml \
       libbluray/bdj/java \
       libbluray/bdj/java-j2me \
       libbluray/bdj/java-j2se

lib_LTLIBRARIES=libbluray.la
libbluray_la_SOURCES = \
	libbluray/bluray.h \
	libbluray/bluray.c \
	libbluray/bluray_internal.h \
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
	libbluray/bdnav/extdata_parse.h \
	libbluray/bdnav/extdata_parse.c \
	libbluray/bdnav/bdid_parse.h \
	libbluray/bdnav/bdid_parse.c \
	libbluray/decoders \
	libbluray/decoders/pes_buffer.h \
	libbluray/decoders/pes_buffer.c \
	libbluray/decoders/m2ts_demux.h \
	libbluray/decoders/m2ts_demux.c \
	libbluray/decoders/m2ts_filter.h \
	libbluray/decoders/m2ts_filter.c \
	libbluray/decoders/overlay.h \
	libbluray/decoders/pg.h \
	libbluray/decoders/pg_decode.h \
	libbluray/decoders/pg_decode.c \
	libbluray/decoders/ig.h \
	libbluray/decoders/ig_decode.h \
	libbluray/decoders/ig_decode.c \
	libbluray/decoders/rle.h \
	libbluray/decoders/rle.c \
	libbluray/decoders/textst.h \
	libbluray/decoders/textst_decode.h \
	libbluray/decoders/textst_decode.c \
	libbluray/decoders/textst_render.h \
	libbluray/decoders/textst_render.c \
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
	file/file.h \
	file/file_posix.c \
	file/dirs.h \
	file/dl.h \
	file/filesystem.h \
	file/filesystem.c \
	file/libaacs.h \
	file/libaacs.c \
	file/libbdplus.h \
	file/libbdplus.c \
	util/attributes.h \
	util/refcnt.h \
	util/refcnt.c \
	util/strutl.c \
	util/strutl.h \
	util/macro.h \
	util/mutex.h \
	util/time.h \
	util/logging.c \
	util/log_control.h \
	util/bits.h \
	util/bits.c \
	util/logging.h

if HAVE_DARWIN
libbluray_la_SOURCES+= \
	file/dir_posix.c \
	file/dl_posix.c \
	file/dirs_darwin.c
else
if HAVE_WIN32
libbluray_la_SOURCES+= \
	file/dir_win32.c \
	file/dl_win32.c \
	file/dirs_win32.c
else
libbluray_la_SOURCES+= \
	file/dir_posix.c \
	file/dl_posix.c \
	file/dirs_xdg.c
endif
endif


libbluray_la_LDFLAGS= -version-info $(LT_VERSION_INFO)
libbluray_la_LIBADD= $(LIBXML2_LIBS) $(FT2_LIBS)

pkginclude_HEADERS = \
	file/filesystem.h \
	libbluray/bdnav/clpi_data.h \
	libbluray/bdnav/meta_data.h \
	libbluray/bluray-version.h \
	libbluray/bluray.h \
	libbluray/decoders/overlay.h \
	libbluray/keys.h \
	util/log_control.h

if USING_BDJAVA
jardir=$(libdir)/libbluray/
jar_DATA=$(top_builddir)/src/.libs/libbluray.jar

$(top_builddir)/src/.libs/libbluray.jar: all-local

libbluray_la_SOURCES += \
	libbluray/bdj/bdj.h \
	libbluray/bdj/bdj_private.h \
	libbluray/bdj/bdj_util.h \
	libbluray/bdj/bdj_util.c \
	libbluray/bdj/bdj.c \
	libbluray/bdj/bdjo_parser.h \
	libbluray/bdj/bdjo_parser.c \
	libbluray/bdj/common.h \
	libbluray/bdj/native/java_awt_BDGraphics.h \
	libbluray/bdj/native/java_awt_BDGraphics.c \
	libbluray/bdj/native/java_awt_BDFontMetrics.h \
	libbluray/bdj/native/java_awt_BDFontMetrics.c \
	libbluray/bdj/native/org_videolan_Libbluray.h \
	libbluray/bdj/native/org_videolan_Libbluray.c \
	libbluray/bdj/native/org_videolan_Logger.h \
	libbluray/bdj/native/org_videolan_Logger.c \
	libbluray/bdj/native/register_native.h \
	libbluray/bdj/native/register_native.c

AM_CFLAGS += $(BDJAVA_CFLAGS)

all-local:
	ant -f $(top_srcdir)/src/libbluray/bdj/build.xml \
	    -Dbuild='$(abs_builddir)/libbluray/bdj/build' \
	    -Ddist='$(abs_builddir)/.libs' \
	    -Dsrc_awt=:java-$(BDJ_TYPE) \
	    -Dbootclasspath="$(BDJ_BOOTCLASSPATH)"

clean-local:
	ant -f $(top_srcdir)/src/libbluray/bdj/build.xml \
	    -Dbuild='$(abs_builddir)/libbluray/bdj/build' \
	    -Ddist='$(abs_builddir)/.libs' \
	    clean
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbluray.pc


#
# examples
#

#SET_FEATURES = -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112L -D_REENTRANT
#SET_INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/src/file -I$(top_srcdir)/src/libbluray/bdnav

if USING_EXAMPLES

#AM_CFLAGS = -std=c99 $(SET_FEATURES) $(SET_INCLUDES)

noinst_PROGRAMS = \
	bdsplice \
	clpi_dump \
	hdmv_test \
	index_dump \
	libbluray_test \
	list_titles \
	mobj_dump \
	mpls_dump \
	sound_dump

if USING_BDJAVA
noinst_PROGRAMS += \
	bdj_test
endif

bin_PROGRAMS = \
	bd_info

bdsplice_SOURCES = examples/bdsplice.c
bdsplice_LDADD = libbluray.la

mpls_dump_CFLAGS = $(AM_CFLAGS)
mpls_dump_SOURCES = \
	examples/mpls_dump.c \
	examples/util.c \
	examples/util.h \
	file/file_posix.c \
	libbluray/bdnav/clpi_parse.c \
	libbluray/bdnav/extdata_parse.c \
	libbluray/bdnav/mpls_parse.c \
	util/bits.c \
	util/logging.c \
	util/strutl.c
if HAVE_WIN32
mpls_dump_SOURCES += \
	file/dir_win32.c
else
mpls_dump_SOURCES += \
	file/dir_posix.c
endif

clpi_dump_CFLAGS = $(AM_CFLAGS)
clpi_dump_SOURCES = \
	examples/clpi_dump.c \
	examples/util.c \
	examples/util.h
clpi_dump_LDADD = libbluray.la

index_dump_CFLAGS = $(AM_CFLAGS)
index_dump_SOURCES = \
	examples/index_dump.c \
	file/file_posix.c \
	libbluray/bdnav/index_parse.c \
	util/bits.c \
	util/logging.c

mobj_dump_CFLAGS = $(AM_CFLAGS)
mobj_dump_SOURCES = \
	examples/mobj_dump.c \
	file/file_posix.c \
	libbluray/hdmv/mobj_parse.c \
	libbluray/hdmv/mobj_print.c \
	util/bits.c \
	util/logging.c

sound_dump_SOURCES = examples/sound_dump.c
sound_dump_LDADD = libbluray.la

libbluray_test_SOURCES = examples/libbluray_test.c
libbluray_test_LDADD = libbluray.la

hdmv_test_SOURCES = examples/hdmv_test.c
hdmv_test_LDADD = libbluray.la

list_titles_SOURCES = examples/list_titles.c
list_titles_LDADD = libbluray.la

bd_info_SOURCES = examples/bd_info.c
bd_info_LDADD = libbluray.la

bdj_test_SOURCES = examples/bdj_test.c
bdj_test_LDADD = libbluray.la

endif

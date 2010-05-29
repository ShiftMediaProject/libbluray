SUBDIRS=libbdnav . examples

AM_CFLAGS = -I$(top_builddir)/src

lib_LTLIBRARIES=

if HAVE_LIBAACS_SRC
LIBAACS_LIBADD = libaacs.la
lib_LTLIBRARIES += $(LIBAACS_LIBADD)
libaacs_la_SOURCES=libaacs/aacs.c libaacs/crypto.c libaacs/mkb.c libaacs/mmc.c file/configfile.c file/file_posix.c util/logging.c
libaacs_la_CFLAGS=@OPENSSL_INCLUDES@
libaacs_la_LDFLAGS=@OPENSSL_LDFLAGS@
endif

if HAVE_LIBBDPLUS_SRC
LIBBDPLUS_LIBADD = libbdplus.la
lib_LTLIBRARIES += $(LIBBDPLUS_LIBADD)
libbdplus_la_SOURCES=libbdplus/bdplus.c libbdplus/bdsvm/segment.c libbdplus/bdsvm/trap.c \
	libbdplus/bdsvm/dlx.c libbdplus/bdsvm/interface.c libbdplus/bdsvm/trap_helper.c \
	libbdplus/bdsvm/diff.c libbdplus/bdsvm/slot.c libbdplus/bdsvm/ram.c libbdplus/internal.c \
	util/logging.c
libbdplus_la_CFLAGS = -I$(top_builddir)/src/libbdplus \
	-I$(top_builddir)/src/libbdplus/bdsvm @OPENSSL_INCLUDES@
libbdplus_la_LDFLAGS = @OPENSSL_LDFLAGS@
endif

if USING_DLOPEN
LIBBLURAY_LDFLAGS=@DLOPEN_LDFLAGS@
else
LIBBLURAY_LIBADD=$(LIBAACS_LIBADD) $(LIBBDPLUS_LIBADD)
endif

lib_LTLIBRARIES+=libbluray.la
libbluray_la_SOURCES= bluray.c bluray.h file/dl_posix.c
libbluray_la_LIBADD = libbdnav/libbdnav.la $(LIBBLURAY_LIBADD)
libbluray_la_LDFLAGS= -version-info 0:0:0 $(LIBBLURAY_LDFLAGS)

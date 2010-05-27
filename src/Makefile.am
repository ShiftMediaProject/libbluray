SUBDIRS=libbdplus libbdnav . examples

AM_CFLAGS = -I$(top_builddir)/src -I$(top_builddir)/src/libbdplus

lib_LTLIBRARIES=

if HAVE_LIBAACS_SRC
LIBAACS_LIBADD = libaacs.la
lib_LTLIBRARIES += $(LIBAACS_LIBADD)
libaacs_la_SOURCES=libaacs/aacs.c libaacs/crypto.c libaacs/mkb.c libaacs/mmc.c file/configfile.c file/file_posix.c util/logging.c
endif

lib_LTLIBRARIES+=libbluray.la
libbluray_la_SOURCES= bluray.c bluray.h file/dl_posix.c
libbluray_la_LIBADD = $(LIBAACS_LIBADD) libbdplus/libbdplus.la libbdnav/libbdnav.la
libbluray_la_LDFLAGS= -version-info 0:0:0

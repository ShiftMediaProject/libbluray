include $(top_srcdir)/doxygen-include.am

SUBDIRS = src
if USING_EXAMPLES
SUBDIRS += src/examples
endif

ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=bootstrap doc COPYING player_wrappers README.txt ChangeLog

MOSTLYCLEANFILES = $(DX_CLEANFILES)

DISTCHECK_CONFIGURE_FLAGS = --enable-bdjava

include $(top_srcdir)/doxygen-include.am

SUBDIRS=src

ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=bootstrap doc COPYING player_wrappers README.txt ChangeLog

MOSTLYCLEANFILES = $(DX_CLEANFILES)

AUTOMAKE_OPTIONS = \
	dist-bzip2 \
	no-dist-gzip

DISTCHECK_CONFIGURE_FLAGS = --enable-bdjava

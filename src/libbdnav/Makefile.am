## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I $(top_builddir)/src

lib_LTLIBRARIES = libbdnav.la

libbdnav_la_SOURCES = \
	../util/logging.c \
	../util/strutl.c \
	../file/file_posix.c \
	../file/dir_posix.c \
	navigation.c \
	navigation.h \
	mpls_parse.c \
	clpi_parse.c \
	mpls_parse.h \
	clpi_parse.h \
	mobj_parse.c \
	mobj_parse.h \
	index_parse.c \
	index_parse.h

noinst_HEADERS = mpls_parse.h clpi_parse.h bdparse.h index_parse.h mobj_parse.h


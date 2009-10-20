## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I $(top_builddir)/..

noinst_LIBRARIES = libbdnav.a

libbdnav_a_SOURCES = \
	../util/logging.c \
	../util/strutl.c \
	../file/file_posix.c \
	../file/dir_posix.c \
	navigation.c \
	navigation.h \
	mpls_parse.c \
	clpi_parse.c \
	mpls_parse.h \
	clpi_parse.h

noinst_HEADERS = mpls_parse.h clpi_parse.h bdparse.h


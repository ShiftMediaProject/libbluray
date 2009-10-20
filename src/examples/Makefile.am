
AM_CPPFLAGS = -I $(top_builddir)/..

bin_PROGRAMS=bdplus_test bdsplice mpls_dump clpi_dump

bdplus_test_SOURCE=bdplus_test.c

bdplus_test_LDADD=$(top_builddir)/../libbdplus/libbdplus.a
bdplus_test_CPPFLAGS = -I$(top_builddir)/../libbdplus
 
BDNAV_LIB = $(top_builddir)/../libbdnav/libbdnav.a

bdsplice_SOURCES = \
	bdsplice.c

bdsplice_LDADD = $(BDNAV_LIB)

mpls_dump_SOURCES = \
	mpls_dump.c util.c

mpls_dump_LDADD = $(BDNAV_LIB)

clpi_dump_SOURCES = \
	clpi_dump.c util.c

clpi_dump_LDADD = $(BDNAV_LIB)


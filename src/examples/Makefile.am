
# Set this for all builds, or use $bin_CPPFLAGS for individual
AM_CPPFLAGS = -I$(top_builddir)/src

bin_PROGRAMS=bdplus_test bdsplice mpls_dump clpi_dump index_dump libbluray_test

bdplus_test_SOURCE=bdplus_test.c

bdplus_test_LDADD=$(top_builddir)/src/libbluray.la
bdplus_test_CPPFLAGS = -I$(top_builddir)/src/libbdplus

BDNAV_LIB = $(top_builddir)/src/libbdnav/libbdnav.la

bdsplice_SOURCES = \
	bdsplice.c

bdsplice_LDADD = $(BDNAV_LIB)

mpls_dump_SOURCES = \
	mpls_dump.c util.c

mpls_dump_LDADD = $(BDNAV_LIB)

clpi_dump_SOURCES = \
	clpi_dump.c util.c

clpi_dump_LDADD = $(BDNAV_LIB)

index_dump_SOURCES = \
	index_dump.c

index_dump_LDADD = $(BDNAV_LIB)

libbluray_test_SOURCES = \
	libbluray_test.c

libbluray_test_LDADD = $(top_builddir)/src/libbluray.la

# Set this for all builds, or use $bin_CFLAGS for individual
AM_CFLAGS = -I$(top_builddir)/src

noinst_PROGRAMS=bdsplice mpls_dump clpi_dump index_dump mobj_dump \
    libbluray_test list_titles

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

mobj_dump_SOURCES = \
	mobj_dump.c

mobj_dump_LDADD = $(BDNAV_LIB)

libbluray_test_SOURCES = \
	libbluray_test.c

libbluray_test_LDADD = $(top_builddir)/src/libbluray.la

list_titles_SOURCES = \
	list_titles.c

list_titles_LDADD = $(top_builddir)/src/libbluray.la

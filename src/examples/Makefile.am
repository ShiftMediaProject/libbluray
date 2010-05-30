# Set this for all builds, or use $bin_CFLAGS for individual
AM_CFLAGS = -I$(top_builddir)/src

noinst_PROGRAMS=bdsplice mpls_dump clpi_dump index_dump mobj_dump \
    libbluray_test list_titles

BLURAY_LIB = $(top_builddir)/src/libbluray.la

bdsplice_SOURCES = \
	bdsplice.c

bdsplice_LDADD = $(BLURAY_LIB)

mpls_dump_SOURCES = \
	mpls_dump.c util.c

mpls_dump_LDADD = $(BLURAY_LIB)

clpi_dump_SOURCES = \
	clpi_dump.c util.c

clpi_dump_LDADD = $(BLURAY_LIB)

index_dump_SOURCES = \
	index_dump.c

index_dump_LDADD = $(BLURAY_LIB)

mobj_dump_SOURCES = \
	mobj_dump.c

mobj_dump_LDADD = $(BLURAY_LIB)

libbluray_test_SOURCES = \
	libbluray_test.c

libbluray_test_LDADD = $(BLURAY_LIB)

list_titles_SOURCES = \
	list_titles.c

list_titles_LDADD = $(BLURAY_LIB)

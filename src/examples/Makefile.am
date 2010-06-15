SET_FEATURES = -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112L -D_REENTRANT
SET_WARNINGS = @SET_WARNINGS@
SET_OPTIMIZATIONS = @SET_OPTIMIZATIONS@
SET_DEBUG_OPTS = @SET_DEBUG_OPTS@
SET_INCLUDES = -I$(top_srcdir) -I.. -I../file -I../libbluray/bdnav

AM_CFLAGS = -std=c99 $(SET_FEATURES) $(SET_WARNINGS) $(SET_OPTIMIZATIONS) \
	$(SET_DEBUG_OPTS) $(SET_INCLUDES)
CFLAGS=$(shell test -z "$$CFLAGS" || echo "$$CFLAGS")

noinst_PROGRAMS=bdsplice mpls_dump clpi_dump index_dump mobj_dump sound_dump \
    libbluray_test list_titles

BLURAY_LIB = $(top_builddir)/src/libbluray.la

bdsplice_SOURCES = \
	bdsplice.c

bdsplice_LDADD = $(BLURAY_LIB)

mpls_dump_SOURCES = \
	mpls_dump.c util.c util.h

mpls_dump_LDADD = $(BLURAY_LIB)

clpi_dump_SOURCES = \
	clpi_dump.c util.c util.h

clpi_dump_LDADD = $(BLURAY_LIB)

sound_dump_SOURCES = \
	sound_dump.c

sound_dump_LDADD = $(BLURAY_LIB)

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

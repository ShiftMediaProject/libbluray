#
# settings
#

DESTDIR        ?= /
INSTALL         = install

#
# compiler options
#

CFLAGS      += -O2 -fPIC
CFLAGS      += -Wall
CFLAGS      += -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS      += $(shell pkg-config libxine --cflags)
CFLAGS      += $(shell pkg-config libbluray --cflags)

#
# linker options
#

LIBS_XINE    = $(shell pkg-config libxine --libs)
LIBS_BD      = $(shell pkg-config libbluray --libs)

LDFLAGS     += -shared -fvisibility=hidden

#
# targets
#

XINEPLUGINDIR  = $(shell pkg-config libxine --variable=plugindir)

XINEINPUTPLUGIN= xineplug_inp_bluray.so

#
# object files
#

OBJS_XINEINPUTPLUGIN = input_bluray.o

#
# rules
#

all: $(XINEINPUTPLUGIN)

$(XINEINPUTPLUGIN): $(OBJS_XINEINPUTPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) $(LIBS_BD) -o $@ $(OBJS_XINEINPUTPLUGIN)

#
# targets
#

clean:
	@rm -rf *.o *.so *~

install: all uninstall
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
	@$(INSTALL) -D -m 0644 $(XINEINPUTPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)

uninstall:
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)

CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk+-3.0)
LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

TARGET = simpleshot
SRC = simpleshot.c screenshot-app.c screenshot-menu.c
GTK_BLD_SRC = resources.c

OBJS = $(GTK_BLD_SRC:.c=.o) $(SRC:.c=.o)

all: $(TARGET)

$(GTK_BLD_SRC): simpleshot.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies simpleshot.gresource.xml)
	$(GLIB_COMPILE_RESOURCES) $< --target=$@ --sourcedir=. --generate-source

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(GTK_BLD_SRC)
	rm -f $(OBJS)
	rm -f $(TARGET)

CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk+-3.0)
LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0) -lslopy
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
GLIB_COMPILE_SCHEMAS = $(shell $(PKGCONFIG) --variable=glib_compile_schemas gio-2.0)

TARGET = simpleshot
SRC = simpleshot.c screenshot-app.c screenshot-menu.c screenshot-logic.c
G_RESOURCE = simpleshot.gresource.xml
G_SCHEMA = com.tomaszal.simpleshot.gschema.valid

OBJS = resources.o $(SRC:.c=.o)

all: $(TARGET)

$(G_SCHEMA): $(G_SCHEMA:.valid=.xml)
	$(GLIB_COMPILE_SCHEMAS) --strict --dry-run --schema-file=$< && mkdir -p $(@D) && touch $@

gschemas.compiled: $(G_SCHEMA)
	$(GLIB_COMPILE_SCHEMAS) .

resources.c: $(G_RESOURCE) $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies $(G_RESOURCE))
	$(GLIB_COMPILE_RESOURCES) $(G_RESOURCE) --target=$@ --sourcedir=. --generate-source

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS) gschemas.compiled
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f resources.c
	rm -f gschemas.compiled
	rm -f $(G_SCHEMA)
	rm -f $(OBJS)
	rm -f $(TARGET)

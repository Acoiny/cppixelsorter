MAKEFLAGS=-j


BINARY := cppixelsorter

INSTALL_DIR=~/.local/bin/

# arguments for the `make run` command
RUN_ARGS := -g -v

# prefix with debug, to make running as easy as typing d-TAB
BINARY_DEBUG := debug_$(BINARY)

# flags only for debugging
DEBUG_FLAGS = -g3 -O0 -ggdb

# flags only for release
RELEASE_FLAGS := -O3

# CFLAGS = -Wall -Wextra -Werror -std=gnu++23 -march=native -Wno-unused-parameter
CFLAGS = -std=gnu++23 -fopenmp -Wall -Wno-unused-parameter # -finline-functions -fno-exceptions

# CFLAGS += $(shell pkg-config --cflags --libs dbus-1)

CFLAGS += -I hdr

SRCDIR := src

OUTDIR := bin

SOURCE_FILES = $(wildcard src/*.cpp)
SOURCE_FILES += $(wildcard src/*/*.cpp)
SOURCE_FILES += $(wildcard src/*/*/*.cpp)

OBJECT_FILES = $(patsubst %.cpp,$(OUTDIR)/release/%.o,$(shell basename -a $(SOURCE_FILES)))

DOBJECT_FILES = $(patsubst %.cpp,$(OUTDIR)/debug/%.o,$(shell basename -a $(SOURCE_FILES)))

ASSEMBLY_FILES = $(DOBJECT_FILES:$(OUTDIR)/debug/%.o=$(OUTDIR)/assembly/%.s)

$(shell mkdir -p $(OUTDIR)/release)
$(shell mkdir -p $(OUTDIR)/debug)

# LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
LDFLAGS +=-lSDL3 -lSDL3_ttf
# LDFLAGS=-lraylib

.PHONY := all debug release clean

all: debug

install: release
	mv $(BINARY) $(INSTALL_DIR)

release: $(BINARY)
release: CFLAGS += $(RELEASE_FLAGS)

debug: $(BINARY_DEBUG)
debug: CFLAGS += $(DEBUG_FLAGS)

embeds:
	xxd -i assets/icon.png > src/embed_data.cpp
	xxd -i assets/Archivo-Regular.ttf >> src/embed_data.cpp
	# generating the huebar on the fly :)
	python scripts/huebar.py | xxd -i -n assets_huebar >> src/embed_data.cpp

# rule for building and running debug build
run: $(BINARY_DEBUG)
	./$(BINARY_DEBUG) $(RUN_ARGS)

$(BINARY): $(OBJECT_FILES)
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BINARY_DEBUG): $(DOBJECT_FILES)
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

## Release objects
# rule for all object files directly in the src folder
$(OUTDIR)/release/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# rule for all subfolders
$(OUTDIR)/release/%.o: $(SRCDIR)/*/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUTDIR)/release/%.o: $(SRCDIR)/*/*/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
## --

## Debug objects
$(OUTDIR)/debug/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUTDIR)/debug/%.o: $(SRCDIR)/*/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUTDIR)/debug/%.o: $(SRCDIR)/*/*/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
## --

# rule to dump assembly
assembly: $(ASSEMBLY_FILES)
assembly: CFLAGS += $(DEBUG_FLAGS)

$(ASSEMBLY_FILES): $(DOBJECT_FILES)
	$(shell mkdir -p $(OUTDIR)/assembly)
	objdump -d -S $< > $@

clean:
	rm -f $(OBJECT_FILES) $(DOBJECT_FILES) $(ASSEMBLY_FILES) $(BINARY) $(BINARY_DEBUG)

rebuild:
	$(DEL)
	make all

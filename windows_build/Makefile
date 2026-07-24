
PWD := $(shell pwd)
OUTDIR := $(PWD)/build
BINARY := cppixelsorter.exe

REL_DIR := $(PWD)/output

output: release
	$(shell mkdir $(REL_DIR))
	$(shell mv ../$(BINARY) "$(REL_DIR)/")
	$(shell cp /usr/x86_64-w64-mingw32/bin/*.dll "$(REL_DIR)/")

release:
	make -C ../sui release
	make -C .. release LDFLAGS="-lstdc++exp -lSDL3 -lSDL3_ttf" BINARY=$(BINARY) OUTDIR=$(OUTDIR)

clean:
	make -C ../sui clean
	make -C .. clean BINARY=cppixelsorter.exe OUTDIR=$(OUTDIR)
	rm -rf $(OUTDIR)
	rm -rf $(REL_DIR)

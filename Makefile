CC=gcc
CFLAGS=-I$(IDIR) -Wall -Wextra -Wpedantic
IDIR=include
SRCDIR=src

ODIR=obj
LIBS=

_DEPS = itay_shell.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o itay_shell.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: itay_shell


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) build
	$(CC) -c -o $@ $< $(CFLAGS)


itay_shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


build:
	mkdir -p $(ODIR)


clean:
	rm -rf $(ODIR) *.swp core $(IDIR)/*.swp


.PHONY: build clean

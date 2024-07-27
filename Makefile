CC=gcc
CFLAGS=-I$(IDIR) -Wall -Wextra -Wpedantic
IDIR=include
SRCDIR=src

PROGRAM=itay_shell
ODIR=obj
LIBS=

_DEPS = itay_shell.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o itay_shell.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: $(PROGRAM)


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) build
	$(CC) -c -o $@ $< $(CFLAGS)


$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


build:
	mkdir -p $(ODIR)


clean:
	rm -rf $(PROGRAM) $(ODIR) *.swp core $(IDIR)/*.swp compile_commands.json


.PHONY: build clean

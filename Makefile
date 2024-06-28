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


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


itay_shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *.swp core $(IDIR)/*.swp

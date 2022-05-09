CFLAGS += -std=c99 -mfpmath=sse -Ofast -flto -march=native -funroll-loops -Wall -Wextra -pedantic -Wold-style-declaration
CFLAGS += -Wmissing-prototypes -Wno-unused-parameter
PREFIX ?= $(HOME)
BINDIR ?= $(PREFIX)/.local/bin
CC     ?= gcc

all: zwm

config.h:
	cp config.def.h config.h

zwm: zwm.c config.h Makefile
	$(CC) $(CFLAGS) -o $@ $< -lX11 -lXinerama -DXINERAMA $(LDFLAGS)

install: all
	install -Dm755 zwm $(DESTDIR)$(BINDIR)/zwm

clean:
	rm -f zwm *.o

.PHONY: all install clean

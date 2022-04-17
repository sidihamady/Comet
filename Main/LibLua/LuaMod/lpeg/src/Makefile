BITS := $(shell getconf LONG_BIT)
ifeq "$(BITS)" "64"
    DEVC_OUTDIR = ../../../../../output/Linux64
else
    DEVC_OUTDIR = ../../../../../output/Linux32
endif
DEVC_BINDIR = $(DEVC_OUTDIR)/bin
DEVC_LIBDIR = $(DEVC_OUTDIR)/lib

LIBNAME = lpeg.so

COPT = -O2

LDFLAGS_RELEASE = -L$(DEVC_OUTDIR)/bin -lluacore

CWARNS = -Wall

CFLAGS = $(CWARNS) $(COPT) -std=c99 -fPIC -DUSE_LUAJIT -I../include
CC = gcc

FILES = lpvm.o lpcap.o lptree.o lpcode.o lpprint.o

all:
	make $(LIBNAME) "DLLFLAGS = -shared -fPIC"

$(LIBNAME): $(FILES)
	env $(CC) $(DLLFLAGS) $(FILES) -o $(LIBNAME) $(LDFLAGS_RELEASE)

$(FILES): Makefile

clean:
	rm -f $(FILES) $(LIBNAME)

lpcap.o: lpcap.c
lpcode.o: lpcode.c
lpprint.o: lpprint.c
lptree.o: lptree.c
lpvm.o: lpvm.c

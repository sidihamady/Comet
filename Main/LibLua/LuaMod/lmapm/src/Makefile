BITS := $(shell getconf LONG_BIT)
ifeq "$(BITS)" "64"
    DEVC_OUTDIR = ../../../../../output/Linux64
else
    DEVC_OUTDIR = ../../../../../output/Linux32
endif
DEVC_BINDIR = $(DEVC_OUTDIR)/bin
DEVC_LIBDIR = $(DEVC_OUTDIR)/lib

LIBNAME = lmapm.so

COPT = -O2

LDFLAGS_RELEASE = -L $(DEVC_OUTDIR)/bin -lluacore

CWARNS = -Wall

CFLAGS = $(CWARNS) $(COPT) -std=c99 -fPIC -DUSE_LUAJIT -I../include
CC = gcc

FILES = lmapm.o mapm_add.o mapm_cpi.o mapm_div.o mapm_exp.o mapm_fam.o mapm_fft.o mapm_flr.o mapm_fpf.o mapm_gcd.o mapm_lg2.o mapm_lg3.o mapm_lg4.o mapm_log.o mapm_mul.o mapm_pow.o mapm_rcp.o mapm_rnd.o mapm_set.o mapm_sin.o mapm5sin.o mapmasin.o mapmasn0.o mapmcbrt.o mapmcnst.o mapmfact.o mapmfmul.o mapmgues.o mapmhasn.o mapmhsin.o mapmipwr.o mapmistr.o mapmpwr2.o mapmrsin.o mapmsqrt.o mapmstck.o mapmutil.o mapmutl1.o mapmutl2.o

all:
	make $(LIBNAME) "DLLFLAGS = -shared -fPIC"

$(LIBNAME): $(FILES)
	env $(CC) $(DLLFLAGS) $(FILES) -o $(LIBNAME) $(LDFLAGS_RELEASE)

$(FILES): Makefile

clean:
	rm -f $(FILES) $(LIBNAME)

lmapm.o: lmapm.c
mapm_add.o: mapm_add.c
mapm_cpi.o: mapm_cpi.c
mapm_div.o: mapm_div.c
mapm_exp.o: mapm_exp.c
mapm_fam.o: mapm_fam.c
mapm_fft.o: mapm_fft.c
mapm_flr.o: mapm_flr.c
mapm_fpf.o: mapm_fpf.c
mapm_gcd.o: mapm_gcd.c
mapm_lg2.o: mapm_lg2.c
mapm_lg3.o: mapm_lg3.c
mapm_lg4.o: mapm_lg4.c
mapm_log.o: mapm_log.c
mapm_mul.o: mapm_mul.c
mapm_pow.o: mapm_pow.c
mapm_rcp.o: mapm_rcp.c
mapm_rnd.o: mapm_rnd.c
mapm_set.o: mapm_set.c
mapm_sin.o: mapm_sin.c
mapm5sin.o: mapm5sin.c
mapmasin.o: mapmasin.c
mapmasn0.o: mapmasn0.c
mapmcbrt.o: mapmcbrt.c
mapmcnst.o: mapmcnst.c
mapmfact.o: mapmfact.c
mapmfmul.o: mapmfmul.c
mapmgues.o: mapmgues.c
mapmhasn.o: mapmhasn.c
mapmhsin.o: mapmhsin.c
mapmipwr.o: mapmipwr.c
mapmistr.o: mapmistr.c
mapmpwr2.o: mapmpwr2.c
mapmrsin.o: mapmrsin.c
mapmsqrt.o: mapmsqrt.c
mapmstck.o: mapmstck.c
mapmutil.o: mapmutil.c
mapmutl1.o: mapmutl1.c
mapmutl2.o: mapmutl2.c

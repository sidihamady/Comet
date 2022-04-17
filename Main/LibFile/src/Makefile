# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

BITS := $(shell getconf LONG_BIT)
ifeq "$(BITS)" "64"
    DEVC_OUTDIR = ../../../output/Linux64
else
    DEVC_OUTDIR = ../../../output/Linux32
endif
DEVC_BINDIR = $(DEVC_OUTDIR)/bin
DEVC_LIBDIR = $(DEVC_OUTDIR)/lib

WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall -fexceptions
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_RELEASE = -I../include $(INC)
CFLAGS_RELEASE =  -O2 -std=c++0x -fPIC $(CFLAGS)
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = 
LDFLAGS_RELEASE = $(LDFLAGS)
OBJDIR_RELEASE = $(DEVC_OUTDIR)/obj/LibFile
DEP_RELEASE = 
OUT_RELEASE = $(DEVC_OUTDIR)/bin/libfile.so

OBJ_RELEASE = $(OBJDIR_RELEASE)/Ascii.o $(OBJDIR_RELEASE)/File.o $(OBJDIR_RELEASE)/Config.o $(OBJDIR_RELEASE)/Sigma.o

all: release

clean: clean_release

before_release: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) $(OBJ_RELEASE)  -o $(OUT_RELEASE) $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/Ascii.o: Ascii.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Ascii.cpp -o $(OBJDIR_RELEASE)/Ascii.o

$(OBJDIR_RELEASE)/File.o: File.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c File.cpp -o $(OBJDIR_RELEASE)/File.o

$(OBJDIR_RELEASE)/Config.o: Config.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Config.cpp -o $(OBJDIR_RELEASE)/Config.o

$(OBJDIR_RELEASE)/Sigma.o: Sigma.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Sigma.cpp -o $(OBJDIR_RELEASE)/Sigma.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_release after_release clean_release


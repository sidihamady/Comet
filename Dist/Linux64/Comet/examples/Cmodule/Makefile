WORKDIR = `pwd`

CC = gcc

INC =
CFLAGS = -Wall -funwind-tables

INC_RELEASE = -I../../include $(INC)
CFLAGS_RELEASE =  -O2 -fPIC -DUSE_LUAJIT -std=c99 $(CFLAGS)
LDFLAGS_RELEASE = -s -L../../bin -lluacore $(LDFLAGS)
OBJDIR_RELEASE = .
OUT_RELEASE = ./Cmodule.so
OBJ_RELEASE = ./Cmodule.o

all: release

clean: clean_release

before_release:
	test -d . || mkdir -p .

after_release:

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE)
	$(LD) -shared $(OBJ_RELEASE) -o $(OUT_RELEASE) $(LDFLAGS_RELEASE)

$(OBJDIR_RELEASE)/Cmodule.o: Cmodule.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Cmodule.c -o $(OBJDIR_RELEASE)/Cmodule.o

clean_release:
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)

.PHONY: before_release after_release clean_release


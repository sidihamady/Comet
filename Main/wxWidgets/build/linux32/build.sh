#!/bin/bash

make --file=Makefile.me clean
LD_RUN_PATH='$ORIGIN' make --file=Makefile.me
make --file=Makefile.me cleanobj
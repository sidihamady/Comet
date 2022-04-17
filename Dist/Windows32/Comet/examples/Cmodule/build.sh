#!/bin/sh

make clean
LD_RUN_PATH='$ORIGIN' make all

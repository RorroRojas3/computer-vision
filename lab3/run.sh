#!/bin/bash
FILE="lab3.c"
OUTPUT="lab3"
ARG1="parenthood.ppm"
ARG2="msf_e.ppm"
ARG3="ground_truth.txt"
gcc -Wall -g $FILE -o $OUTPUT
./$OUTPUT $ARG1 $ARG2 $ARG3


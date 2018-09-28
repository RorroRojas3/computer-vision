#!/bin/bash
FILE="lab3.c"
OUTPUT="lab3"
ARG1="parenthood.ppm"
ARG2="parenthood_e_template.ppm"
ARG3="ground_truth.txt"
ARG4="msf_e.ppm"
gcc -Wall -g $FILE -o $OUTPUT
./$OUTPUT $ARG1 $ARG2 $ARG3 $ARG4


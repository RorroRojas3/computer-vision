FILE="lab5.c"
OUT="lab5"
ARG1="hawk.ppm"
ARG2="initial_contour.txt"
gcc -Wall -g $FILE -o $OUT -lm
./$OUT $ARG1 $ARG2

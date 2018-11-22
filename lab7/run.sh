ARG1="lab7.c"
ARG2="data.txt"
OUTPUT="out"
gcc -g -Wall $ARG1 -o $OUTPUT -lm
./$OUTPUT $ARG2 
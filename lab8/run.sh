ARG1="lab8.c"
ARG2="chair-range.ppm"
OUTPUT="program"
gcc -Wall -g $ARG1 -o $OUTPUT -lm
./$OUTPUT $ARG2


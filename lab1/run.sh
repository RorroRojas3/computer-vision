FILE1="convolution_2d.c"
FILE2="separable_filters.c"
FILE3="sliding_window.c"
OUTPUT1="convolution"
OUTPUT2="separable_filters"
OUTPUT3="sliding_window"
gcc -Wall -g $FILE1 -o $OUTPUT1
gcc -Wall -g $FILE2 -o $OUTPUT2
gcc -Wall -g $FILE3 -o $OUTPUT3

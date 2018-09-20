#!/bin/bash
gcc -Wall -g  lab2.c -o lab2
./lab2 parenthood.ppm parenthood_e_template.ppm ground_truth.txt

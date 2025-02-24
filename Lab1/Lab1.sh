#!/bin/bash

# Compile the C program
gcc OS_lab1.c

# Run the compiled program with an argument (5 in this case)
./a.out 5

# Concatenate all output files matching the pattern 'output_[some number].txt'
#cat output_*.txt

read -p "Press any key to continue..."

# Remove the compiled program and all output files
rm a.out output_*.txt

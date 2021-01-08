#!/bin/sh

gcc -Wall -g test.c original_board.o -o test_teacher
./test_teacher > output_teacher

gcc -Wall -g test.c my_board.c -o test_yours
./test_yours > output_yours

diff -C 1 output_teacher output_yours
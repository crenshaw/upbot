# Filename: Makefile (for real-time interrupts demo)
# Author: Tanya L. Crenshaw
# Date created: 7/1/2013
# 
# Note: One may want to change the compiler CC based on if they are
# compiling for a desktop or an embedded system.
CC = gcc

# Compile the demo
demo.out:	real_timer.c error_functions.c get_num.c error_functions.h tlpi_hdr.h 
	$(CC)  -o demo.out real_timer.c error_functions.c get_num.c

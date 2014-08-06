# Very simple Makefile
CFLAGS=-Wall -ansi -pedantic

haconv: haconv.c
	gcc $(CFLAGS) -o $@ $<

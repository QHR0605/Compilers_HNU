#
# Makefile for C-MINUS
#

CC = gcc

CFLAGS =

SRCS = main.c util.c scan.c parse.c symtab.c analyze.c code.c cgen.c
OBJS = ${SRCS:c=o}

cminus: $(OBJS) tm
	$(CC) $(CFLAGS) $(OBJS) -o cminus

tm: tm.c
	$(CC) $(CFLAGS) tm.c -o tm

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^

clean:
	-rm cminus
	-rm $(OBJS)
	-rm tm
	-rm *.tm

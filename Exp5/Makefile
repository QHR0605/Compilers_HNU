CC := gcc

CFLAGS := 

SRCS := main.c util.c scan.c parse.c analyse.c symtab.c
OBJS := ${SRCS:c=o}

scanner: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o scanner

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^

clean:
	-rm scanner
	-rm $(OBJS)
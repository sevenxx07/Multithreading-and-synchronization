CC = gcc
CFLAGS = -Wall -g -pthread -O2
EXTRA_CFLAGS ?= -fsanitize=address -fno-omit-frame-pointer


%.o: %.c
	$(CC) -c -o $@ $< $(EXTRA_CFLAGS) $(CFLAGS)

prod-cons: prod-cons.o
	$(CC) -o prod-cons prod-cons.c $(EXTRA_CFLAGS) $(CFLAGS)

clean:
	$(RM) prod-cons osy05.tgz prod-cons.o

tar:
	make clean
	tar czf osy05.tgz Makefile *.[ch]

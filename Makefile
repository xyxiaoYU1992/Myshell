

CC       = gcc
CFLAGS   = -g
CPPFLAG  = -std=c90
CPPFLAGS = -std=c90 -Wall -pedantic

AR = ar
.PHONEY: all s myshell
all: allE#$TARGET

allE: myshell
myshell.o: myshell.c
	$(CC) $(CPPFLAG) $(CFLAGS) -c myshell.c
myshell: myshell.o
	$(CC) $(CPPFLAG) $(CFLAGS) myshell.o -o myshell

clean:
		rm -f PartA_1.o PartA_1.exe PartA_2.o Square.o PartA_2
		rm -f add.o move.o remo.o liblist.a test.o mytestlist other.o
		rm -f myshell.o myshell
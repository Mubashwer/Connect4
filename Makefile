###
  # Name: Mubashwer Salman Khurshid
  # Username: mskh
  # StudentID: 601738
  #
  # Basic Makefile for comp30023 project2
  ##


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall  

## OB = Object files.
## SR = Source files.
## EX = Executable name.

SR1 =		connect4.c server.c
OB1 =		connect4.o server.o
EX1 = 		server

SR2 =		connect4.c client.c
OB2 =		connect4.o client.o
EX2 = 		client

OBJ =		server.o client.o connect4.o

all: $(EX1) $(EX2)

## Top level target is executable.
$(EX1):	$(OB1)
		$(CC) $(CFLAGS) -o $(EX1) $(OB1) -lpthread

## Top level target is executable.
$(EX2):	$(OB2)
		$(CC) $(CFLAGS) -o $(EX2) $(OB2)


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ)

## Clobber: Performs Clean and removes executable files.

clobber: clean
		/bin/rm $(EX1) $(EX2) 

## Dependencies

connect4.o:	connect4.h
server.o:	server.h

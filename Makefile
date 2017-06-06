OBJS4= combied.o

SRC = $(OBJS:.o=.cc)


# Compiler settings
CC              = g++
CFLAGS          = -Wall -O2
INCLUDES        = -Iincludes


.PHONY: clean all

all: $(OBJS4)

combied: $(OBJS4)
	$(CC) $(CFLAGS)  -o bin/$@ build/$(OBJS4)
combied.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -o build/combied.o src/combied.cc 

clean:
	rm build/*

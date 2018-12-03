OBJS4= combied.o
OBJS1 = translate_trace.o

SRC = $(OBJS:.o=.cc)


# Compiler settings
CC              = g++
CFLAGS          = -Wall -O2
INCLUDES        = -Iincludes


.PHONY: clean all

all: $(OBJS4)

translate_trace: $(OBJS1)
	$(CC) $(CFLAGS)  -o bin/$@ build/$(OBJS1)
translate_trace.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -o build/translate_trace.o src/translate_trace.cc
	
combied: $(OBJS4)
	$(CC) $(CFLAGS)  -o bin/$@ build/$(OBJS4)
combied.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -o build/combied.o src/combied.cc 

clean:
	rm build/*

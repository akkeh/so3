CC = g++
CFLAGS = -I/usr/local/include -Wall -I../include
LDFLAGS= -L/usr/local/lib -lportaudio -llo

OBJ= main.o pEst.o STFT.o audio_io.o art.o neuron.o
SHELL := /bin/bash

all: sogmProj3

sogmProj3: $(OBJ)
	#$(shell echo ${OBJ})
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(LDFLAGS)

.cpp.o:
	$(CC) $< -c $(CFLAGS)

clean:
	rm -f *.o

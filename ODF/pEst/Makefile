CC = g++
CFLAGS = -Wall
LDFLAGS= -L/usr/local/lib -lportaudio

OBJ= main.o pEst.o STFT.o audio_io.o wavio.o
SHELL := /bin/bash

all: sogmProj3

sogmProj3: $(OBJ)
	#$(shell echo ${OBJ})
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(LDFLAGS)

.cpp.o:
	$(CC) $< -c $(CFLAGS)

clean:
	rm -f *.o

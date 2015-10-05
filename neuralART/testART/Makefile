CC = g++
CFLAGS = -Wall
LDFLAGS= -L/usr/local/lib -lportaudio

OBJ= main.o STFT.o art.o neuron.o wavio.o

all: sogmProj3

sogmProj3: $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(LDFLAGS)

.cpp.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o

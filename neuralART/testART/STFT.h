#ifndef __STFT__class__
#define __STFT__class__

#include <cmath>
#include <iostream>

#define TWOPI (M_PI+M_PI)
#define FOURPI (TWOPI+TWOPI)

//N
#define BUFFERSIZE 1024
//M
#define WINDOWSIZE 512
//H
#define HOPSIZE 128
//bins
#define FFTSIZE 1024

class STFT
{
public:
    
    STFT();
    STFT(unsigned long N, unsigned long M, unsigned long bins, unsigned long H);
    ~STFT();

    int FFT(float* data, unsigned long nn);

    float* getBlackman(unsigned long M);
    float* zeropad(float* x, unsigned long N, unsigned long newN);

    float** stft(float* x);
    float** stft(float* x, unsigned long N, unsigned long M, unsigned long bins, unsigned long H);
    float imabs(float re, float im);
    
private:
    //window
    float* w;
    
    unsigned long N;
    unsigned long M;
    unsigned long bins;
    unsigned long H;
};

#endif

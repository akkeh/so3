#include <cmath>
#include <iostream>
#include <cstdlib>

#include "ODF.h"

// N
#define BUFFERSIZE 1024
// M
#define WINDOWSIZE 512
// bins
#define FFTSIZE 1024
// H
#define HOPSIZE 128

#define TWOPI (M_PI+M_PI)
#define FOURPI (TWOPI+TWOPI)

ODF::ODF(unsigned long N, unsigned long M, unsigned long bins, unsigned long H) {
    stft = new STFT(N, M, bins, H);

    thMul = 1;
    pX_mem = new float[M];
    derv_mem = new float[M];
    for(unsigned long m=0; m<M; m++) {
        pX_mem[m] = 0;
        derv_mem[m] = 0;
    };
};


float imabs(float re, float im) {
    return std::sqrt((re*re)+(im*im));
};

float** stft_mag(float** X, unsigned long frames, unsigned long bins) {
    float** mX = new float*[frames];
    for(unsigned long l=0; l<frames; l++) {
        mX[l] = new float[bins];
        for(unsigned long k=0; k<bins; k++)
            mX[l][k] = imabs(X[l][2*k], X[l][2*k + 1]) / bins;
    };
    return mX;
};

float imphs(float re, float im) {
    return std::atan2(im, re);
};

float** transpose(float** x, unsigned long M, unsigned long N) {
    float** y = new float*[N];
    for(unsigned long n=0; n<N; ++n) {
        y[n] = new float[M];
        for(unsigned long m=0; m<M; ++m)
            y[n][m] = x[m][n];
    };
    return y;
};

float* phase_unwrap(float* x, unsigned long N) {
    float* y = new float[N];
    unsigned long addPI = 0;

    y[0] = x[0];
    for(unsigned long n=1; n<N; n++) {
        if((x[n] < 0) && (x[n - 1]))
            addPI++;
        y[n] = x[n] + (TWOPI * addPI);
    };
    return y;
};

float** nd_phase_unwrap(float** x, unsigned long M, unsigned long N) {
    float** y = new float*[M];
    for(unsigned long m=0; m<M; ++m)
        y[m] = phase_unwrap(x[m], N);

    return y;
        
};

float** stft_phs(float** X, unsigned long frames, unsigned long bins) {
    float** pX = new float*[frames];
    for(unsigned long l=0; l<frames; l++) {
        pX[l] = new float[bins];
        for(unsigned long k=0; k<bins; k++)
            pX[l][k] = imphs(X[l][2*k], X[l][2*k + 1]);
    };
    // transpose pX
    return pX;
};


float* derv(float* x, float mem, unsigned long N) {
    float* y = new float[N];
    y[0] = x[0] - mem;
    for(unsigned long n=1; n<N; ++n)
        y[n] = x[n] - x[n - 1];
    return y;
};

float** nd_derv(float** x, float* mem, unsigned long M, unsigned long N) {
    float** y = new float*[M];
    for(unsigned long m=0; m<M; ++m) {
        y[m] = derv(x[m], mem[m], N);
        mem[m] = y[m][N -1]; // save memory
    }
    return y;
};

float abs(float a) {
    return std::sqrt(a*a);
};


float* ODF::phaseFlux(float* x, unsigned long N, float th, unsigned long rechargeN, float binTh, unsigned long binRel) {
    float* onsets = new float[N];

    // STFT* stft = new STFT(WINDOWSIZE, FFTSIZE, HOPSIZE);
    // float** X = STFT->stft(x, N, WINDOWSIZE, FFTSIZE, HOPSIZE);
    float* x_compl = new float[N*2];
    for(unsigned long n=0; n<N; ++n) {
        x_compl[2*n] = x[n];
        x_compl[2*n+1] = 0;
    }
    float** X = stft->stft(x_compl, N, WINDOWSIZE, FFTSIZE, HOPSIZE);

    unsigned long frames = N/HOPSIZE;
    float** mX = stft_mag(X, frames, FFTSIZE);
    float** pX = stft_phs(X, frames, FFTSIZE);

    float** pXT = transpose(pX, frames, FFTSIZE);
    float** pX_unwrap = nd_phase_unwrap(pXT, FFTSIZE, frames);
    
    float** derv = nd_derv(pX_unwrap, pX_mem, FFTSIZE, frames);
    derv = nd_derv(derv, derv_mem, FFTSIZE, frames);
    
    for(unsigned long l=0; l<frames; ++l) {
        float val = 0;
        for(unsigned k=0; k<FFTSIZE; ++k)
            val += (derv[k][l]);// * (mX[l][k]);// > binTh);
        val = val / FFTSIZE;
        if(val > (th * thMul)) {
            std::cout << "val: " << val << " th: " << th << " thMul: " << thMul << std::endl;
            thMul = 100;
            onsets[l * HOPSIZE] = val;
            for(unsigned long k=0; k<FFTSIZE; ++k) {
                if(derv[k][l] > binTh) 
                    for(unsigned long n=0; n<binRel; ++n) {
                        mX[l + n][k] *= n/binRel;
                        if((l + n) <= frames)
                            break;
                    }
            };
        }
        if(thMul > 1) {
            thMul = thMul - 1/rechargeN;
        }
        if(thMul < 1)
            thMul = 1;
    }
    for(unsigned long l=0; l<frames; ++l) {
        delete[] mX[l];
        delete[] pX[l];
        delete[] X[l];
    };

    for(unsigned long k=0; k<FFTSIZE; ++k) {
        delete[] pXT[k];    
        delete[] pX_unwrap[k];    
        delete[] derv[k];
    }
    
    return onsets;
};



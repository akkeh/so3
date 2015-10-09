#include <cmath>

#include "pEst.h"
#include "STFT.h"

#define FS 44100
pEst::pEst(unsigned long M, unsigned long bins, unsigned long H) {
    this->M = M;
    this->bins = bins;
    this->H = H;
    est = new float[bins >> 1];
    Ts = new float[bins >> 1];
    mX_mem = new float[bins >> 1];
    for(unsigned long k=0; k<bins >> 1; ++k) {
        est[k] = 0;
        mX_mem[k] = 0;
    }
    thMul = 1;
};

float imphs(float re, float im) {
    return std::atan2(im, re);
};

float** stft_phs(float** X, unsigned long frames, unsigned long bins) {
    float** pX = new float*[frames];
    for(unsigned long l=0; l<frames; ++l) {
        pX[l] = new float[bins];
        for(unsigned long k=0; k<bins; ++k)
            pX[l][k] = imphs(X[l][2*k], X[l][2*k+1]);
    };
    return pX;
};

float imabs(float re, float im) {
    return std::sqrt((re*re) + (im*im));
};

float** stft_mag(float** X, unsigned long frames, unsigned long bins) {
    float** mX = new float*[frames];
    for(unsigned long l=0; l<frames; ++l) {
        mX[l] = new float[bins];
        for(unsigned long k=0; k<bins; ++k)
            mX[l][k] = imabs(X[l][2*k], X[l][2*k+1]);
    };
    return mX;
};

float* pEst::phaseFlux(float* x, unsigned long N, float errTh, float noiseTh, float onsetTh, float recharge) {
    unsigned long frames = N/H;
   
    float* x_c = new float[2*N];
    for(unsigned long n=0; n<N; ++n) {
        x_c[2*n] = x[n];
        x_c[2*n + 1] = 0;
    }; 

    STFT* stft = new STFT(N, M, bins, H);
    float** X = stft->stft(x_c, N, M, bins, H);

    float** mX = stft_mag(X, frames, bins);
    float** pX = stft_phs(X, frames, bins);

    unsigned long hBins = bins >> 1;
    float err;
    float* onsets = new float[N];
    for(unsigned long n=0; n<N; ++n)
        onsets[n] = 0;

    for(unsigned long l=0; l<frames; ++l) {
        float Ponset = 0;
        float amp = 0;
        for(unsigned long k=0; k<hBins; ++k) {
            // calculate next phase:
            for(unsigned long n=0; n<H; ++n) {
                est[k] += (TWOPI) / ((double)M/(double)k);
                if(est[k] > TWOPI)
                    est[k] = est[k] - TWOPI;
            };
            err = ((pX[l][k]+M_PI) - est[k]);
            err = err*err;  // squared error
            err = err < errTh;
            err = err * (mX[l][k] > noiseTh) * (mX[l][k] - mX_mem[k] > 0);
            Ponset += err; // increase probability of onset
            est[k] = pX[l][k] + M_PI; 
            mX_mem[k] = mX[l][k];
            
        }
        if(Ponset > (onsetTh * thMul))
            onsets[l*H] = Ponset;// / hBins;
    
       if(thMul > 1)
        thMul *= 1 - (1 / recharge);
    if(thMul < 1)
        thMul = 1;
    }
    
     for(unsigned long l=1; l<frames-1; ++l)
            if(onsets[l*H] > 0)
                if(onsets[(l-1)*H] == 0 && onsets[(l+1)*H] == 0)
                    onsets[l*H] = 0;
   

    return onsets;
};

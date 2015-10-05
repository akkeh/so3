#ifndef ODF_H_
#define ODF_H_

#include <cmath>
#include <iostream>

#include "STFT.h"

/* 
   --------------------------------------------|
    Onset detection function using phaseFlux
        by Akke H.
    improvements:
    -   static buffers?
    -   mem leaks?
   --------------------------------------------|
*/
class ODF {
public:
    ODF(unsigned long N, unsigned long M, unsigned long bins, unsigned long H, float th, float inhibTh, unsigned long inhibRel);
    float* phaseFlux(float* x, unsigned long N, float th, unsigned long rechargeN, float inhibTh, unsigned long inhibRel);
private:
    unsigned long N;    // input buffer lenght
    STFT* stft;
    
    float th;
    float inhibTh;
    float inhibRel;
    float thMul;
    float* pX_mem;
    float* derv_mem;    
    
};


#endif

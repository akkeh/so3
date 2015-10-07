#ifndef PEST_H_
#define PEST_H_

class pEst{
public:
    pEst(unsigned long M, unsigned long bins, unsigned long H);
    float* phaseFlux(float* x, unsigned long N, float errTh, float onsetTh, float recharge);
    
private:
    unsigned long M, bins, H;
    float* est;
    float* Ts;
    float th;
    float thMul;
};



#endif
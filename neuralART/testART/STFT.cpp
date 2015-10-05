#include "STFT.h"

/*
    -   static buffers?
    -   mem leaks
*/

STFT::STFT() {
    N = BUFFERSIZE;
    M = WINDOWSIZE;
    bins = HOPSIZE;
    H = FFTSIZE;

    w = getBlackman(M);
}

STFT::STFT(unsigned long N, unsigned long M, unsigned long bins, unsigned long H) {
    this->N = N;
    this->M = M;
    this->bins = bins;
    this->H = H;

    //  x_part = new float[2*M];   // allocation improvements?
    w = getBlackman(M);
}

STFT::~STFT() {
    delete[] w;
}

int STFT::FFT(float* data, unsigned long nn) {
    unsigned long n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;

    // reverse-binary reindexing
    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            std::swap(data[j-1], data[i-1]);
            std::swap(data[j], data[i]);
        }
        m = nn;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    };

    // Danielson-Lanczos
    mmax = 2;
    while (n > mmax) {
        istep = mmax << 1;
        theta = -(2 * M_PI / mmax);
        wtemp = sin(0.5 * theta);
        wpr = -2.0 * wtemp*wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m; i <= n; i += istep) {
                j= i + mmax;
                tempr = wr * data[j-1] - wi * data[j];
                tempi = wr * data[j] + wi * data[j-1];

                data[j-1] = data[i-1] - tempr;
                data[j] = data[i] - tempi;
                data[i-1] += tempr;
                data[i] += tempi;
            }
            wtemp = wr;
            wr += wr*wpr - wi*wpi;
            wi += wi*wpr + wtemp*wpi;
        }
        mmax = istep;
    }
    return 0;
}

float* STFT::getBlackman(unsigned long M) {
    float a0, a1, a2;
    a0 = 7938.0/18608.0;
    a1 = 9240.0/18608.0;
    a2 = 1430.0/18608.0;

    float* w = new float[M];
    for(unsigned long n=0; n<M; ++n) {
        w[n] = a0 - a1 * std::cos((TWOPI * (long double)n) / ((long double)M - 1)) + a2 * std::cos((FOURPI * (long double)n) / ((long double)M - 1));
    };

    return w;
}

float* STFT::zeropad(float* x, unsigned long N, unsigned long newN) {
    float* y = new float[newN];
    for (unsigned long n=0; n<N; ++n)
        y[n] = x[n];
    for(unsigned long n=N; n<newN; n++)
        y[n] = 0;
    return y;
}

float** STFT::stft(float* x) {
    return stft(x, N, M, bins, H);
}

float** STFT::stft(float* x, unsigned long N, unsigned long M, unsigned long bins, unsigned long H) {
    // N multiple of M?
    unsigned long frames = N/H;

    float** X = new float*[frames];

    float* x_part = new float[bins];    // static!

    for(unsigned long l=0; l<frames; l++) {
        // copy input buffer:
        for(unsigned long n=0; n<M; n++) {
            x_part[2*n] = x[(2*H)*l + 2*n] * w[n];          // even
            x_part[2*n + 1] = x[(2*H)*l + 2*n + 1] * w[n];  // odd
        };
        X[l] = zeropad(x_part, M*2, bins*2);
        FFT(X[l], bins);

    }
    delete[] x_part;

    return X;
}

float STFT::imabs(float re, float im) {
    return std::sqrt((re*re)+(im*im));
}

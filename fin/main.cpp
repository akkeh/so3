#include <cmath>
#include <cstdlib>
#include <fstream>


#include "art.h"
#include "STFT.h"
#include "pEst.h"
#include "audio_io.h"

#define SAMPLERATE	44100
#define CHANNELS	1
#define FRAMES	    65536
#define ARGCOUNT 5

/* sample input:
$ ./sogmProj3 4096 0.08 0.001 100
*/

int main(int argc, char** argv) {    
    if(argc < ARGCOUNT) {
        std::cout << "usage: [vigilance][blocksize][errorTh][noiseTh][onsetTh][recharge]\n";
        return -1;
    }

    unsigned long N = atoi(argv[2]);
    float* buffer = new float[N];
    
    
    // open audiostream:
    Audio_IO audioStream(SAMPLERATE, 1);
    static int input_device;

    audioStream.set_mode(AUDIO_IO_READONLY);
    audioStream.set_framesperbuffer(N);
    audioStream.initialise();
    audioStream.list_devices();
    std::cout << "\nGive input device number: ";
    std::cin >> input_device;
    audioStream.set_input_device(input_device);
    audioStream.start_server();

    // init ODF:
    float* onsets = new float[N];
    float th = atof(argv[3]);
    float noiseTh = atof(argv[4]);
    float onsetTh = atof(argv[5]);
    float recharge = atof(argv[6]);
    pEst odf(512, 512, 128);

    // init STFT:
    STFT stft(N, 512, 512, 128);
    float** X;

    // init ART:
    float vigl = atof(argv[1]);
    ART art(N/128, 512, vigl);

    unsigned long strt_n = 0;
    unsigned long old_strt = 0;
    float* snd = new float[N];
    while (true) {

        audioStream.read(buffer);
        strt_n = odf.phaseFlux(buffer, N, th, noiseTh, onsetTh, recharge);
        for(unsigned long n=0; n<2048-old_strt; ++n)
            snd[2048-old_strt + n] = buffer[n];
        X = stft.stft(snd, 2048, 512, 512, 128);
        art.eval(X, 0);

        old_strt = strt_n;
        for(unsigned long n=0; n<N-strt_n; ++n)
            snd[n] = buffer[n + strt_n];       
    }
    
    delete[] buffer;

    return 0;
}

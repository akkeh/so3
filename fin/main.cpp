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

    long onset_n = 0;
    unsigned long w_ptr = 0;
    unsigned long sndN = N*64;

    float* snd = new float[sndN];
    for(unsigned long n=0; n<N; ++n)
        snd[n] = 0;

    while (true) {
        audioStream.read(buffer);
        std::cout << "ODF\n";
        onset_n = odf.phaseFlux(buffer, N, th, noiseTh, onsetTh, 0);
        if(onset_n > 0) {
            w_ptr = 0;
            for(unsigned long n=0; n<N-onset_n; ++n) {
                snd[w_ptr] = buffer[onset_n + n];
                ++w_ptr;
            }
            while(w_ptr < sndN) {
                audioStream.read(buffer);
                for(unsigned long n=0; n<N; ++n) {
                    snd[w_ptr] = buffer[n];
                    ++w_ptr;
                    if(w_ptr > sndN)
                        break;
                }
            }; 

            std::cout << "STFT\n";
            X = stft.stft(snd, 2048, 512, 512, 128);
            std::cout << "ART\n";
   //         art.eval(X, 0);
        }
    }
    
    delete[] buffer;

    return 0;
}

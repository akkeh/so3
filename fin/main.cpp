#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>

#include "art.h"
#include "STFT.h"
#include "pEst.h"
#include "audio_io.h"
#include <lo/lo.h>

#define SAMPLERATE	44100
#define CHANNELS	1
#define FRAMES	    65536
#define ARGCOUNT 5

/* sample input:
$ ./sogmProj3 0.01 2048 0.2 1 100

*/

//OSC send function
int sosc(short bang) {
    lo_address target;
    std::string symbol;
    target = lo_address_new("localhost","7777");
    lo_send(target,"/bang","ii",bang, 1);
    bang++;

    return 0;
}

void wait(long t) {
    std::time_t t0 = std::time(0);
    while(std::time(0) - t0 < t) {}; 
};
    

int main(int argc, char** argv) {
    if(argc < ARGCOUNT) {
        std::cout << "usage: [vigilance][blocksize][errorTh][noiseTh][onsetTh]\n";
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

    // init read/write pointers
    long onset_n = 0;
    unsigned long w_ptr = 0;
    unsigned long sndN = N*10;

    // init ODF:
    float th = atof(argv[3]);
    float noiseTh = atof(argv[4]);
    float onsetTh = atof(argv[5]);
    pEst odf(512, 512, 128);

    // init STFT:
    STFT stft(sndN, 512, 512, 128);
    float** X;

    // init ART:
    float vigl = atof(argv[1]);
    short group_id;
    ART art(N/128, 512, vigl);

    float* snd = new float[2*sndN];
    for(unsigned long n=0; n<N; ++n)
        snd[n] = 0;

    unsigned long last_Onset = 0;
    unsigned long i = 0;
    unsigned long waitN = 0;
    while (true) {
        audioStream.read(buffer);
        onset_n = odf.phaseFlux(buffer, N, th, noiseTh, onsetTh, 0);
        if(i > waitN) {

                if(onset_n > 0) {
                    std::cout << "last onset: " << last_Onset << " now: " << i << std::endl;
                    last_Onset = i;
                    w_ptr = 0;
                    for(unsigned long n=0; n<N-onset_n; ++n) {
                        snd[2*w_ptr] = buffer[onset_n + n];
                        snd[2*w_ptr + 1] = 0;
                        ++w_ptr;
                    }
                    while(w_ptr < sndN) {
                        audioStream.read(buffer);
                        for(unsigned long n=0; n<N; ++n) {
                            snd[2*w_ptr] = buffer[n];
                            snd[2*w_ptr + 1] = 0;
                            ++w_ptr;
                            if(w_ptr > sndN)
                                break;
                        }
                    };
                    std::cout << "STFT\n";
                    X = stft.stft(snd, sndN, 512, 512, 128);
                    std::cout << "ART\n";

                    group_id = art.eval(X, 0);
                    if (group_id >= 0) {
                        sosc(group_id);
                        wait(1);
                    }
                    std::cout << "now listening..\n";
                }
        }
        ++i;
    }

    delete[] buffer;

    return 0;
}

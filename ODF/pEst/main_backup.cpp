#include <cmath>
#include <cstdlib>

#include "pEst.h"
#include "audio_io.h"
#include "wavio.h"

#define SAMPLERATE	44100
#define CHANNELS	1
#define FRAMES	    65536
#define ARGCOUNT 3 

/* sample input:
$ ./sogmProj3 4096 0.08 0.001 100
*/

int findNotZeros(float* x, unsigned long N, unsigned long offset, float* buffer) {
    int count = 0;
    for(unsigned long n=0; n<N; ++n) {
        if(x[n] > 0) {
            count++;
        };
    };
    if(count > 0) 
        std::cout << count << "\n";
    for(unsigned long n=0; n<128; ++n)
        std::cout << 0 << std::endl;
    return count;
};

int main(int argc, char** argv) {    
    if(argc < ARGCOUNT) {
        std::cout << "usage: ....\n";
        return -1;
    }

    float* mem = new float[256*256];
    unsigned char wr_ptr1;
    unsigned char wr_ptr2;
    
    unsigned long N = atoi(argv[2]);
    // open audiostream:
    Audio_IO audioStream(SAMPLERATE, 1);
    float* buffer = new float[N];
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
    float th = atof(argv[3]);
    float* onsets;

    pEst* odf = new pEst(512, 512, 128);

    unsigned long bufN = -10;
    float* file_buf = readWav(argv[1], &bufN, 0);
    bool fileRead = true;
    unsigned long rptr = 0;
    unsigned long frame = 0; 
    while (true) {
        /*
        if(!fileRead)
            audioStream.read(buffer); 
        */
            for(unsigned long n=0; n<N; ++n) {
                buffer[n] = file_buf[rptr];
                rptr++;
                if(rptr > bufN)
                    goto exit;
            };
        onsets = odf->phaseFlux(buffer, N, th);
        findNotZeros(onsets, N, frame, buffer);
        frame++;
    }
    exit:
    delete[] buffer;

    return 0;
}

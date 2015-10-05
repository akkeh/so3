#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "STFT.h"
#include "neuron.h"
#include "art.h"
#include "wavio.h"

#define ARGC 2
int main(int argc, char** argv) {
    if(argc < ARGC) {
        return -1;  
    };
    float vigilance = atof(argv[1]);
    std::ifstream files;
    files.open("files.txt");
    std::string file;

    std::ofstream resFP;
    resFP.open("res_fP.txt", std::ofstream::app);
    std::ofstream resFN;
    resFN.open("res_fN.txt", std::ofstream::app);

    resFP << vigilance << "\t";    
    resFN << vigilance << "\t";    
    
    STFT* stft = new STFT(4096, 512, 1024, 128);

    ART* art = new ART(4096/128, 1024, vigilance);
    unsigned long N;
    int fP, fN;
    std::string fileIdstr;    
    while(std::getline(files, file)) {
        N = -10;
        float* x = readWav(file.c_str(), &N, 0);
        std::cout << "loading file: " << file << std::endl;
        std::getline(files, fileIdstr);
        int fileId = atoi(fileIdstr.c_str()); 
        if(N > 0) {
            float** X = stft->stft(x, 4096, 512, 1024, 128);
            int respId = art->eval(X, fileId);
            if(art->get_id_of(respId) != fileId) {
                fP++;
            } else {
                bool falseNeg = false;
                for(int i=0; i<respId - 1; i++)
                    if(art->get_id_of(respId) == fileId)
                        falseNeg = true;
                if(falseNeg)
                    fN++;
            };
                   
            
        };
    }
    resFP << fP << std::endl;
    resFN << fN << std::endl;
};

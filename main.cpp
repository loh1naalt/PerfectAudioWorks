#include <iostream>
#include "AudioPharser/Portaudiohandler.h"

int main(int argc, char* argv[]){
    
    if (argc < 2){
        printf("must pass file");
        return 0;
    }

    PaHandler(argv[1]);
    return 0;
}
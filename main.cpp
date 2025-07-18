#include <iostream>
#include "AudioPharser/PortAudioHandler.h"
#include "PAW_GUI/main_paw_widget.h"

#include <QApplication>

int main(int argc, char* argv[]){
    
    // if (argc < 2){
    //     printf("must pass a file");
    //     return 0;
    // }
    PortaudioThread thread;
    thread.PaInit();
    
    QApplication a(argc, argv);
    Main_PAW_widget w;
    w.show();

    return a.exec();
}
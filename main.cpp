#include <iostream>
#include "AudioPharser/PortAudioHandler.h"
#include "PAW_GUI/main_paw_widget.h"

#include <QApplication>

int main(int argc, char* argv[]){
    
    PortaudioThread thread;
    thread.PaInit();
    
    QApplication a(argc, argv);
    Main_PAW_widget w;
    w.show();
    if (argc > 1){
        w.start_playback(argv[1]);  
    }
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&thread]() {
        thread.stop();
    });

    return a.exec();
}
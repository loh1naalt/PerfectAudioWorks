#include <iostream>

#include "PAW_GUI/main_paw_widget.h"

#include <QApplication>

int main(int argc, char* argv[]){



    QApplication a(argc, argv);
    Main_PAW_widget w;

    w.show();

    if (argc > 1){
        w.start_playback(QString(argv[1]));
    }


    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&w]() {
        w.getAudioThread().stopPlayback(); 
    });

    return a.exec();
}
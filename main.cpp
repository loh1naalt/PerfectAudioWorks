#include <iostream>
#include "AudioPharser/Portaudiohandler.h"
#include "PAW_GUI/main_paw_widget.h"

#include <QApplication>

int main(int argc, char* argv[]){
    
    // if (argc < 2){
    //     printf("must pass file");
    //     return 0;
    // }
    QApplication a(argc, argv);
    Main_PAW_widget w;
    w.show();
    return a.exec();
}
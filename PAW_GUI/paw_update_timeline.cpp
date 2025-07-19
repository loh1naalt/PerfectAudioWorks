#include "paw_update_timeline.h"



Updatetimeline::Updatetimeline(QObject *parent): QThread(parent), TimelineSlider(nullptr){}

Updatetimeline::~Updatetimeline(){
    wait();
}

void Updatetimeline::UpdateCycle(){
    callback_data_s streamdata;
    framesinpercent = streamdata.currentframe / streamdata.Fileinfo.frames * 100.0f;
    for (streamdata.currentframe; streamdata.currentframe < streamdata.Fileinfo.frames; streamdata.currentframe++){
        // printf("%d\n", streamdata.currentframe);
        // printf("%d\n", streamdata.Fileinfo.frames);
        // printf("%f\n", framesinpercent);
        TimelineSlider -> setValue(framesinpercent);
    }
    
}
void Updatetimeline::SetSlider(QSlider *slider){
    printf("updated slider\n");
    TimelineSlider = slider;
}
void Updatetimeline::run(){
    printf("starting\n");
    UpdateCycle();
}
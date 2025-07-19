#ifndef UPDATETIMELINE_H
#define UPDATETIMELINE_H

#include <iostream>

#include <QSlider>
#include <QThread>

#include "../AudioPharser/PortAudioHandler.h"

class Main_PAW_widget;

class Updatetimeline : public QThread{

    Q_OBJECT

    public:

    Updatetimeline(QObject *parent = nullptr);
    ~Updatetimeline();
    void UpdateCycle();
    void SetSlider(QSlider *slider);

    private:
    QSlider *TimelineSlider;
    float   framesinpercent;


    protected:
    void run() override;

};

#endif
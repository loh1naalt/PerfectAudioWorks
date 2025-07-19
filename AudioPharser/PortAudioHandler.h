#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

#include <sndfile.h>
#include <portaudio.h>
#include <QThread>
#include <QSlider>

#include "wavpharser.h"

struct  GUIElements
{
    /* data */
};

struct callback_data_s
{
    SNDFILE     *file;
    SF_INFO      Fileinfo;
    float	 	 frametime;
    sf_count_t	 currentframe;
    int			 rewindtoframe;
        
};

struct Pa_info
{
    // int IsRunning;
    PaStream *Stream;
};

class PortaudioThread : public QThread {

    Q_OBJECT

    public:

    PortaudioThread(QObject *parent = nullptr);
    ~PortaudioThread();

    bool returnIsRunning();
    int Portaudiohandler(int calltype);
    void PaInit();
    float UpdateCycle(int currentFrame, int MaxFrames);
    void StartPlayback();
    void setFile(char *filenameset);
    void stop();
    void SetSlider(QSlider *slider);

    // void CheckPaError(PaError err);
    // int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    //                 const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
    //                 void *userData );
    private:
    char* filename;
    bool IsRunning;
    QSlider *TimelineSlider;


    protected:
    void run() override;

};

#endif
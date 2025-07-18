#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <sndfile.h>
#include <portaudio.h>
#include <iostream>
#include <QThread>

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
    PaStream *Stream;
};

class PortaudioThread : public QThread {

    Q_OBJECT

    public:


    PortaudioThread(QObject *parent = nullptr);
    ~PortaudioThread();

    int Portaudiohandler(int calltype);
    void PaInit();
    void StartPlayback();
    void setFile(char *filenameset);
    // void CheckPaError(PaError err);
    // int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    //                 const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
    //                 void *userData );
    private:
    char* filename;

    protected:
    void run() override;

};

#endif
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

class PortaudioThread : public QThread {

    Q_OBJECT

    public:
    struct callback_data_s
    {
    SNDFILE     *file;
    SF_INFO      Fileinfo;
    PortaudioThread *playerThread; 
    sf_count_t	 currentframe;
    int			 rewindtoframe;
        
    };

    std::map<std::string, int> FileInfoDict;
    callback_data_s *File_data;
    PortaudioThread(QObject *parent = nullptr);
    ~PortaudioThread();

    
    bool returnIsRunning();
    void SetFrameFromTimeline(int ValueInPercent);
    int Portaudiohandler(int calltype);
    void PaInit();
    // float CalculatePercentage(int currentFrame, int MaxFrames);
    void StartPlayback();
    void setFile(char *filenameset);
    void ReturnFileinfo(int CurrentFrame, int frames, int Samplerate);
    void stop();

    // void CheckPaError(PaError err);
    // int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    //                 const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
    //                 void *userData );
    private:
    char* filename;
    bool IsRunning;
    PaStream *m_stream;
    static int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                    void *userData );


    protected:
    void run() override;

};

#endif
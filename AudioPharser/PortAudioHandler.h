#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h> 

#include <sndfile.h>
#include <portaudio.h>
#include <QThread>
#include <QString> 



#include "wavpharser.h" 

#define BitsPerSample 512 

class PortaudioThread : public QThread {
    Q_OBJECT

public:
    struct callback_data_s {
        SNDFILE     *file;
        SF_INFO      Fileinfo;
        PortaudioThread *playerThread;
        sf_count_t   currentframe;
        int          rewindtoframe;
    };

   


    explicit PortaudioThread(QObject *parent = nullptr); 
    ~PortaudioThread() override; 

    
    void PaInit(); 
    void StartPlayback();
    void setFile(const QString &filename); 
    void setPlayPause();
    bool isPaused() const; 
    void SetFrameFromTimeline(int valueInPercent);
    void stopPlayback(); 

    
    int Portaudiohandler(int calltype); 

signals:
    void playbackProgress(int currentFrame, int totalFrames, int sampleRate);
    void totalFileInfo(int totalFrames, int sampleRate); 
    void playbackFinished();
    void errorOccurred(const QString &errorMessage); 

protected:
    void run() override; 

private:

    QString m_filename; 
    bool m_isRunning;   
    bool m_isPaused;    
    PaStream *m_stream;
    callback_data_s m_filedata; 


    static int audio_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                              void *userData);


};

#endif // AUDIOTHREAD_H
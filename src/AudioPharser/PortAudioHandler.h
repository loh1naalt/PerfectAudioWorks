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

#include "libsndfiledecoder.h"

#define BitsPerSample 512 



class PortaudioThread : public QThread {
    Q_OBJECT

public:
    typedef struct {
        PortaudioThread *playerThread; 
        SndFileDecoder  *sndFileDecoder; 

    } SndfileCallback;

    explicit PortaudioThread(QObject *parent = nullptr); 
    ~PortaudioThread() override; 

    
    void PaInit(); 
    void StartPlayback(const int OutputDevice);
    void setFile(const QString &filename); 
    void setPlayPause();
    bool isPaused() const; 
    void SetFrameFromTimeline(int valueInPercent);
    void stopPlayback();
    QStringList GetAvaliableDevices();
    PaDeviceIndex GetDefaultDevice();

    
    

signals:
    void playbackProgress(int currentFrame, int totalFrames, int sampleRate);
    void totalFileInfo(int totalFrames, int sampleRate); 
    void playbackFinished();
    void errorOccurred(const QString &errorMessage); 

protected:
    void run() override; 

private:
    PortaudioThread *playerThread;
    SndFileDecoder m_sndFileDecoder;
    QString m_filename; 
    bool m_isRunning;   
    bool m_isPaused;    
    PaStream *m_stream;
    SndfileCallback m_SndFileData; 


    static int audio_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                              void *userData);


};

#endif // AUDIOTHREAD_H
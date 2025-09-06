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

#include "CodecHandler.h"

#define BitsPerSample 512 



class PortaudioThread : public QThread {
    Q_OBJECT

public:
    typedef struct {
        PortaudioThread *playerThread; 
        CodecHandler  *CodecDecoder; 

    } CodecCallback;

    explicit PortaudioThread(QObject *parent = nullptr); 
    ~PortaudioThread() override; 

    
    void PaInit(); 
    void StartPlayback();
    void setFile(const QString &filename); 
    void setAudioDevice(int set_audiodevice);
    void setPlayPause();
    bool isPaused() const; 
    void SetFrameFromTimeline(int valueInPercent);
    void stopPlayback();
    QList<QPair<QString, int>> GetAvailableOutputDevices();
    PaDeviceIndex GetDefaultDevice();

    
    

signals:
    void playbackProgress(int currentFrame, int totalFrames, int sampleRate);
    void totalFileInfo(int totalFrames, int sampleRate); 
    void playbackFinished();
    void errorOccurred(const QString &errorMessage); 

protected:
    void run() override; 

private:
    CodecHandler *m_CodecDecoder;
    QString m_filename; 
    bool m_isRunning;   
    bool m_isPaused;    
    PaStream *m_stream;
    PaDeviceIndex audiodevice;
    CodecCallback m_CodecData; 


    static int audio_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                              void *userData);


};

#endif // AUDIOTHREAD_H
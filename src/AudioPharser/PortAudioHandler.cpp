#include "PortAudioHandler.h"
#include <QDebug>
#include <QByteArray> 


#define framesPerBuffer_GLOBAL 512 



static int checkFileOnErrors(SNDFILE *file) {
    if (file && sf_error(file) != 0) { 
        qCritical() << "Sndfile Error!: " << sf_strerror(file);
        return sf_error(file);
    }
    return 0;
}


static void CheckPaError(PaError err) {
    if (err != paNoError) {
        qCritical() << "Portaudio Error!: " << Pa_GetErrorText(err);
        exit(1); 
    }
}


int PortaudioThread::audio_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer_local,
                                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                                    void *userData) {
    Q_UNUSED(inputBuffer);
    Q_UNUSED(timeinfo);
    Q_UNUSED(statusFlags);

    float *out = (float*)outputBuffer;
    SndfileCallback *p_data = (SndfileCallback*)userData;

    if (!p_data || !p_data->playerThread || !p_data->sndFileDecoder) {
        qCritical() << "Portaudio callback: Invalid user data, playerThread, or sndFileDecoder pointer!";
        return paAbort;
    }

    bool isPaused = p_data->playerThread->m_isPaused;

    
    const SF_INFO& fileInfo = p_data->sndFileDecoder->getFileInfo();
    sf_count_t currentLogicalFrame = p_data->sndFileDecoder->getLogicalCurrentFrame();

    
    memset(out, 0, sizeof(float) * framesPerBuffer_local * fileInfo.channels);

    
    emit p_data->playerThread->playbackProgress(
        static_cast<int>(currentLogicalFrame),
        static_cast<int>(fileInfo.frames),
        static_cast<int>(fileInfo.samplerate)
    );

    if (isPaused) {
        return paContinue; 
    }


    sf_count_t num_read_samples = p_data->sndFileDecoder->SndfileReadFloat(out, framesPerBuffer_local);
    sf_count_t num_read_frames = num_read_samples / fileInfo.channels;



    if (num_read_samples < (framesPerBuffer_local * fileInfo.channels)) {
        memset(out + num_read_samples, 0,
               (framesPerBuffer_local * fileInfo.channels - num_read_samples) * sizeof(float));

        if (num_read_samples == 0 && currentLogicalFrame >= fileInfo.frames) {
            emit p_data->playerThread->playbackFinished();
            return paComplete; 
        }
    }
    return paContinue; 
}

PortaudioThread::PortaudioThread(QObject *parent)
    : QThread(parent), m_isRunning(false), m_isPaused(false), m_stream(nullptr) {

    m_SndFileData.playerThread = this;
    m_SndFileData.sndFileDecoder = &m_sndFileDecoder; 
    
}


PortaudioThread::~PortaudioThread() {
    stopPlayback(); 
    if (m_stream) {
        PaError err = paNoError;
        if (Pa_IsStreamActive(m_stream) == 1) {
            err = Pa_StopStream(m_stream);
            if (err != paNoError) qWarning() << "Error stopping stream in destructor:" << Pa_GetErrorText(err);
        }
        err = Pa_CloseStream(m_stream);
        if (err != paNoError) qWarning() << "Error closing stream in destructor:" << Pa_GetErrorText(err);
        m_stream = nullptr;
    }
    Pa_Terminate(); 
}


QStringList PortaudioThread::GetAvaliableDevices(){
    QStringList deviceList;
    int quantityDevices = Pa_GetDeviceCount();
    
    if (quantityDevices < 0) {
        qCritical() << "PortAudio Error: " << Pa_GetErrorText(quantityDevices);
        return deviceList;
    }
    
    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < quantityDevices; ++i) {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxOutputChannels > 0) {
            // Add the device name to the list
            deviceList << deviceInfo->name;
        }
    }
    
    return deviceList;
}

PaDeviceIndex PortaudioThread::GetDefaultDevice(){
           
    PaDeviceIndex outputDevice = Pa_GetDefaultOutputDevice();
    if (outputDevice == paNoDevice) {
        emit errorOccurred("No default output device found.");
        return -1;
    }
    const PaDeviceInfo *infoO = Pa_GetDeviceInfo(outputDevice);
    if (infoO) {
        qDebug() << "Default output Device:" << infoO->name;
        return outputDevice;
    } else {
        emit errorOccurred("Failed to get info for default output device.");
        return -1;
            
    }
}
        


void PortaudioThread::PaInit() {
    PaError err = Pa_Initialize();
    CheckPaError(err);
}


void PortaudioThread::setFile(const QString &filename) {
    m_filename = filename;
}


void PortaudioThread::StartPlayback(const int OutputDevice) {
    PaError err;

    
    QByteArray ba = m_filename.toLocal8Bit();
    const char *filename_c_str = ba.data();

    if (!m_sndFileDecoder.SndfileSetFile(filename_c_str)) {
        emit errorOccurred("Could not open file: " + m_filename);
        return;
    }

 
    const SF_INFO& fileInfo = m_sndFileDecoder.getFileInfo();

    emit totalFileInfo(static_cast<int>(fileInfo.frames),
                       static_cast<int>(fileInfo.samplerate));

    if (OutputDevice == -1) {
        emit errorOccurred("Failed to get default output device.");
        m_sndFileDecoder.SndfileCloseFile(); 
        return;
    }

    PaStreamParameters outputParameters;
    memset(&outputParameters, 0, sizeof(outputParameters));
    outputParameters.channelCount = fileInfo.channels;
    outputParameters.device = OutputDevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(OutputDevice)->defaultLowOutputLatency;

    err = Pa_OpenStream(
        &m_stream,
        0, 
        &outputParameters,
        fileInfo.samplerate,
        framesPerBuffer_GLOBAL, 
        paNoFlag,
        audio_callback,
        &m_SndFileData 
    );
    CheckPaError(err);

    
    err = Pa_StartStream(m_stream);
    CheckPaError(err);

    m_isRunning = true;
    m_isPaused = false;
}


void PortaudioThread::run() {
    StartPlayback(GetDefaultDevice());

    
    while (m_stream && Pa_IsStreamActive(m_stream) == 1 && m_isRunning) {
        QThread::msleep(50);
    }

    
    if (m_stream) {
        PaError err = paNoError;
        if (Pa_IsStreamActive(m_stream) == 1) { 
            err = Pa_StopStream(m_stream);
            if (err != paNoError) qWarning() << "Error stopping stream:" << Pa_GetErrorText(err);
        }
        err = Pa_CloseStream(m_stream);
        if (err != paNoError) qWarning() << "Error closing stream:" << Pa_GetErrorText(err);
        m_stream = nullptr;
    }

}


void PortaudioThread::stopPlayback() {
    if (!m_isRunning) return;

    m_isRunning = false; 


    wait();
}

void PortaudioThread::setPlayPause() {
    m_isPaused = !m_isPaused;
}


bool PortaudioThread::isPaused() const {
    return m_isPaused;
}

void PortaudioThread::SetFrameFromTimeline(int ValueInPercent) {
    const SF_INFO& fileInfo = m_sndFileDecoder.getFileInfo();

    if (!m_sndFileDecoder.getLogicalCurrentFrame() || fileInfo.frames <= 0) { 
        qWarning() << "Cannot seek: no file loaded or file has no frames.";
        return;
    }

    float percentage = ValueInPercent / 100.0f;
    sf_count_t targetFrame = static_cast<sf_count_t>(percentage * fileInfo.frames);


    sf_count_t seek_result = m_sndFileDecoder.SetSampleTo(targetFrame);

    if (seek_result == -1) {

        qWarning() << "Error seeking in file via decoder.";
        emit errorOccurred("Error seeking in file."); 
    } else {

        emit playbackProgress(
            static_cast<int>(seek_result), 
            static_cast<int>(fileInfo.frames),
            static_cast<int>(fileInfo.samplerate)
        );
    }
}
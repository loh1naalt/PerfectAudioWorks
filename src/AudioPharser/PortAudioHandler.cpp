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


int PortaudioThread::audio_callback(const void *inputBuffer, void *outputBuffer,
                                    unsigned long framesPerBuffer_local,
                                    const PaStreamCallbackTimeInfo* timeinfo,
                                    PaStreamCallbackFlags statusFlags,
                                    void *userData)
{
    Q_UNUSED(inputBuffer);
    Q_UNUSED(timeinfo);
    Q_UNUSED(statusFlags);

    float *out = static_cast<float*>(outputBuffer);
    CodecCallback *p_data = static_cast<CodecCallback*>(userData);

    if (!p_data || !p_data->playerThread || !p_data->CodecDecoder) {
        qCritical() << "Portaudio callback: Invalid user data!";
        return paAbort;
    }

    bool isPaused = p_data->playerThread->m_isPaused;
    int channels = codec_get_channels(p_data->CodecDecoder);
    long totalFrames = codec_get_total_frames(p_data->CodecDecoder);
    long currentFrame = codec_get_current_frame(p_data->CodecDecoder);
    int samplerate = codec_get_samplerate(p_data->CodecDecoder);

    if (isPaused) {
        memset(out, 0, sizeof(float) * framesPerBuffer_local * channels);
        return paContinue;
    }
    memset(out, 0, framesPerBuffer_GLOBAL * channels * sizeof(float));
    long num_read_frames = codec_read_float(
        p_data->CodecDecoder,
        out,
        framesPerBuffer_GLOBAL);
    
    emit p_data->playerThread->playbackProgress(
        static_cast<int>(currentFrame),
        static_cast<int>(totalFrames),
        samplerate
    );

    if (num_read_frames < (long)framesPerBuffer_local) {
        size_t offset = num_read_frames * channels;
        size_t remaining = (framesPerBuffer_local - num_read_frames) * channels;
        memset(out + offset, 0, remaining * sizeof(float));

        if (num_read_frames == 0 && currentFrame >= totalFrames) {
            emit p_data->playerThread->playbackFinished();
            return paComplete;
        }
    }

    return paContinue;
}


PortaudioThread::PortaudioThread(QObject *parent)
    : QThread(parent), m_isRunning(false), m_isPaused(false), m_stream(nullptr) {

    audiodevice = GetDefaultDevice();
    m_CodecData.playerThread = this;

    m_CodecData.CodecDecoder = nullptr;
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


QList<QPair<QString, int>> PortaudioThread::GetAvailableOutputDevices() {
    QList<QPair<QString, int>> deviceList;
    int quantityDevices = Pa_GetDeviceCount();
    
    if (quantityDevices < 0) {
        qCritical() << "PortAudio Error: " << Pa_GetErrorText(quantityDevices);
        return deviceList;
    }
    
    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < quantityDevices; ++i) {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxOutputChannels > 0) {
            deviceList.append(qMakePair(QString(deviceInfo->name), i));
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


void PortaudioThread::setAudioDevice(int set_audiodevice) {
    qDebug() << "Changing to:" << set_audiodevice;
    audiodevice = set_audiodevice;
    qDebug() << "setted to:" << audiodevice;
}



void PortaudioThread::StartPlayback() {
    QByteArray ba = m_filename.toLocal8Bit();
    const char *filename_c_str = ba.data();


    m_CodecData.CodecDecoder = codec_open(filename_c_str);
    if (!m_CodecData.CodecDecoder) {
        emit errorOccurred("Could not open file: " + m_filename);
        return;
    }

    codec_seek(m_CodecData.CodecDecoder, 0);


    int channels = codec_get_channels(m_CodecData.CodecDecoder);
    long totalFrames = codec_get_total_frames(m_CodecData.CodecDecoder);
    int samplerate = codec_get_samplerate(m_CodecData.CodecDecoder);

    emit totalFileInfo(static_cast<int>(totalFrames),
                       static_cast<int>(samplerate));

    if (audiodevice == -1) {
        emit errorOccurred("Failed to get default output device.");
        codec_close(m_CodecData.CodecDecoder);
        m_CodecData.CodecDecoder = nullptr;
        return;
    }

    PaStreamParameters outputParameters;
    memset(&outputParameters, 0, sizeof(outputParameters));
    outputParameters.channelCount = channels;
    outputParameters.device = audiodevice;
    outputParameters.hostApiSpecificStreamInfo = nullptr;
    outputParameters.sampleFormat = paFloat32; 
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(audiodevice)->defaultLowOutputLatency;

    PaError err = Pa_OpenStream(
        &m_stream,
        nullptr,
        &outputParameters,
        samplerate,
        framesPerBuffer_GLOBAL,
        paNoFlag,
        audio_callback,
        &m_CodecData
    );
    CheckPaError(err);
    

    err = Pa_StartStream(m_stream);
    CheckPaError(err);

    m_isRunning = true;
    m_isPaused = false;
}

void PortaudioThread::run() {
    StartPlayback();

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
    if (!m_CodecData.CodecDecoder) {
        qWarning() << "Cannot seek: no file loaded.";
        return;
    }

    long totalFrames = codec_get_total_frames(m_CodecData.CodecDecoder);
    if (totalFrames <= 0) {
        qWarning() << "Cannot seek: file has no frames.";
        return;
    }

    if (ValueInPercent < 0) {
        ValueInPercent = 0;
    } else if (ValueInPercent > 100) {
        ValueInPercent = 100;
    }


    long targetFrame = (totalFrames * ValueInPercent) / 100;

    long seek_result = codec_seek(m_CodecData.CodecDecoder, targetFrame);

    if (seek_result == -1) {
        qWarning() << "Error seeking in file via decoder.";
        emit errorOccurred("Error seeking in file.");
        return;
    }

    emit playbackProgress(
        static_cast<int>(seek_result),
        static_cast<int>(totalFrames),
        codec_get_samplerate(m_CodecData.CodecDecoder)
    );
}
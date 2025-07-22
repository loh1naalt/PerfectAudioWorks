#include "PortAudioHandler.h"
#include <QDebug>

#define GetAllAvailableDevices 0
#define GetDefaultDevice 1

static int checkFileOnErrors(SNDFILE *file) {
    if (sf_error(file) != 0) {
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

int PortaudioThread::audio_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                                    void *userData) {
    Q_UNUSED(inputBuffer);
    Q_UNUSED(timeinfo);
    Q_UNUSED(statusFlags);

    float *out = (float*)outputBuffer;
    callback_data_s *p_data = (callback_data_s*)userData;

    if (!p_data || !p_data->playerThread) {
        qCritical() << "Portaudio callback: Invalid user data or playerThread pointer!";
        return paAbort;
    }

    bool isPaused = p_data->playerThread->m_isPaused;

    memset(out, 0, sizeof(float) * framesPerBuffer * p_data->Fileinfo.channels);

    p_data->currentframe = sf_seek(p_data->file, 0, SEEK_CUR);

    emit p_data->playerThread->playbackProgress(
        static_cast<int>(p_data->currentframe),
        static_cast<int>(p_data->Fileinfo.frames),
        static_cast<int>(p_data->Fileinfo.samplerate)
    );

    if (isPaused) {
        return paContinue;
    }

    sf_count_t num_read = sf_read_float(p_data->file, out, framesPerBuffer * p_data->Fileinfo.channels);

    if (num_read < (framesPerBuffer * p_data->Fileinfo.channels)) {
        memset(out + num_read, 0, (framesPerBuffer * p_data->Fileinfo.channels - num_read) * sizeof(float));

        if (num_read == 0 && p_data->currentframe >= p_data->Fileinfo.frames) {
            emit p_data->playerThread->playbackFinished();
            return paComplete;
        }
    }
    return paContinue;
}

int PortaudioThread::Portaudiohandler(int calltype) {
    int quantityDevices = Pa_GetDeviceCount();
    qDebug() << "Devices detected:" << quantityDevices;

    if (quantityDevices < 0) {
        emit errorOccurred("Error getting device count: " + QString(Pa_GetErrorText(quantityDevices)));
        return -1;
    } else if (quantityDevices == 0) {
        qDebug() << "No devices detected on this machine.";
        return -1;
    }

    switch (calltype) {
        case GetAllAvailableDevices: {
            const PaDeviceInfo* deviceInfo;
            for (int i = 0; i < quantityDevices; i++) {
                deviceInfo = Pa_GetDeviceInfo(i);
                qDebug() << "Device" << i << ":";
                qDebug() << "    name:" << deviceInfo->name;
                qDebug() << "    maxInputChannels:" << deviceInfo->maxInputChannels;
                qDebug() << "    maxOutputChannels:" << deviceInfo->maxOutputChannels;
                qDebug() << "    defaultSampleRate:" << deviceInfo->defaultSampleRate;
            }
            return 0;
        }
        case GetDefaultDevice: {
            PaDeviceIndex outputDevice = Pa_GetDefaultOutputDevice();
            if (outputDevice == paNoDevice) {
                emit errorOccurred("No default output device found.");
                return -1;
            }
            const PaDeviceInfo *infoO = Pa_GetDeviceInfo(outputDevice);
            qDebug() << "Default output Device:" << infoO->name;
            return outputDevice;
        }
        default:
            return -1;
    }
}

PortaudioThread::PortaudioThread(QObject *parent)
    : QThread(parent), m_isRunning(false), m_isPaused(false), m_stream(nullptr) {
    memset(&m_filedata, 0, sizeof(m_filedata));
    m_filedata.playerThread = this;
}

PortaudioThread::~PortaudioThread() {
    stopPlayback();
}

void PortaudioThread::PaInit() {
    PaError err = Pa_Initialize();
    CheckPaError(err);
    qDebug() << "PortAudio Initialized.";
}

void PortaudioThread::setFile(const QString &filename) {
    m_filename = filename;
    qDebug() << "File set to:" << m_filename;
}

void PortaudioThread::StartPlayback() {
    PaError err;

    if (m_filedata.file) {
        sf_close(m_filedata.file);
        m_filedata.file = nullptr;
    }

    QByteArray ba = m_filename.toLocal8Bit();
    const char *filename_c_str = ba.data();

    m_filedata.file = sf_open(filename_c_str, SFM_READ, &m_filedata.Fileinfo);
    if (!m_filedata.file || checkFileOnErrors(m_filedata.file) != 0) {
        emit errorOccurred("Could not open file: " + m_filename + " - " + QString(sf_strerror(m_filedata.file)));
        return;
    }

    qDebug() << "File opened:" << m_filename;
    qDebug() << "Channels:" << m_filedata.Fileinfo.channels
             << ", Sample Rate:" << m_filedata.Fileinfo.samplerate
             << ", Frames:" << m_filedata.Fileinfo.frames;

    emit totalFileInfo(static_cast<int>(m_filedata.Fileinfo.frames),
                       static_cast<int>(m_filedata.Fileinfo.samplerate));

    const int OutputDevice = Portaudiohandler(GetDefaultDevice);
    if (OutputDevice == -1) {
        emit errorOccurred("Failed to get default output device.");
        sf_close(m_filedata.file);
        m_filedata.file = nullptr;
        return;
    }

    PaStreamParameters outputParameters;
    memset(&outputParameters, 0, sizeof(outputParameters));
    outputParameters.channelCount = m_filedata.Fileinfo.channels;
    outputParameters.device = OutputDevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(OutputDevice)->defaultLowOutputLatency;


    err = Pa_OpenStream(
        &m_stream,
        0,
        &outputParameters,
        m_filedata.Fileinfo.samplerate,
        BitsPerSample,
        paNoFlag,
        audio_callback,
        &m_filedata
    );
    CheckPaError(err);

    err = Pa_StartStream(m_stream);
    CheckPaError(err);

    m_isRunning = true;
    m_isPaused = false;
    qDebug() << "Playback started.";
}

void PortaudioThread::run() {
    StartPlayback();

    while (m_stream && Pa_IsStreamActive(m_stream) == 1 && m_isRunning) {
        QThread::msleep(50);
    }

    qDebug() << "PortaudioThread run() finished.";

    if (m_stream) {
        PaError err = paNoError;
        if (Pa_IsStreamActive(m_stream) == 1) {
            err = Pa_StopStream(m_stream);
            if (err != paNoError) qDebug() << "Error stopping stream:" << Pa_GetErrorText(err);
        }
        err = Pa_CloseStream(m_stream);
        if (err != paNoError) qDebug() << "Error closing stream:" << Pa_GetErrorText(err);
        m_stream = nullptr;
    }
    if (m_filedata.file) {
        sf_close(m_filedata.file);
        m_filedata.file = nullptr;
    }

    Pa_Terminate();
}

void PortaudioThread::stopPlayback() {
    if (!m_isRunning) return;

    qDebug() << "Stopping playback...";
    m_isRunning = false;

    wait();
}

void PortaudioThread::setPlayPause() {
    m_isPaused = !m_isPaused;
    qDebug() << "Playback is now" << (m_isPaused ? "PAUSED" : "PLAYING");
}

bool PortaudioThread::isPaused() const {
    return m_isPaused;
}

void PortaudioThread::SetFrameFromTimeline(int ValueInPercent) {
    if (!m_filedata.file || m_filedata.Fileinfo.frames <= 0) {
        qWarning() << "Cannot seek: no file loaded or file has no frames.";
        return;
    }

    float percentage = ValueInPercent / 100.0f;
    sf_count_t targetFrame = static_cast<sf_count_t>(percentage * m_filedata.Fileinfo.frames);

    sf_count_t seek_result = sf_seek(m_filedata.file, targetFrame, SEEK_SET);

    if (seek_result == -1) {
        qWarning() << "Error seeking in file:" << sf_strerror(m_filedata.file);
        emit errorOccurred("Error seeking in file: " + QString(sf_strerror(m_filedata.file)));
    } else {
        m_filedata.currentframe = seek_result;
        qDebug() << "Seeked to frame:" << m_filedata.currentframe << "(" << ValueInPercent << "%)";
        emit playbackProgress(
            static_cast<int>(m_filedata.currentframe),
            static_cast<int>(m_filedata.Fileinfo.frames),
            static_cast<int>(m_filedata.Fileinfo.samplerate)
        );
    }
}
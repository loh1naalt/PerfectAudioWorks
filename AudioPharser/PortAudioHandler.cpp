#include "PortAudioHandler.h"
#include <QDebug>
#include <QByteArray> 


#define framesPerBuffer_GLOBAL 512 

#define GetAllAvailableDevices 0
#define GetDefaultDevice 1


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
                if (deviceInfo) {
                    qDebug() << "Device" << i << ":";
                    qDebug() << "    name:" << deviceInfo->name;
                    qDebug() << "    maxInputChannels:" << deviceInfo->maxInputChannels;
                    qDebug() << "    maxOutputChannels:" << deviceInfo->maxOutputChannels;
                    qDebug() << "    defaultSampleRate:" << deviceInfo->defaultSampleRate;
                }
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
            if (infoO) {
                 qDebug() << "Default output Device:" << infoO->name;
                 return outputDevice;
            } else {
                 emit errorOccurred("Failed to get info for default output device.");
                 return -1;
            }
        }
        default:
            qWarning() << "Unknown calltype for Portaudiohandler:" << calltype;
            return -1;
    }
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


void PortaudioThread::PaInit() {
    PaError err = Pa_Initialize();
    CheckPaError(err);
}


void PortaudioThread::setFile(const QString &filename) {
    m_filename = filename;
}


void PortaudioThread::StartPlayback() {
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

    const int OutputDevice = Portaudiohandler(GetDefaultDevice);
    if (OutputDevice == -1) {
        emit errorOccurred("Failed to get default output device.");
        m_sndFileDecoder.SndfileCloseFile(); // Close file if device not found
        return;
    }

    PaStreamParameters outputParameters;
    memset(&outputParameters, 0, sizeof(outputParameters));
    outputParameters.channelCount = fileInfo.channels;
    outputParameters.device = OutputDevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(OutputDevice)->defaultLowOutputLatency;

    // Open PortAudio stream
    err = Pa_OpenStream(
        &m_stream,
        0, // No input
        &outputParameters,
        fileInfo.samplerate,
        framesPerBuffer_GLOBAL, // Use the defined framesPerBuffer_GLOBAL
        paNoFlag,
        audio_callback,
        &m_SndFileData // Pass pointer to SndfileCallback struct
    );
    CheckPaError(err);

    // Start PortAudio stream
    err = Pa_StartStream(m_stream);
    CheckPaError(err);

    m_isRunning = true;
    m_isPaused = false;
}

// The thread's main execution loop
void PortaudioThread::run() {
    StartPlayback();

    // Loop while stream is active and playback is intended to run
    while (m_stream && Pa_IsStreamActive(m_stream) == 1 && m_isRunning) {
        QThread::msleep(50);
    }

    // Clean up stream resources after loop finishes
    if (m_stream) {
        PaError err = paNoError;
        if (Pa_IsStreamActive(m_stream) == 1) { // Check if still active before stopping
            err = Pa_StopStream(m_stream);
            if (err != paNoError) qWarning() << "Error stopping stream:" << Pa_GetErrorText(err);
        }
        err = Pa_CloseStream(m_stream);
        if (err != paNoError) qWarning() << "Error closing stream:" << Pa_GetErrorText(err);
        m_stream = nullptr;
    }
    // m_sndFileDecoder's destructor will handle closing the file when the PortaudioThread is destroyed.
    // If you need to explicitly close the file when playback ends but the thread persists,
    // call m_sndFileDecoder.SndfileCloseFile(); here.
    // For now, assuming it's okay for the decoder to remain open until thread destruction or new file.
}

// Stops playback and waits for the thread to finish
void PortaudioThread::stopPlayback() {
    if (!m_isRunning) return;

    m_isRunning = false; // Signal the thread to stop

    // Important: Wait for the run() method to complete its cleanup.
    // Without wait(), the thread might still be active when the destructor is called,
    // leading to potential race conditions or ungraceful termination.
    wait();
}

// Toggles playback pause/resume
void PortaudioThread::setPlayPause() {
    m_isPaused = !m_isPaused;
}

// Checks if playback is paused
bool PortaudioThread::isPaused() const {
    return m_isPaused;
}

// Seeks to a specific position in the audio file
void PortaudioThread::SetFrameFromTimeline(int ValueInPercent) {
    // Get file info from the decoder
    const SF_INFO& fileInfo = m_sndFileDecoder.getFileInfo();

    if (!m_sndFileDecoder.getLogicalCurrentFrame() || fileInfo.frames <= 0) { // Check if a file is logically open/valid
        qWarning() << "Cannot seek: no file loaded or file has no frames.";
        return;
    }

    float percentage = ValueInPercent / 100.0f;
    sf_count_t targetFrame = static_cast<sf_count_t>(percentage * fileInfo.frames);

    // Use SndFileDecoder's SetSampleTo method
    sf_count_t seek_result = m_sndFileDecoder.SetSampleTo(targetFrame);

    if (seek_result == -1) {
        // Error message should come from SndFileDecoder, or be more generic
        qWarning() << "Error seeking in file via decoder.";
        emit errorOccurred("Error seeking in file."); // Generic error message
    } else {
        // Emit playback progress with the updated position from the decoder
        emit playbackProgress(
            static_cast<int>(seek_result), // seek_result is the new current frame
            static_cast<int>(fileInfo.frames),
            static_cast<int>(fileInfo.samplerate)
        );
    }
}
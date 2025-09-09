#include "PortAudioHandler.h"
#include <QTimer>

PortaudioThread::PortaudioThread(QObject* parent)
    : QThread(parent), audiodevice(-1), m_isPaused(false), m_isRunning(false)
{
    audio_init();
    memset(&m_player, 0, sizeof(m_player));
}

PortaudioThread::~PortaudioThread() {
    stopPlayback();
    audio_terminate();
}

QList<QPair<QString, int>> PortaudioThread::GetAllAvailableOutputDevices() {
    QList<QPair<QString, int>> deviceList;
    int quantityDevices = Pa_GetDeviceCount();

    
    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < quantityDevices; ++i) {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxOutputChannels > 0) {
            deviceList.append(qMakePair(QString(deviceInfo->name), i));
        }
    }
    
    return deviceList;
}

void PortaudioThread::setFile(const QString& filename) {
    m_filename = filename;
}

void PortaudioThread::setAudioDevice(int device) {
    audiodevice = device;
}

void PortaudioThread::StartPlayback() {
    if (!m_filename.isEmpty()) {
        if (audio_play(&m_player, m_filename.toLocal8Bit().data(), audiodevice) != 0) {
            emit errorOccurred("Failed to start playback");
            return;
        }
        m_isRunning = true;
        emit totalFileInfo((int)m_player.totalFrames, m_player.channels, m_player.samplerate);
    }
}

void PortaudioThread::run() {
    StartPlayback();
    if (!m_player.stream) return;

    const double streamStartTime = Pa_GetStreamTime(m_player.stream);
    m_isRunning = true;

    while (m_isRunning && Pa_IsStreamActive(m_player.stream)) {
        double currentTime = Pa_GetStreamTime(m_player.stream) - streamStartTime;


        long currentFrame = static_cast<long>(currentTime * m_player.samplerate);
        if (currentFrame > m_player.totalFrames) currentFrame = m_player.totalFrames;

        m_player.currentFrame = currentFrame;

        emitProgress();

        QThread::msleep(50); 
    }

    m_player.currentFrame = codec_get_current_frame(m_player.codec);
    emitProgress();

    emit playbackFinished();
    audio_stop(&m_player);
    m_isRunning = false;
}

void PortaudioThread::emitProgress() {
    if (!m_player.codec) return;

    emit playbackProgress(
        static_cast<int>(m_player.currentFrame),
        static_cast<int>(m_player.totalFrames),
        m_player.samplerate
    );
}


void PortaudioThread::stopPlayback() {
    m_isRunning = false;
    wait();
    audio_stop(&m_player);
}

void PortaudioThread::setPlayPause() {
    m_isPaused = !m_isPaused;
    audio_pause(&m_player, m_isPaused ? 1 : 0);
}

bool PortaudioThread::isPaused() const {
    return m_isPaused;
}

void PortaudioThread::SetFrameFromTimeline(int percent) {
    if (!m_player.codec) return;
    long frame = (m_player.totalFrames * percent) / 100;
    audio_seek(&m_player, frame);
}

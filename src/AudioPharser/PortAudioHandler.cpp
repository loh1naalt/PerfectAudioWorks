#include "PortAudioHandler.h"


PortaudioThread::PortaudioThread(QObject* parent)
    : QThread(parent),
      audiodevice(-1),
      m_isPaused(false),
      m_isRunning(false),
      m_streamStartTime(0.0)
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

    for (int i = 0; i < quantityDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
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
    if (m_filename.isEmpty()) return;

    if (audio_play(&m_player, m_filename.toLocal8Bit().data(), audiodevice) != 0) {
        emit errorOccurred("Failed to start playback");
        return;
    }

    m_isRunning = true;
    m_streamStartTime = Pa_GetStreamTime(m_player.stream) - ((double)m_player.currentFrame / m_player.samplerate);

    emit totalFileInfo((int)m_player.totalFrames, m_player.channels, m_player.samplerate, m_player.CodecName);
}

void PortaudioThread::run() {
    StartPlayback();

    while (m_player.stream && Pa_IsStreamActive(m_player.stream) && m_isRunning) {
        if (!m_isPaused && m_player.codec) {
            double streamTime = Pa_GetStreamTime(m_player.stream) - m_streamStartTime;
            long frameFromTime = static_cast<long>(std::round(streamTime * m_player.samplerate));

            if (frameFromTime != m_player.currentFrame) {
                m_player.currentFrame = frameFromTime;
                emitProgress();
            }
        }

        QThread::msleep(10);
    }

    emitProgress();
    emit playbackFinished();

    audio_stop(&m_player);
    m_isRunning = false;
}

void PortaudioThread::stopPlayback() {
    m_isRunning = false;
    wait();
    audio_stop(&m_player);
}

void PortaudioThread::setPlayPause() {
    m_isPaused = !m_isPaused;
    audio_pause(&m_player, m_isPaused ? 1 : 0);

    if (!m_isPaused && m_player.codec && m_player.stream) {
        // Adjust stream start time after unpausing
        m_streamStartTime = Pa_GetStreamTime(m_player.stream) - ((double)m_player.currentFrame / m_player.samplerate);
    }
}

bool PortaudioThread::isPaused() const {
    return m_isPaused;
}

void PortaudioThread::SetFrameFromTimeline(int percent) {
    if (!m_player.codec || !m_player.stream) return;

    long frame = (m_player.totalFrames * percent) / 100;

    audio_seek(&m_player, frame);

    m_streamStartTime = Pa_GetStreamTime(m_player.stream) - (double)frame / m_player.samplerate;

    m_player.currentFrame = frame;

    emitProgress();
}

void PortaudioThread::emitProgress() {
    if (!m_player.codec) return;
    emit playbackProgress((int)m_player.currentFrame, (int)m_player.totalFrames, m_player.samplerate);
}

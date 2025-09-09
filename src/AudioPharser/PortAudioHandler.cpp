#include "PortAudioHandler.h"
#include "portaudio_thread.h"
#include <cmath>

PortaudioThread::PortaudioThread(QObject* parent)
    : QThread(parent),
      audiodevice(-1),
      m_isPaused(false),
      m_isRunning(false),
      m_playRequested(false),
      m_streamStartTime(0.0)
{
    audio_init();
    memset(&m_player, 0, sizeof(m_player));
}

PortaudioThread::~PortaudioThread() {
    stopPlayback();
    m_waitCondition.wakeAll();
    wait();
    audio_terminate();
}

void PortaudioThread::setAudioDevice(int device) {
    audiodevice = device;
}

void PortaudioThread::setFile(const QString &filename) {
    QMutexLocker locker(&m_mutex);
    m_filename = filename;
    m_playRequested = true;
    m_waitCondition.wakeAll();
}

void PortaudioThread::queueFile(const QString &filename) {
    QMutexLocker locker(&m_mutex);
    m_nextFile = filename;
}

void PortaudioThread::StartPlayback() {
    QMutexLocker locker(&m_mutex);
    m_playRequested = true;
    m_waitCondition.wakeAll();
}

void PortaudioThread::stopPlayback() {
    QMutexLocker locker(&m_mutex);
    m_isRunning = false;
    m_playRequested = false;
    m_waitCondition.wakeAll();
}

void PortaudioThread::setPlayPause() {
    QMutexLocker locker(&m_mutex);
    m_isPaused = !m_isPaused;
    audio_pause(&m_player, m_isPaused ? 1 : 0);

    if (!m_isPaused && m_player.codec && m_player.stream) {
        m_streamStartTime = Pa_GetStreamTime(m_player.stream) - ((double)m_player.currentFrame / m_player.samplerate);
    }
}

bool PortaudioThread::isPaused() const {
    QMutexLocker locker(&m_mutex);
    return m_isPaused;
}

void PortaudioThread::SetFrameFromTimeline(int percent) {
    QMutexLocker locker(&m_mutex);
    if (!m_player.codec || !m_player.stream) return;

    long frame = (m_player.totalFrames * percent) / 100;
    audio_seek(&m_player, frame);
    m_player.currentFrame = frame;
    m_streamStartTime = Pa_GetStreamTime(m_player.stream) - (double)frame / m_player.samplerate;

    emitProgress();
}

void PortaudioThread::emitProgress() {
    if (!m_player.codec) return;
    emit playbackProgress((int)m_player.currentFrame, (int)m_player.totalFrames, m_player.samplerate);
}

void PortaudioThread::run() {
    while (true) {
        QMutexLocker locker(&m_mutex);
        if (!m_playRequested) {
            m_waitCondition.wait(&m_mutex);
        }

        if (!m_playRequested) continue;
        QString fileToPlay = m_filename;
        m_playRequested = false;
        m_isRunning = true;
        locker.unlock();

        playFile(fileToPlay);

        locker.relock();
        if (!m_nextFile.isEmpty()) {
            m_filename = m_nextFile;
            m_nextFile.clear();
            m_playRequested = true;
            locker.unlock();
            continue;  // auto-play queued file
        }
        m_isRunning = false;
    }
}

void PortaudioThread::playFile(const QString &filename) {
    if (filename.isEmpty()) return;

    if (audio_play(&m_player, filename.toLocal8Bit().data(), audiodevice) != 0) {
        emit errorOccurred("Failed to start playback");
        return;
    }

    m_streamStartTime = Pa_GetStreamTime(m_player.stream) - ((double)m_player.currentFrame / m_player.samplerate);
    emit totalFileInfo((int)m_player.totalFrames, m_player.channels, m_player.samplerate, m_player.CodecName);

    while (m_player.stream && Pa_IsStreamActive(m_player.stream) && m_isRunning) {
        {
            QMutexLocker locker(&m_mutex);
            if (m_isPaused) {
                locker.unlock();
                QThread::msleep(10);
                continue;
            }
        }

        double streamTime = Pa_GetStreamTime(m_player.stream) - m_streamStartTime;
        long frameFromTime = static_cast<long>(std::round(streamTime * m_player.samplerate));

        if (frameFromTime != m_player.currentFrame) {
            m_player.currentFrame = frameFromTime;
            emitProgress();
        }

        QThread::msleep(10);
    }

    emitProgress();
    emit playbackFinished();
    audio_stop(&m_player);
}

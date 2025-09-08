#pragma once
#include <QThread>
#include <QString>
#include <QTimer>
#include <QDebug>
#include "portaudio_backend.h"

class PortaudioThread : public QThread {
    Q_OBJECT
public:
    explicit PortaudioThread(QObject* parent = nullptr);
    ~PortaudioThread() override;

    void setFile(const QString& filename);
    void setAudioDevice(int device);
    void StartPlayback();
    void stopPlayback();
    void setPlayPause();
    bool isPaused() const;
    void SetFrameFromTimeline(int percent);
    QList<QPair<QString, int>> GetAllAvailableOutputDevices();

signals:
    void errorOccurred(const QString&);
    void playbackProgress(int current, int total, int samplerate);
    void playbackFinished();
    void totalFileInfo(int totalFrames, int samplerate);

protected:
    void run() override;

private:
    QString m_filename;
    int audiodevice;
    AudioPlayer m_player;
    bool m_isPaused;
    bool m_isRunning;

    QTimer* m_updateTimer;

    void emitProgress();
};

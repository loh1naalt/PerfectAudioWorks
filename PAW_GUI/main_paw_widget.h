#ifndef MAIN_PAW_WIDGET_H
#define MAIN_PAW_WIDGET_H

#include <QMainWindow>

#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QMessageBox> 

#include "../AudioPharser/PortAudioHandler.h" 

QT_BEGIN_NAMESPACE
namespace Ui {
class Main_PAW_widget;
}
QT_END_NAMESPACE


class Main_PAW_widget : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_PAW_widget(QWidget *parent = nullptr); 
    ~Main_PAW_widget() override; 


    void start_playback(const QString &filename);

    
    PortaudioThread& getAudioThread() { return m_audiothread; }


private slots:
   
    void on_actionopen_file_triggered();
    void onSliderValueChanged(int value);
    void PlayPauseButton();

    
    void handlePlaybackProgress(int currentFrame, int totalFrames, int sampleRate);
    void handleTotalFileInfo(int totalFrames, int sampleRate);
    void handlePlaybackFinished();
    void handleError(const QString &errorMessage);

private:
    QTimer *m_updateTimer; 
    PortaudioThread m_audiothread; 
    QString m_currentFile; 

    
    QString floatToMMSS(float totalSeconds);

    Ui::Main_PAW_widget *ui; 
};
#endif 
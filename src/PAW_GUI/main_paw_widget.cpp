#include "main_paw_widget.h"
#include "ui_main_paw_widget.h"
#include <cmath> 

Main_PAW_widget::Main_PAW_widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_PAW_widget)
{
    ui->setupUi(this);

    
    connect(ui->TimelineSlider, &QSlider::valueChanged, this, &Main_PAW_widget::onSliderValueChanged);
    connect(ui->PlayPause, &QPushButton::clicked, this, &Main_PAW_widget::PlayPauseButton);
    connect(ui->actionSettings, &QAction::triggered, this, &Main_PAW_widget::openSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &Main_PAW_widget::openAbout);

    connect(&m_audiothread, &PortaudioThread::playbackProgress, this, &Main_PAW_widget::handlePlaybackProgress);
    connect(&m_audiothread, &PortaudioThread::totalFileInfo, this, &Main_PAW_widget::handleTotalFileInfo);
    connect(&m_audiothread, &PortaudioThread::playbackFinished, this, &Main_PAW_widget::handlePlaybackFinished);
    connect(&m_audiothread, &PortaudioThread::errorOccurred, this, &Main_PAW_widget::handleError);

   
    m_updateTimer = new QTimer(this);


    ui->TimelineSlider->setRange(0, 100); 
    ui->TimelineSlider->setValue(0);
    ui->CurrentFileDuration->setText("00:00");


    m_audiothread.PaInit();
}

Main_PAW_widget::~Main_PAW_widget()
{
   
    m_audiothread.stopPlayback(); 
    delete ui;
}

void Main_PAW_widget::start_playback(const QString &filename) {
    
    if (m_audiothread.isRunning()) {
        m_audiothread.stopPlayback();
    }

    m_currentFile = filename;
    m_audiothread.setFile(m_currentFile);
    ui->Filename->setText(m_currentFile.section('/', -1));

    
    m_audiothread.start(); 
    m_updateTimer->start(100); 
}


void Main_PAW_widget::handlePlaybackProgress(int currentFrame, int totalFrames, int sampleRate) {
    if (totalFrames > 0 && sampleRate > 0) {
        float framesInPercentage = (currentFrame * 1.0f) / totalFrames * 100.0f;
        float currentDuration = (currentFrame * 1.0f) / sampleRate;


        bool oldBlockState = ui->TimelineSlider->blockSignals(true);
        ui->TimelineSlider->setValue(static_cast<int>(framesInPercentage));
        ui->TimelineSlider->blockSignals(oldBlockState); 

        ui->CurrentFileDuration->setText(floatToMMSS(currentDuration));
    }
}


void Main_PAW_widget::handleTotalFileInfo(int totalFrames, int sampleRate) {
    if (totalFrames > 0 && sampleRate > 0) {
        float totalDuration = (totalFrames * 1.0f) / sampleRate;
        ui->TotalFileDuration->setText(floatToMMSS(totalDuration));
        ui->SampleRate->setText(QString::number(sampleRate)); 
    }
}


void Main_PAW_widget::handlePlaybackFinished() {
    m_updateTimer->stop(); 
    ui->TimelineSlider->setValue(100); 
    ui->CurrentFileDuration->setText(ui->TotalFileDuration->text()); 
    qDebug() << "Playback finished.";
}


void Main_PAW_widget::on_actionopen_file_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Audio File", "", "Audio Files (*.wav *.flac *.ogg *.opus);;Compressed audio files(*.mp3);;All Files (*)");
    if (!filename.isEmpty()) {
        start_playback(filename); 
    }
}


void Main_PAW_widget::onSliderValueChanged(int value) {

    m_audiothread.SetFrameFromTimeline(value);
}


void Main_PAW_widget::PlayPauseButton() {
    m_audiothread.setPlayPause();
    if (m_audiothread.isPaused()) {
        ui->PlayPause->setText("|>"); 
        m_updateTimer->stop(); 
    } else {
        ui->PlayPause->setText("||"); 
        m_updateTimer->start(100); 
    }
}


void Main_PAW_widget::handleError(const QString &errorMessage) {
    QMessageBox::critical(this, "Audio Playback Error", errorMessage);
    m_audiothread.stopPlayback(); 
    handlePlaybackFinished(); 
}

void Main_PAW_widget::openSettings(){
    s.show();
}
void Main_PAW_widget::openAbout(){
    about.show();
}


QString Main_PAW_widget::floatToMMSS(float totalSeconds) {
    bool isNegative = totalSeconds < 0;
    totalSeconds = std::abs(totalSeconds);

    int minutes = static_cast<int>(totalSeconds / 60.0f);
    float remainingSeconds = totalSeconds - (minutes * 60.0f);
    int seconds = static_cast<int>(std::round(remainingSeconds)); // Round to nearest second


    if (seconds == 60) {
        seconds = 0;
        minutes++;
    }

    return QStringLiteral("%1%2:%3")
           .arg(isNegative ? "-" : "")
           .arg(minutes, 2, 10, QChar('0'))
           .arg(seconds, 2, 10, QChar('0'));
}
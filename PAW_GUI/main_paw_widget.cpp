#include "main_paw_widget.h"
#include "ui_main_paw_widget.h"



Main_PAW_widget::Main_PAW_widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_PAW_widget)
{
    ui->setupUi(this);
    connect(ui->TimelineSlider, &QSlider::valueChanged, this, &Main_PAW_widget::onSliderValueChanged);
}

Main_PAW_widget::~Main_PAW_widget()
{
    delete ui;
}


void Main_PAW_widget::start_playback(char *filename){
    timer = new QTimer(this);
    Audiothread.setFile(filename);
    ui->Filename->setText(filename);
    connect(timer, &QTimer::timeout, this, &Main_PAW_widget::updateSlider);

    Audiothread.start();

    timer->start(100);
    updateSlider();
}

void Main_PAW_widget::on_actionopen_file_triggered()
{

    QString filename = QFileDialog::getOpenFileName(this, "Open the file");
    QByteArray ba = filename.toLocal8Bit();
    char *filename_std = ba.data();
    start_playback(filename_std);

}

void Main_PAW_widget::updateSlider()
{
    std::map<std::string, int> Fileinfo = Audiothread.FileInfoDict;

    float framesInPercentage = (Fileinfo["CurrentFrame"] * 1.0f) / Fileinfo["TotalFrames"] * 100.0f;
    float CurrentDuration = (Fileinfo["CurrentFrame"] * 1.0f) / Fileinfo["SampleRate"];
    float TotalDuration = (Fileinfo["TotalFrames"] * 1.0f) / Fileinfo["SampleRate"];


    ui->CurrentFileDuration->setText(floatToMMSS(CurrentDuration));
    ui->TotalFileDuration->setText(floatToMMSS(TotalDuration));
    ui->TimelineSlider->setValue(framesInPercentage);
}

void Main_PAW_widget::onSliderValueChanged(int value){
    Audiothread.SetFrameFromTimeline(value);
}

QString Main_PAW_widget::floatToMMSS(float totalSeconds) {
    bool isNegative = totalSeconds < 0;
    totalSeconds = std::abs(totalSeconds);

    int minutes = static_cast<int>(totalSeconds / 60.0f);
    int seconds = static_cast<int>(totalSeconds) % 60; 
    int milliseconds = static_cast<int>((totalSeconds - (minutes * 60 + seconds)) * 1000); 


    if (milliseconds >= 500) {
        seconds++;
        if (seconds == 60) {
            seconds = 0;
            minutes++;
        }
    }

    return QStringLiteral("%1%2:%3")
           .arg(isNegative ? "-" : "")
           .arg(minutes, 2, 10, QChar('0'))
           .arg(seconds, 2, 10, QChar('0')); 
}
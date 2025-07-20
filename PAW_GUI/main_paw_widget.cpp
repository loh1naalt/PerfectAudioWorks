#include "main_paw_widget.h"
#include "ui_main_paw_widget.h"



Main_PAW_widget::Main_PAW_widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_PAW_widget)
{
    ui->setupUi(this);
}

Main_PAW_widget::~Main_PAW_widget()
{
    delete ui;
}


void Main_PAW_widget::start_playback(char *filename){
    timer = new QTimer(this);
    Audiothread.setFile(filename);
    ui->Filename->setText(filename);

    Audiothread.start();

    connect(timer, &QTimer::timeout, this, &Main_PAW_widget::updateSlider);
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
    ui->TimelineSlider->setValue(framesInPercentage);
}

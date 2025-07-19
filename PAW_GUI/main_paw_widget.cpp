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
    callback_data_s file_info;
    Pa_info pa_info;
    // std::string CharCurrentFrame;
    // CharCurrentFrame = std::to_string(file_info.currentframe);

    Audiothread.setFile(filename);
    Audiothread.SetSlider(ui->TimelineSlider);
    ui->Filename->setText(filename);

    Audiothread.start();
}

void Main_PAW_widget::on_actionopen_file_triggered()
{

    QString filename = QFileDialog::getOpenFileName(this, "Open the file");
    QByteArray ba = filename.toLocal8Bit();
    char *filename_std = ba.data();
    start_playback(filename_std);

}


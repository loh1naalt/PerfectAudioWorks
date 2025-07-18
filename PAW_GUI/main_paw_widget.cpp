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

// void Main_PAW_widget::StartPlaybackSlot(QString filename){
//     QByteArray ba = filename.toLocal8Bit();
//     char *filename_std = ba.data(); 
//     StartPlayback(filename_std);
// }

void Main_PAW_widget::on_actionopen_file_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open the file");
    QByteArray ba = filename.toLocal8Bit();
    char *filename_std = ba.data();
    thread.setFile(filename_std);
    thread.start();

}


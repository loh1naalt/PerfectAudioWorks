#ifndef MAIN_PAW_WIDGET_H
#define MAIN_PAW_WIDGET_H

#include <string>
#include <QMainWindow>
#include <QWidget>
#include <QFileDialog>
#include <QThread>
#include <QDebug>
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
    Main_PAW_widget(QWidget *parent = nullptr);
    ~Main_PAW_widget();

    void start_playback(char *filename);
    PortaudioThread thread;

private slots:
    void on_actionopen_file_triggered();
    // void StartPlaybackSlot(QString filename);

private:
    Ui::Main_PAW_widget *ui;
    QString currentFile = "";
};
#endif // MAIN_PAW_WIDGET_H

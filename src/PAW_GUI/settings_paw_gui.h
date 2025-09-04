#ifndef Settings_PAW_gui_H
#define Settings_PAW_gui_H

#include <QMainWindow>

#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QMessageBox> 
#include <QComboBox>

#include "../AudioPharser/PortAudioHandler.h" 

QT_BEGIN_NAMESPACE
namespace Ui {
class Settings_PAW_gui;
}
QT_END_NAMESPACE


class Settings_PAW_gui : public QMainWindow
{
        Q_OBJECT

    public:
        
        explicit Settings_PAW_gui(PortaudioThread* audioThread ,QWidget *parent = nullptr);
        ~Settings_PAW_gui() override;


    private:
        // int CurrentSettingsEntry;
        PortaudioThread* m_audiothread; 
        Ui::Settings_PAW_gui *ui;
        QComboBox* audioDeviceComboBox = new QComboBox();

    private slots:

    void showAudioSettings();
    void applySettings();
    /*disabled for a while
    void showUISettings();
    void showPluginsSettings();
    */
};
#endif 
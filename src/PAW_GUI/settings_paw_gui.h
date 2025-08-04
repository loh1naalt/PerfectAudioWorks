#ifndef Settings_PAW_gui_H
#define Settings_PAW_gui_H

#include <QMainWindow>

#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QMessageBox> 

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
        struct AudioSettings
        {
            std::string* audiodevice;
            int* bitspersecond;
        };
        
        explicit Settings_PAW_gui(QWidget *parent = nullptr);
        ~Settings_PAW_gui() override;


    private:
        // int CurrentSettingsEntry;

        Ui::Settings_PAW_gui *ui; 

    private slots:

    void showAudioSettings();
    void showUISettings();
    void showPluginsSettings();
};
#endif 
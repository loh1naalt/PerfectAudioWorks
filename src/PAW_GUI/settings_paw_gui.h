#ifndef Settings_PAW_gui_H
#define Settings_PAW_gui_H

#include <QMainWindow>

#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QMessageBox> 


QT_BEGIN_NAMESPACE
namespace Ui {
class Settings_PAW_gui;
}
QT_END_NAMESPACE


class Settings_PAW_gui : public QMainWindow
{
        Q_OBJECT

    public:
        explicit Settings_PAW_gui(QWidget *parent = nullptr);
        ~Settings_PAW_gui() override;


    private:

        Ui::Settings_PAW_gui *ui; 
};
#endif 
#ifndef ABOUT_PAW_GUI_H
#define ABOUT_PAW_GUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class About_PAW_gui;
}
QT_END_NAMESPACE


class About_PAW_gui : public QMainWindow
{
        Q_OBJECT

    public:
        
        explicit About_PAW_gui(QWidget *parent = nullptr);
        ~About_PAW_gui() override;


    private:

        Ui::About_PAW_gui *ui; 

    // private slots:
};
#endif 
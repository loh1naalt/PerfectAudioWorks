#ifndef MAIN_PAW_WIDGET_H
#define MAIN_PAW_WIDGET_H

#include <QMainWindow>

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

private:
    Ui::Main_PAW_widget *ui;
};
#endif // MAIN_PAW_WIDGET_H

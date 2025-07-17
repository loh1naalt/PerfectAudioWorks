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

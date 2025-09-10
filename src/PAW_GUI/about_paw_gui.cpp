#include "about_paw_gui.h"
#include "ui_about_paw_gui.h"
#include <QLabel>

About_PAW_gui::About_PAW_gui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::About_PAW_gui)
{
    ui->setupUi(this);


    // QLabel* extraLabel = new QLabel("Extra dynamic info", this);
    // ui->dynamicLayout->addWidget(extraLabel);
}

About_PAW_gui::~About_PAW_gui()
{
    delete ui;
}

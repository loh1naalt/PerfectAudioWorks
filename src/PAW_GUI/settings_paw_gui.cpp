#include "settings_paw_gui.h" // Correct include
#include "ui_settings_paw_gui.h" 


Settings_PAW_gui::Settings_PAW_gui(QWidget *parent) // Correct class name
    : QMainWindow(parent)
    , ui(new Ui::Settings_PAW_gui) // Correct class name
{
    ui->setupUi(this);
}

Settings_PAW_gui::~Settings_PAW_gui() // Correct class name
{
    delete ui;
}
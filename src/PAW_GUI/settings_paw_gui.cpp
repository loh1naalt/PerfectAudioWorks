#include "settings_paw_gui.h" // Correct include
#include "ui_settings_paw_gui.h" 


Settings_PAW_gui::Settings_PAW_gui(QWidget *parent) // Correct class name
    : QMainWindow(parent)
    , ui(new Ui::Settings_PAW_gui) // Correct class name
{
    ui->setupUi(this);

    connect(ui->AudioSettings, &QCommandLinkButton::clicked, this, &Settings_PAW_gui::showAudioSettings);
    /* disabled for a while
    connect(ui->UserInterface, &QCommandLinkButton::clicked, this, &Settings_PAW_gui::showUISettings);
    connect(ui->PluginsSettings, &QCommandLinkButton::clicked, this, &Settings_PAW_gui::showPluginsSettings);
    */
    
    QStringList availableDevices = audioThread.GetAvaliableDevices();
    audioDeviceComboBox->addItems(availableDevices);
}

Settings_PAW_gui::~Settings_PAW_gui() // Correct class name
{
    delete ui;
}


void Settings_PAW_gui::showAudioSettings(){
    ui->EntryContent->setRowCount(2);
    ui->EntryContent->setColumnCount(2);

    ui->EntryContent->setCellWidget(0, 1, audioDeviceComboBox);
}

/*disabled for a while
void Settings_PAW_gui::showUISettings(){
    ui->EntryContent->setRowCount(3);
    ui->EntryContent->setColumnCount(4);
}

void Settings_PAW_gui::showPluginsSettings(){
    ui->EntryContent->setRowCount(5);
    ui->EntryContent->setColumnCount(2);
}
*/
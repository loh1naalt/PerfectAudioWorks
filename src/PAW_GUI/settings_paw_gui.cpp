#include "settings_paw_gui.h" // Correct include
#include "ui_settings_paw_gui.h" 


Settings_PAW_gui::Settings_PAW_gui(PortaudioThread* audioThread, QWidget *parent) // Correct class name
    : QMainWindow(parent)
    , ui(new Ui::Settings_PAW_gui), m_audiothread(audioThread)
{
    ui->setupUi(this);

    connect(ui->AudioSettings, &QCommandLinkButton::clicked, this, &Settings_PAW_gui::showAudioSettings);
    connect(ui->ApplyButtion, &QCommandLinkButton::clicked,this, &Settings_PAW_gui::applySettings);
    
    QList<QPair<QString, int>> availableDevices = m_audiothread->GetAllAvailableOutputDevices();
    for (const auto& device : availableDevices) {
        audioDeviceComboBox->addItem(device.first, device.second);
    }
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
void Settings_PAW_gui::applySettings() {
    if (m_audiothread) { // Check if the pointer is not null
        int selectedPaDeviceIndex = audioDeviceComboBox->currentData().toInt();
        m_audiothread->setAudioDevice(selectedPaDeviceIndex);
    } else {
        qWarning() << "PortaudioThread is null, cannot set audio device.";
    }
}

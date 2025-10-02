#include "powermodule.h"
#include "ui_powermodule.h"
#include "myconfig.h"

using namespace PowerSymb;

PowerModule::PowerModule(QWidget *parent) : QWidget(parent), ui(new Ui::PowerModule)
{
    ui->setupUi(this);
}

PowerModule::~PowerModule()
{
    delete ui;
}

void PowerModule::changeStylesheet(float factor)
{
    // Main layout margins
    ui->mainLayout->setContentsMargins(int(26*factor), int(26*factor), int(26*factor), int(26*factor));

    // Battery bar
    bar.replace("barHeight", QString::number(52*factor));
    bar.replace("barBorder", QString::number(24*factor));
    bar.replace("borderRadius", QString::number(26*factor));
    ui->batBar->setStyleSheet(bar);

    // Frames
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->battery_frame->setStyleSheet(frames);
    frames.replace("#battery_frame", "#volt5V_frame");
    ui->volt5V_frame->setStyleSheet(frames);
    frames.replace("#volt5V_frame", "#volt3V3_frame");
    ui->volt3V3_frame->setStyleSheet(frames);
    frames.replace("#volt3V3_frame", "#pyro1_frame");
    ui->pyro1_frame->setStyleSheet(frames);
    frames.replace("#pyro1_frame", "#pyro2_frame");
    ui->pyro2_frame->setStyleSheet(frames);

    // Spacing
    ui->mainLayout->setSpacing(int(20*factor));
    ui->voltageLayout->setSpacing(int(20*factor));
    ui->pyrosLayout->setSpacing(int(20*factor));
    ui->pyro1Layout->setSpacing(int(10*factor));
    ui->pyro2Layout->setSpacing(int(10*factor));
    ui->voltage5Layout->setSpacing(int(10*factor));
    ui->voltage3Layout->setSpacing(int(10*factor));
    ui->batteryLayout->setSpacing(int(10*factor));
}

void PowerModule::updateLCDs(QVector<double> values)
{
    ui->voltBatLCD->display(values[Bat_Volt]);
    ui->volt5LCD->display(values[_5V_Volt]);
    ui->volt3LCD->display(values[_3V3_Volt]);
    ui->Pyro1_volt_LCD->display(values[Pyro1_Volt]);
    ui->Pyro2_volt_LCD->display(values[Pyro2_Volt]);

    ui->ampBatLCD->display(values[Bat_Amp]);
    ui->amp5LCD->display(values[_5V_Amp]);
    ui->amp3LCD->display(values[_3V3_Amp]);
    ui->Pyro1_amp_LCD->display(values[Pyro1_Amp]);
    ui->Pyro2_amp_LCD->display(values[Pyro2_Amp]);
}

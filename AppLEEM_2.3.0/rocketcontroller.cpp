#include "rocketcontroller.h"
#include "ui_rocketcontroller.h"
#include "myconfig.h"

#include <QQuickItem>
#include <QQmlContext>
#include <QMessageBox>

using namespace RocketSymb;

RocketController::RocketController(QWidget *parent) : QWidget(parent), ui(new Ui::RocketController)
{
    ui->setupUi(this);

    // QML widgets setup
    ui->speed->setSource(QUrl(QStringLiteral("qrc:/qmlResources/speedGauge.qml")));
    ui->acc->setSource(QUrl(QStringLiteral("qrc:/qmlResources/accGauge.qml")));
    ui->gyroscope->setSource(QUrl(QStringLiteral("qrc:/qmlResources/Gyroscope.qml")));

    ui->speed->show();
    ui->acc->show();
    ui->gyroscope->show();

    // LCD setups
    ui->timeLCD->setDigitCount(7);
    ui->altitudLCD->setDigitCount(7);
    ui->maxAltitudLCD->setDigitCount(7);
}

RocketController::~RocketController()
{
    delete ui;
}

void RocketController::on_ping_button_clicked()
{
    emit dataToSend(ping, AppModes::rocketMode);
}

void RocketController::on_phase_button_clicked()
{
    emit dataToSend(change_phase, AppModes::rocketMode);
}

void RocketController::on_tare_button_clicked()
{
    emit dataToSend(tare, AppModes::rocketMode);
}

void RocketController::on_alarm_button_clicked(bool status)
{
    if (status)
    {
        ui->alarm_button->setText("Alarm on");
        emit dataToSend(alarm_on, AppModes::rocketMode);
    }
    else
    {
        ui->alarm_button->setText("Alarm off");
        emit dataToSend(alarm_off, AppModes::rocketMode);
    }
}

void RocketController::updateGauges(QVector<double> data)
{
    emit changeAcc              (data[Gauges::acceleration]);
    emit changeSpeed            (data[Gauges::speed]);
    emit changeRotX             (data[Gauges::rotX]);
    emit changeRotY             (data[Gauges::rotY]);
    emit changeRotZ             (data[Gauges::rotZ]);

    ui->statusLCD->setText      (PhasesName[(int) data[Gauges::status]]);
    ui->timeLCD->display        (data[Gauges::time_seconds]);
    ui->altitudLCD->display     (data[Gauges::altitude]);
    ui->maxAltitudLCD->display  (data[Gauges::maxAltitude]);
}

void RocketController::resetTest()
{
    launchStarted = false;
    readyToLaunch = false;
    landed = false;

    ui->phase_button->setDisabled(false);
    checkedButton.replace("rgb(212,235,242)", "rgb(114,190,212)");
    ui->phase_button->setStyleSheet(checkedButton);
    ui->ping_button->setStyleSheet(checkedButton);
    ui->ping_button->setDisabled(false);
}

void RocketController::makeConnections()
{
    auto speed = ui->speed->rootObject();
    auto acc = ui->acc->rootObject();
    auto gyroscope = ui->gyroscope->rootObject();

    connect(this, SIGNAL(changeSpeed(QVariant)), speed, SLOT(changeGaugeValue(QVariant)));
    connect(this, SIGNAL(changeAcc(QVariant)), acc, SLOT(changeGaugeValue(QVariant)));
    connect(this, SIGNAL(changeRotX(QVariant)), gyroscope, SLOT(changeRotationX(QVariant)));
    connect(this, SIGNAL(changeRotY(QVariant)), gyroscope, SLOT(changeRotationY(QVariant)));
    connect(this, SIGNAL(changeRotZ(QVariant)), gyroscope, SLOT(changeRotationZ(QVariant)));
    connect(this, SIGNAL(setZoomRocket(QVariant)), gyroscope, SLOT(setZoomRocket(QVariant)));
    emit setZoomRocket(30);
}

void RocketController::changeStylesheet(float factor)
{
    whiteButton.replace("borderRadius", QString::number(20*factor));
    whiteButton.replace("buttonHeight", QString::number(40*factor));
    ui->tare_button->setStyleSheet(whiteButton);
    ui->ping_button->setStyleSheet(whiteButton);
    ui->phase_button->setStyleSheet(whiteButton);

    checkedButton.replace("borderRadius", QString::number(20*factor));
    checkedButton.replace("buttonHeight", QString::number(40*factor));
    ui->alarm_button->setStyleSheet(checkedButton);

    ui->mainLayout->setSpacing(int(50*factor));
    ui->buttonLayout->setVerticalSpacing(int(20*factor));
    ui->buttonLayout->setHorizontalSpacing(int(45*factor));
    ui->buttonLayout->setContentsMargins(int(20*factor), int(10*factor), int(20*factor), 0);
    ui->displaysLayout->setVerticalSpacing(int(15*factor));
    ui->displaysLayout->setHorizontalSpacing(int(60*factor));
    ui->gaugesLayout->setVerticalSpacing(int(40*factor));

    ui->mainLayout->setContentsMargins(int(20*factor), int(20*factor), int(20*factor), int(20*factor));
}

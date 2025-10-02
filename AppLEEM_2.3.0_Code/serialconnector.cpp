#include "serialconnector.h"
#include "ui_serialConnector.h"

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMessageBox>

SerialConnector::SerialConnector(QWidget *parent): QDialog(parent), ui(new Ui::SerialConnector)
{
    ui->setupUi(this);
    connect(serialCred, SIGNAL(orderReceived(uint8_t, uint8_t)), this, SLOT(deviceRecognition(uint8_t, uint8_t)));
    connect(deviceTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

SerialConnector::~SerialConnector()
{
    delete ui;
}

void SerialConnector::selectPort()
{
    ui->portSelect->clear();
    directions.clear();

    // We count all the connections
    portDirections.append(QString::number(QSerialPortInfo::availablePorts().length()));

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        // We name each connections, getting the vendor ID, product ID and port name
        if (serialPortInfo.hasVendorIdentifier())
        {
            portDirections.append((QString::number(serialPortInfo.vendorIdentifier())));
            portDirections.append((QString::number(serialPortInfo.productIdentifier())));
            portDirections.append(serialPortInfo.portName());
            serialPortInfo.productIdentifier();
        }
    }
    portListArrived();
    this->exec();
}

void SerialConnector::portListArrived()
{
    // We label the port selector
    QString information = QString(" %1%2%3 \n").arg("VendorID", 8, ' ').arg("ProductID", 15, ' ').arg("Port Name", 15, ' ');
    ui->portSelect->addItem(information);

    // We write the information
    for (int i = 0; i < portDirections.at(0).toInt()/3; ++i)
    {
        QString portCompound = QString("%1").arg(portDirections.at(1+i*3), 8, ' ');
        for (int j = 2; j <= 3; ++j) {portCompound.append(QString("%1").arg(portDirections.at(j+i*3), 15, ' '));}
        ui->portSelect->addItem(portCompound);
    }
    selectingPort = true;
}

void SerialConnector::on_portSelect_currentRowChanged(int currentRow)
{
    // While selecting the port
    if (selectingPort)
    {
        if (currentRow > 0)
        {
            // We indicate that after this, it is going to select the baudRate
            selectingBaudRate = true;
            selectingPort = false;
            portName = portDirections.at(3 * currentRow);

            ui->portSelect->clear();
            ui->portSelect->addItem(" Select Baud Rate: \n");

            for (int i = 0; i < baudRate.size(); ++i) {ui->portSelect->addItem(" " + QString::number(baudRate[i]));}
        }
    }
    // While selecting the baudRate
    else if (selectingBaudRate && (currentRow > 0))
    {
        // Starts the device identification procedure
        baudRateSelected = baudRate[currentRow-1];
        serialCred->connectArduino(portName, baudRateSelected);
        serialCred->sendOrdersSerial(GeneralSymb::Orders::deviceConnected, AppModes::benchMode);

        // Starts the timeout
        deviceTimer->start(1000);
        secondsCount = 0;
    }
}

void SerialConnector::deviceRecognition(uint8_t mode, uint8_t order)
{
    using namespace GeneralSymb;
    if (order == Orders::deviceConnected)
    {
        switch (mode)
        {
        case Devices::benchDevice:
            controllerID = 0;
            break;
        case Devices::rocketDevice:
            controllerID = 0;
            break;
        }
        selectDeviceConexion();
        QMessageBox::information(this, "Success", "Device connected succesfully");
    }
}

void SerialConnector::timeout()
{
    serialCred->sendOrdersSerial(GeneralSymb::Orders::deviceConnected, AppModes::benchMode);
    if (++secondsCount >= 15)
    {
        QMessageBox::warning(this, "Warning", "Device ID not recognized");
        selectDeviceConexion();
    }
}

void SerialConnector::selectDeviceConexion()
{
    serialCred->closePort();
    deviceTimer->stop();
    emit connectController(portName, baudRateSelected, controllerID);
    this->hide();
}

void SerialConnector::changeStylesheet(float factor)
{
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->frame->setStyleSheet(frames);

    this->resize(factor*480, factor*360);
}

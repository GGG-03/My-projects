#include "orders.h"
#include "ui_orders.h"
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <iostream>

bool mPressed = false;
bool lPressed = false;
bool rPressed = false;
bool aPressed = false;
bool qPressed = false;

orders::orders(QWidget *parent): QWidget(parent), ui(new Ui::orders)
{
    ui->setupUi(this);
    baudRate << 0 << 1200 << 2400 << 4800 << 9600 << 19200 << 38400 << 57600 << 115200;
}

orders::~orders()
{
    delete ui;
}

void orders::on_TLLButton_clicked(bool checked)
{
    if (!lPressed && checked)
    {
        emit dataToSend("TLH\n");
        lPressed = true;
    }
    else if (lPressed && !checked)
    {
        emit dataToSend("TLL\n");
        lPressed = false;
    }
}

void orders::on_TRRButton_clicked(bool checked)
{
    if (!rPressed && checked)
    {
        emit dataToSend("TRH\n");
        rPressed = true;
        qInfo() << "a";
    }
    else if (rPressed && !checked)
    {
        emit dataToSend("TRL\n");
        rPressed = false;
    }
}

void orders::on_MMButton_clicked(bool checked)
{
    if (!mPressed && checked)
    {
        emit dataToSend("EMM\n");
        emit mainMode(-1);
        ui->MMButton->setText("Performing");
        mPressed = true;
    }
    else if (mPressed && !checked)
    {
        emit dataToSend("TMM\n");
        emit mainMode(-2);
        ui->MMButton->setText("Testing");
        mPressed = false;
    }
}

void orders::on_TI2Button_clicked()
{
    emit dataToSend("TI2\n");
}

void orders::on_appButton_clicked(bool checked)
{
    if (!aPressed && checked)
    {
        emit mainMode(1);
        aPressed = true;
        ui->appButton->setText("Bench");
    }
    else if (aPressed && !checked)
    {
        emit mainMode(2);
        aPressed = false;
        ui->appButton->setText("Launchpad");
    }
}

void orders::on_TTDButton_clicked()
{
    emit dataToSend("TTD\n");
}

void orders::on_Conexion_clicked()
{
    selectingBaudRate = false;
    selectingPort = true;
    ui->portSelect->clear();
    emit selectPort();
}

void orders::portListArrived(QStringList portDirections)
{
    QString information = "  VendorID, ProductID, Port Name:";
    //ui->portSelect->addItem("");
    ui->portSelect->addItem(information);
    //qInfo () << portDirections;
    QString portCompound;
    for (int i = 0; i < portDirections.at(0).toInt(); i++)
    {
        portCompound = ("  ");
        portCompound.append(portDirections.at(1+i*3));
        portCompound.append(", ");
        portCompound.append(portDirections.at(2+i*3));
        portCompound.append(", ");
        portCompound.append(portDirections.at(3+i*3)); 
        ui->portSelect->addItem(portCompound);
    }
}

void orders::on_portSelect_currentRowChanged(int currentRow)
{
    if (selectingPort)
    {
        selectingBaudRate = true;
        selectingPort = false;
        nPort = currentRow-1;

        ui->portSelect->clear();
        //ui->portSelect->addItem("");
        ui->portSelect->addItem("  Select Baud Rate: ");

        for (int i = 1; i < baudRate.size(); i++)
        {
            ui->portSelect->addItem(QString("  ") + QString::number(baudRate[i]));
        }
    }
    else if (selectingBaudRate)
    {
        //qInfo() << currentRow;
        if (currentRow > 0) emit portSelected(nPort, baudRate[currentRow]);
    }
}

void orders::changeStatusPort(QString status)
{  
    ui->portSelect->clear();
    //ui->portSelect->addItem("");
    ui->portSelect->addItem(status);
}

/*
void orders::on_QPrintButton_clicked(bool checked)
{
    if (!qPressed && checked)
    {
        emit orderToSend(1);
        qPressed = true;
    }
    else if (qPressed && !checked)
    {
        emit orderToSend(2);
        qPressed = false;
    }
}
*/

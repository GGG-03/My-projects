#include "serialdevice.h"
#include "myconfig.h"
#include "qdebug.h"

SerialDevice::SerialDevice(QObject *parent) : QObject{parent}
{
    buffer.reserve(2000);
}

SerialDevice::~SerialDevice()
{
    if (Arduino) closePort();
}

void SerialDevice::connectArduino(QString portSelected, uint32_t baudRate)
{
    if (!Arduino) {Arduino = new QSerialPort;}
    else Arduino->disconnect();

    // Sets all the specs, including the baudrate and the port selected
    Arduino->setPortName(portSelected);
    Arduino->open(QSerialPort::ReadWrite);
    Arduino->setDataBits(QSerialPort::Data8);
    Arduino->setBaudRate(baudRate);
    Arduino->setParity(QSerialPort::NoParity);
    Arduino->setStopBits(QSerialPort::OneStop);
    Arduino->setFlowControl(QSerialPort::NoFlowControl);

    // Connects the reading
    connect(Arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
}

void SerialDevice::closePort()
{
   Arduino->close();
}

void SerialDevice::sendOrdersSerial(char order, char deviceID)
{
    // Sends a pack consisting of an identifier byte, the order in char and a newline
    QByteArray packOrder;
    packOrder.append(serialID);
    switch (deviceID)
    {
    case AppModes::benchMode:
        packOrder.append(benchID);
        break;
    case AppModes::rocketMode:
        packOrder.append(rocketID);
        break;
    }
    packOrder.append(order + 3);
    packOrder.append(uchar(2));
    packOrder.append(uchar(1));
    packOrder.append(order);
    packOrder.append('\n');

    if (Arduino->isWritable())
    {
        Arduino->write(packOrder);
        Arduino->flush();
    }
}

void SerialDevice::readSerial()
{
    // Adds to the buffer the data and process it
    buffer += Arduino->readAll();
    decodifier();
}

void SerialDevice::decodifier()
{
    do
    {
        int posID = buffer.indexOf(serialID);

        if (posID == -1)
        {
            if (buffer.size() > 1000)
                buffer.clear();
            return;
        }

        int infoIndex = posID + 5;
        int length = buffer[posID + 4];

        int endIndex = infoIndex + length - 1;

        if (endIndex <= buffer.size())
        {
            uint8_t checkSum = 0;
            for (int i = infoIndex - 1; i <= endIndex; ++i) {checkSum += (uint8_t) buffer[i];}

            if (checkSum != ((uint8_t) buffer[posID + 3]))
            {
                buffer.remove(0, posID + 1);
                return;
            }

            if (length > 2)
            {
                QVector<double> arrivedData(length/4);

                for (int i = 0; i < length/4; ++i)
                {
                    QByteArray floatPack("", 4);
                    int packIndex = i * 4 + infoIndex;
                    for (int j = 0; j < 4; ++j) {floatPack[j] = buffer[j + packIndex];}
                    arrivedData[i] = *reinterpret_cast<const float*>(floatPack.constData());
                }

                emit dataReceived(arrivedData);
            }
            else if (length == 2)
            {
                emit orderReceived((uint8_t) buffer[infoIndex], (uint8_t) buffer[infoIndex + 1]);
            }
            else
            {
                endIndex = infoIndex;
            }

            buffer.remove(0, endIndex);
        }
        else
        {
            return;
        }
    } while (buffer.size() >= 25);

    if (buffer.size() < 500)
    {
        buffer.clear();
    }
}

#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <QObject>
#include <QSerialPort>

class SerialDevice : public QObject
{
    Q_OBJECT
    public:
        explicit SerialDevice(QObject *parent = nullptr);
        ~SerialDevice();

    private:
        QSerialPort *Arduino = nullptr;
        QByteArray buffer = "";

        // ID Devices
        QByteArray serialID {"\xFE\xFB"};
        QByteArray benchID {"\xFA"};
        QByteArray rocketID {"\xF3"};

        void decodifier();

    private slots:
        void readSerial();

    public slots:
        void closePort();

    public slots:
        void connectArduino(QString, uint32_t);
        void sendOrdersSerial(char, char);

    signals:
        void dataReceived(QVector<double>&);
        void orderReceived(uint8_t, uint8_t);
};

#endif // SERIALDEVICE_H

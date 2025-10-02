#ifndef SERIALCONNECTOR_H
#define SERIALCONNECTOR_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QElapsedTimer>
#include <QTimer>

#include <serialdevice.h>
#include <myconfig.h>

namespace Ui
{
    class SerialConnector;
}

class SerialConnector : public QDialog
{
    Q_OBJECT 

    public:
        explicit SerialConnector(QWidget *parent = nullptr);
        ~SerialConnector();

        // Stylesheet
        void changeStylesheet(float);

        // Serial device
        SerialDevice *serialCred = new SerialDevice;

    private:
        Ui::SerialConnector *ui;

        QString portName;

        uint8_t nombreProductID = 0;
        uint8_t controllerID = 0;
        uint32_t baudRateSelected = 0;

        void selectDeviceConexion();

        bool selectingPort = false;
        bool selectingBaudRate = false;

        QByteArray serialData;
        QStringList portDirections, directions;

        QTimer *deviceTimer = new QTimer;
        uint8_t secondsCount;

        QVector<uint32_t> baudRate = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 250000, 460800, 500000, 921600, 1000000, 2000000};

        void portListArrived();

        // Stylesheet
        QString frames = "#frame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";

    public slots:
        void selectPort();

    private slots:        
        void on_portSelect_currentRowChanged(int);
        void deviceRecognition(uint8_t, uint8_t);
        void timeout();

    signals:
        void connectController(QString, uint32_t, uint8_t);

};
#endif // SERIALCONNECTOR_H

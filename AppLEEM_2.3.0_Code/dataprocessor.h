#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QStringListModel>

#include <serialdevice.h>

class DataProcessor : public QObject
{
    Q_OBJECT

    public:
        explicit DataProcessor(QObject *parent = nullptr);

        bool zoom = true;
        bool thrustInN = false;

        // Phase
        int phase = 0;

        // Mode
        void changeMode(char);
        void resetTest();

        // Serial device
        SerialDevice *mainSerial;

    private:
        // Mode
        char appMode = 0;

        // File headers
        const QString benchFileHeader    = "Time (s), Thrust (kg), Chamber_Pressure (bar), Temperature (ºC), Humidity (%), "
                                           "TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, ADC1, ADC2, ADC3, ADC4\n";

        const QString launchFileHeader   = "Time, Phase, VN Altitude, Bat Voltage, Bat Current, Pyro 1 Current, Pyro 2 Current, Teensy Temp, STM32 Temp, "
                                           "VN Acc X, VN Acc Y, Vn Acc Z, VN Yaw, VN Pitch, VN Roll, VN Speed X, VN Speed Y, VN Speed Z, Latitude, Longitude\n";

        const QString ignitorsFileHeader = "";

        // Frame rate
        QTimer *frameTimer;

        // Time status
        double timeStarted = 0;
        double timeFinished = 0;
        double timeElapsed = 0;

        // Terminal
        QStringList dataTerminal;
        QStringListModel *dataTerminalModel = new QStringListModel;

        // Order
        QStringList orderTerminal;
        QStringListModel *orderTerminalModel = new QStringListModel;

        // Terminal labels
        QString benchLabel =    QString(" %1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18%19%20%21%22%23 \n")
                                .arg("Time", 11, ' ').arg("Thrust", 11, ' ').arg("Eng. P", 11, ' ').arg("Temp", 11, ' ').arg("Hum", 11, ' ')
                                .arg("TP 1", 11, ' ').arg("TP 2", 11, ' ').arg("TP 3", 11, ' ').arg("TP 4", 11, ' ').arg("TP 5", 11, ' ')
                                .arg("TP 6", 11, ' ').arg("TP 7", 11, ' ').arg("TP 8", 11, ' ').arg("TP 9", 11, ' ').arg("TP 10", 11, ' ')
                                .arg("TP 11", 11, ' ').arg("TP 12", 11, ' ').arg("TP 13", 11, ' ').arg("TP 14", 11, ' ')
                                .arg("ADC 1", 11, ' ').arg("ADC 2", 11, ' ').arg("ADC 3", 11, ' ').arg("ADC 4", 11, ' ');

        QString launchLabel =   QString(" %1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18%19 \n")
                                .arg("Time", 11, ' ').arg("Phase", 11, ' ').arg("VN Alt", 11, ' ').arg("Bat Volt", 11, ' ').arg("Bat Amp", 11, ' ')
                                .arg("Pyro1 Amp", 11, ' ').arg("Pyro2 Amp", 11, ' ').arg("Teensy T", 11, ' ').arg("STM32 T", 11, ' ').arg("Acc X", 11, ' ')
                                .arg("Acc Y", 11, ' ').arg("Yaw", 11, ' ').arg("Pitch", 11, ' ').arg("Roll", 11, ' ').arg("Speed X", 11, ' ')
                                .arg("Speed Y", 11, ' ').arg("Speed Z", 11, ' ').arg("Latitude", 11, ' ').arg("Longitude", 11, ' ');
        QString orderLabel = "     Orders received :";

        // Reading serial
        QByteArray serialID {"\xFE\xFB"};

        QStringList dataSplitted;
        QVector<double> dataReceived;

        // Telemetria     
        QVector<QVector<double>> altitudeData;
        QVector<double> dataGauges, maxAltitudeData;
        int lastData = 0;

        bool firstLocation = true;

        void telemetriaProcessing();

        // Telemetria acc
        float speedTN = 0;
        float speedZN = 0;
        float zN = 0;

        void accIntegrator(float, float, float);

        // Bench        
        QVector<QVector<double>> benchData;
        QVector<double> maxBenchData;

        double timeSA = 0;
        double timeFA = 0;
        QVector<double> benchIntegralData = {0, 0, 0};
        QVector<QString> benchIntegralHeader = {"\n Total impulse is ", "\n Average thrust is ", "\n Max thrust is "};
        QVector<QString> benchIntegralUnits = {" kg*s", " kg", " kg", " N*s.", " N.", " N."};

        int avoidSpikes = 0;
        int nTCouples = 12;

        void benchProcessing();

        // Ignitors       
        QVector<QVector<double>> ignitorsData;
        QVector<double> LCDData, maxIgnitorsData;

        void ignitorsProcessing();

        // Power
        QVector<double> powerGauges;

        // File to Decodify
        QFile decFile, codFile;
        QTextStream decOut, codIn;

        void decodifyFileProcessing(QVector<double>&);

        // File
        QString firstNameFile;
        QFile rawData;
        QTextStream rawOut;

        void openFile();

        // Comparing
        QVector<QVector<double>> comparingData;

    public slots:
        void orderToSave(QString);
        void compareGraph(QString, bool);
        void decodifyFile(QString, QString);

    private slots:
        void startUp();

        // Data processing
        void dataProcessing(QVector<double>&);
        void orderProcessing(uint8_t, uint8_t);

        void phaseChanged(int);
        void getImpulse(double, double);    
        void changeFPS(int);
        void refresh();

        // Bench
        void changeThrustUnits(bool);
        void nTHChanged(int); 

    signals:
        // File error
        void fileNotOpened(int);

        // Terminal
        void updateTerminal(QStringListModel*, QStringListModel*);

        // Mode
        void changeMainMode(int);
        void plot(QVector<QVector<double>>, QVector<double>, char);

        // Bench
        void sendDecreasedThrust(double, double);
        void showImpulse(QString);

        // Rocket
        void updateMap(double, double, bool);
        void mark(QVariant, QVariant);
        void parachuteStatus(bool);
        void plot3D(double, double, double);
        void updateGauges(QVector<double>);

        // Power
        void updatePower(QVector<double>);
};

#endif // DATAPROCESSOR_H

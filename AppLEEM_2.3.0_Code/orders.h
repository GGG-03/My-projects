#ifndef ORDERS_H
#define ORDERS_H

#include <QWidget>
#include <QMap>
#include <QtSerialPort/QSerialPort>

namespace Ui {class orders;}

class orders : public QWidget
{
    Q_OBJECT

    public:
        explicit orders(QWidget *parent = nullptr);      
        ~orders();
        Ui::orders *ui;
        QString conexionStatus;

    public slots:
        void on_portSelect_currentRowChanged(int currentRow);
        void portListArrived(QStringList portDirections);
        void changeStatusPort(QString status);

    private slots:
        void on_appButton_clicked(bool checked);
        void on_TLLButton_clicked(bool checked);
        void on_TRRButton_clicked(bool checked);
        void on_MMButton_clicked(bool checked);
        void on_TI2Button_clicked();
        void on_TTDButton_clicked();
        void on_Conexion_clicked();

    private:
        bool selectingPort;
        bool selectingBaudRate;
        int nPort;
        QVector <int> baudRate;

    signals:
        void mainMode(int mode);
        void dataToSend(QByteArray order);
        void dataToRead(QByteArray order);
        void orderToSend(int orders);
        void dataReady();
        void selectPort();
        void portSelected(int port, int baudRate);
};

#endif // ORDERS_H

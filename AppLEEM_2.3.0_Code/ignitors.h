#ifndef IGNITORS_H
#define IGNITORS_H

#include <QWidget>

namespace Ui
{
    class Ignitors;
}

class Ignitors : public QWidget
{
    Q_OBJECT

    public:
        explicit Ignitors(QWidget *parent = nullptr);
        ~Ignitors();

        // Reset
        void resetTest();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::Ignitors *ui;

        bool block = false;
        bool stop = false;

        // Stylesheet
        QString frames = "QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString whiteButtons = "QPushButton { border-radius: borderRadius%; background-color: white; }";

    public slots:
        void saveGraph(QString);

    private slots:
        void plot(QVector<QVector<double>>, QVector <double>, char);
        void updateGauges(QVector<double>);
        void on_startButton_clicked();

    signals:
        void dataToSend(int);
        void status(bool);

};

#endif // IGNITORS_H

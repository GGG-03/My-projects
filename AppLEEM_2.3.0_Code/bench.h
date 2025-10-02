#ifndef BENCH_H
#define BENCH_H

#include <QWidget>
#include <QVector>
#include <QTextStream>
#include <QDateTime>
#include <QStringListModel>
#include <QElapsedTimer>

namespace Ui
{
    class Bench;
}

class Bench: public QWidget
{
    Q_OBJECT

    public:
        explicit Bench(QWidget *parent = nullptr);
        ~Bench();

        bool testing = true;

        void dual_triggered(bool);
        void resetTest();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::Bench *ui;

        // Phase
        int phase = 0;
        int nTCouples = 12;

        // Widgets
        bool dualScreen = false;
        bool showPressure = true;
        bool showTemps = true;

        QWidget *tempGF;

        // Units
        bool thrustInN = false;

        // Times selected to integrate
        double timeFinished = 0;
        double timeStarted = 0;

        // StyleSheet
        QString frames = "QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString whiteButton = "QPushButton { height: buttonHeightpx; border-radius: borderRadiuspx; background-color: white; }";
        QString checkedButton = "QPushButton { height: buttonHeightpx; border-radius: borderRadiuspx; background-color: white; }  QPushButton:checked { background-color: rgb(212,235,242); }";

    public slots:
        void saveGraph(QString);
        void showLegend(bool);
        void showTerminal(bool);      

    private slots:
        // Terminal
        void updateTerminal(QStringListModel*, QStringListModel*);
        void floatingTerminal_toggled(bool status);

        void plot(QVector<QVector<double>>, QVector <double>, char);
        void recieveDecreasedThrust(double, double);
        void showImpulse(QString);

        void on_transducerButton_clicked(bool);
        void on_startButton_clicked();
        void on_tareButton_clicked();
        void on_hydroStaticButton_clicked(bool);

        void changeThrustUnits(bool);
        void showWidgets(int show);

    signals:
        void dataToSend(char, char);
        void startingLaunch();
        void getImpulse(double, double);
        void cleanMetadata();
        void phaseChanged(int);
        void hydroStatic_triggered(bool);
        void nTHChanged(int);
};

#endif // BANCO_H

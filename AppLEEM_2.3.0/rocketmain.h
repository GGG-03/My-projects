#ifndef ROCKETMAIN_H
#define ROCKETMAIN_H

#include <QWidget>
#include <QDateTime>
#include <QTimer>
#include <QVector>
#include <QVariant>
#include <QFile>
#include <QElapsedTimer>
#include <QTextStream>
#include <QStringListModel>
#include <QtDataVisualization>
#include <Q3DScatter>

namespace Ui
{
    class RocketMain;
}

using namespace QtDataVisualization;

class RocketMain : public QWidget
{
    Q_OBJECT

    public:
        explicit RocketMain(QWidget *parent = nullptr);
        ~RocketMain();

        bool testing = true;

        void makeConnections();

        // Reset
        void resetTest();     

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::RocketMain *ui;

        // Control variables
        bool zoom = true;
        int altitude3D = 0;
        bool showingTerminal = true;

        QElapsedTimer betProcess;

        Q3DScatter *altitudeScatter = new Q3DScatter;
        QWidget *altitude3DContainer = QWidget::createWindowContainer(altitudeScatter);

        QScatter3DSeries *scatterData = new QScatter3DSeries;

        // Stylesheet
        QString whiteButton = "QPushButton { height: buttonHeightpx; border-radius: borderRadiuspx; background-color: white; color: black; }";
        QString checkedButton = "QPushButton { height: buttonHeightpx; border-radius: borderRadiuspx; background-color: white; color: black; } QPushButton:checked { background-color: rgb(212,235,242); }";
        QString dataTerminal = "QListView { font-size: fontSizept; }  QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString frames =  "QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";

    public slots:
        void saveGraph(QString);
        void showLegend(bool);
        void showAltitude3D(bool);
        void showTerminal(bool);  

    private slots:
        // Terminal
        void updateTerminal(QStringListModel*, QStringListModel*);

        // Zoom
        void changeZoom(bool);

        // Buttons
        void on_ping_button_clicked();
        void on_phase_button_clicked();
        void on_tare_button_clicked();
        void on_alarm_button_clicked(bool);

        void updateGauges(QVector<double>);

        void updateMap(double, double, bool);

        void plot(QVector<QVector<double>>, QVector <double>, char);
        void plot3D(double, double, double);
        void floatingTerminal_toggled(bool status);

    signals:
        void dataToSend(char, char);

        void addPathCoordinates(QVariant, QVariant, QVariant);
        void clearPath();
        void mark(QVariant, QVariant);

        void changeSpeed(QVariant);
        void changeAcc(QVariant);
        void changeRotX(QVariant);
        void changeRotY(QVariant);
        void changeRotZ(QVariant);
        void setZoomRocket(QVariant);
};

#endif // ROCKETMAIN_H

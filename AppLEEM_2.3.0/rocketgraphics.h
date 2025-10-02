#ifndef ROCKETGRAPHICS_H
#define ROCKETGRAPHICS_H

#include <QDateTime>
#include <QWidget>
#include <QVariant>
#include <QFile>
#include <QElapsedTimer>
#include <QTextStream>
#include <QStringListModel>
#include <QtDataVisualization>
#include <Q3DScatter>

namespace Ui
{
    class RocketGraphics;
}

using namespace QtDataVisualization;

class RocketGraphics : public QWidget
{
    Q_OBJECT

    public:
        explicit RocketGraphics(QWidget *parent = nullptr);
        ~RocketGraphics();

        // Reset
        void resetTest();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::RocketGraphics *ui;

        // Control variables
        bool zoom = true;
        int altitude3D = 0;

        Q3DScatter *altitudeScatter = new Q3DScatter;
        QWidget *altitude3DContainer = QWidget::createWindowContainer(altitudeScatter);

        QScatter3DSeries *scatterData = new QScatter3DSeries;

        // Stylesheet
        QString dataTerminal = "QListView { font-size: fontSizept; }  QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString frames =  "QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";

    public slots:
        void changeZoom(bool);
        void saveGraph(QString);
        void showLegend(bool);
        void showAltitude3D(bool);
        void showTerminal(bool);
        void makeConnections();

    private slots:
        // Terminal
        void updateTerminal(QStringListModel*, QStringListModel*);

        void updateMap(double, double, bool);
        void plot(QVector<QVector<double>>, QVector <double>, char);
        void plot3D(double, double, double);
        void floatingTerminal_toggled(bool status);

    signals:
        void addPathCoordinates(QVariant, QVariant, QVariant);
        void clearPath();
        void mark(QVariant, QVariant);
};

#endif // ROCKETGRAPHICS_H

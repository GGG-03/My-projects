#ifndef BENCHGRAPHS_H
#define BENCHGRAPHS_H

#include <QWidget>

namespace Ui
{
    class BenchGraphs;
}

class BenchGraphs : public QWidget
{
    Q_OBJECT

    public:
        explicit BenchGraphs(QWidget *parent = nullptr);
        ~BenchGraphs();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::BenchGraphs *ui;

        // Graphs visibility
        bool showTemps = true;
        bool showPressure = true;

        // Thermocouples
        int nTCouples = 12;

        // StyleSheet
        QString frames =  "QFrame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";

    public slots:
        void saveGraph(QString);
        void showLegend(bool);

    private slots:
        void plot(QVector<QVector<double>>, QVector<double>, char);
        void showWidgets(int show);
        void nTHChanged(int);

};

#endif // BENCHGRAPHS_H

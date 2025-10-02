#include "benchgraphs.h"
#include "myconfig.h"
#include "ui_benchgraphs.h"
#include "qcustomplot.h"

BenchGraphs::BenchGraphs(QWidget *parent) : QWidget(parent), ui(new Ui::BenchGraphs)
{
    ui->setupUi(this);

    // Graphs setup
    ui->pressureGraph->addGraph();
    ui->pressureGraph->graph(0)->setPen(QPen(QColor(40, 110, 132)));

    for (int i = 0; i < maxTCouples; ++i)
        ui->tempGraph->addGraph();
    ui->tempGraph->graph(0)->setPen(QPen(QColor(40, 110, 132)));
    ui->tempGraph->graph(1)->setPen(QPen(QColor(100, 110, 132)));
    ui->tempGraph->graph(2)->setPen(QPen(QColor(200, 110, 132)));
    ui->tempGraph->graph(3)->setPen(QPen(QColor(40, 10, 132)));
    ui->tempGraph->graph(4)->setPen(QPen(QColor(40, 210, 132)));
    ui->tempGraph->graph(5)->setPen(QPen(QColor(40, 110, 132)));
    ui->tempGraph->graph(6)->setPen(QPen(QColor(40, 110, 60)));
    ui->tempGraph->graph(7)->setPen(QPen(QColor(40, 110, 200)));
    ui->tempGraph->graph(8)->setPen(QPen(QColor(40, 110, 0)));
    ui->tempGraph->graph(9)->setPen(QPen(QColor(40, 200, 200)));
    ui->tempGraph->graph(10)->setPen(QPen(QColor(200, 200, 132)));
    ui->tempGraph->graph(11)->setPen(QPen(QColor(0, 0, 132)));
    ui->tempGraph->graph(12)->setPen(QPen(QColor(200, 0, 132)));
    ui->tempGraph->graph(13)->setPen(QPen(QColor(50, 0, 132)));

    // Improves the perfomance
    ui->pressureGraph->graph(0)->setAdaptiveSampling(true);
    ui->pressureGraph->setNoAntialiasingOnDrag(true);
    ui->pressureGraph->setNotAntialiasedElements(QCP::aeAll);

    for (int i = 0; i < nTCouples; ++i) ui->tempGraph->graph(i)->setAdaptiveSampling(true);
    ui->tempGraph->setNoAntialiasingOnDrag(true);
    ui->tempGraph->setNotAntialiasedElements(QCP::aeAll);

    // Graphs stylesheet
    // Titles
    QFont title ("Consolas", 12, QFont::Bold);

    QCPTextElement *pressTitle = new QCPTextElement(ui->pressureGraph);
    pressTitle->setText("Chamber Pressure");
    pressTitle->setFont(title);
    ui->pressureGraph->plotLayout()->insertRow(0);
    ui->pressureGraph->plotLayout()->addElement(0, 0, pressTitle);

    QCPTextElement *tempTitle = new QCPTextElement(ui->tempGraph);
    tempTitle->setText("External Engine Temperatures");
    tempTitle->setFont(title);
    ui->tempGraph->plotLayout()->insertRow(0);
    ui->tempGraph->plotLayout()->addElement(0, 0, tempTitle);

    ui->pressureGraph->xAxis->setLabel("Time [s]");
    ui->pressureGraph->yAxis->setLabel("Pressure [bar]");

    ui->tempGraph->xAxis->setLabel("Time [s]");
    ui->tempGraph->yAxis->setLabel("Temperature [ºC]");

    // Axis
    QFont graphFont ("Consolas",11);
    ui->pressureGraph->xAxis->setTickLabelFont(graphFont);
    ui->pressureGraph->yAxis->setTickLabelFont(graphFont);
    ui->pressureGraph->xAxis->setLabelFont(graphFont);
    ui->pressureGraph->yAxis->setLabelFont(graphFont);
    ui->pressureGraph->legend->setFont(graphFont);

    ui->tempGraph->xAxis->setTickLabelFont(graphFont);
    ui->tempGraph->yAxis->setTickLabelFont(graphFont);
    ui->tempGraph->xAxis->setLabelFont(graphFont);
    ui->tempGraph->yAxis->setLabelFont(graphFont);
    ui->tempGraph->legend->setFont(graphFont);

    // Legends
    ui->pressureGraph->graph(0)->setName("Pressure Transducer");
    for (int i = 0; i < maxTCouples; ++i) {ui->tempGraph->graph(i)->setName("TH " + QString::number(i + 1));}

    ui->pressureGraph->replot();
    ui->tempGraph->replot();
}

BenchGraphs::~BenchGraphs()
{
    delete ui;
}

void BenchGraphs::plot(QVector<QVector<double>> data, QVector<double> maxData, char)
{
    using namespace BenchSymb;
    if (showPressure)
    {
        ui->pressureGraph->xAxis->setRange(0, maxData[timeInS]);
        ui->pressureGraph->yAxis->setRange(0, maxData[pressureInBar] + 10);
        ui->pressureGraph->graph(0)->setData(data[timeInS], data[pressureInBar], true);
        ui->pressureGraph->replot(QCustomPlot::rpQueuedReplot);
    }

    if (showTemps)
    {
        ui->tempGraph->xAxis->setRange(0, maxData[timeInS]);
        ui->tempGraph->yAxis->setRange(0, maxData[THTemperature] + 10);
        for (int i = 0; i < nTCouples; ++i) {ui->tempGraph->graph(i)->setData(data[timeInS], data[THTemperature + i], true);}
        ui->tempGraph->replot(QCustomPlot::rpQueuedReplot);
    }
}

void BenchGraphs::nTHChanged(int th)
{
    nTCouples = th;
}

void BenchGraphs::showLegend(bool show)
{
    ui->pressureGraph->legend->setVisible(show);
    ui->pressureGraph->replot();

    ui->tempGraph->legend->setVisible(show);
    ui->tempGraph->replot();
}

void BenchGraphs::showWidgets(int show)
{
    // It shows or hides each graph
    switch (show)
    {
        case 3:
            ui->tempFrame->setVisible(true);
            showTemps = true;
            ui->tempGraph->replot();
        break;

        case 4:
            ui->tempFrame->setVisible(false);
            showTemps = false;
        break;

        case 5:
            ui->pressureFrame->setVisible(true);
            showPressure = true;
            ui->pressureGraph->replot();
        break;

        case 6:
            ui->pressureFrame->setVisible(false);
            showPressure = false;
        break;
    }
}

void BenchGraphs::saveGraph(QString nameFile)
{
    ui->tempGraph->replot();
    ui->pressureGraph->replot();
    nameFile.append("_temperaturesGraph.pdf");
    ui->tempGraph->savePdf(nameFile);
    nameFile.replace("_temperatureGraph.pdf", "_temperatureGraph.pdf");
    ui->pressureGraph->savePdf(nameFile);
}

void BenchGraphs::changeStylesheet(float factor)
{
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->tempFrame->setStyleSheet(frames);
    ui->pressureFrame->setStyleSheet(frames);

    ui->mainLayout->setSpacing(int(20*factor));

    ui->mainLayout->setContentsMargins(int(26*factor), int(26*factor), int(26*factor), int(26*factor));
}

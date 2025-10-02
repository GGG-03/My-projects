#include "bench.h"
#include "ui_bench.h"
#include "qcustomplot.h"
#include "myconfig.h"

#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointF>
#include <QFile>
#include <QTextStream>
#include <QIODevice>

using namespace BenchSymb;

Bench::Bench(QWidget *parent): QWidget(parent), ui(new Ui::Bench)
{
    ui->setupUi(this);

    // Graphs setup
    ui->thrustGraph->addGraph();
    ui->thrustGraph->addGraph();
    ui->thrustGraph->graph(1)->setPen(QPen(QColor(255, 0, 0)));
    ui->thrustGraph->graph(0)->setPen(QPen(QColor(40, 110, 132)));

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
    ui->thrustGraph->graph(0)->setAdaptiveSampling(true);
    ui->thrustGraph->setNoAntialiasingOnDrag(true);
    ui->thrustGraph->setNotAntialiasedElements(QCP::aeAll);

    ui->pressureGraph->graph(0)->setAdaptiveSampling(true);
    ui->pressureGraph->setNoAntialiasingOnDrag(true);
    ui->pressureGraph->setNotAntialiasedElements(QCP::aeAll);

    for (int i = 0; i < nTCouples; ++i)
        ui->tempGraph->graph(i)->setAdaptiveSampling(true);
    ui->tempGraph->setNoAntialiasingOnDrag(true);
    ui->tempGraph->setNotAntialiasedElements(QCP::aeAll);

    // Graphs stylesheet
    // Titles
    QFont title ("Consolas", 12, QFont::Bold);
    QCPTextElement *thrustTitle = new QCPTextElement(ui->thrustGraph);
    thrustTitle->setText("Engine Thrust");
    thrustTitle->setFont(title);
    ui->thrustGraph->plotLayout()->insertRow(0);
    ui->thrustGraph->plotLayout()->addElement(0, 0, thrustTitle);

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

    ui->thrustGraph->xAxis->setLabel("Time [s]");
    ui->thrustGraph->yAxis->setLabel("Thrust [kg]");

    ui->pressureGraph->xAxis->setLabel("Time [s]");
    ui->pressureGraph->yAxis->setLabel("Pressure [bar]");

    ui->tempGraph->xAxis->setLabel("Time [s]");
    ui->tempGraph->yAxis->setLabel("Temperature [ºC]");

    // Axis
    QFont graphFont ("Consolas",11);
    ui->thrustGraph->xAxis->setTickLabelFont(graphFont);
    ui->thrustGraph->yAxis->setTickLabelFont(graphFont);
    ui->thrustGraph->xAxis->setLabelFont(graphFont);
    ui->thrustGraph->yAxis->setLabelFont(graphFont);
    ui->thrustGraph->legend->setFont(graphFont);

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
    ui->thrustGraph->graph(0)->setName("Thrust");
    ui->thrustGraph->graph(1)->setName("Estimated Thrust");
    ui->pressureGraph->graph(0)->setName("Pressure Transducer");
    for (int i = 0; i < maxTCouples; ++i)
        ui->tempGraph->graph(i)->setName("TH " + QString::number(i + 1));

    ui->thrustGraph->replot();
    ui->pressureGraph->replot();
    ui->tempGraph->replot();
    tempGF = ui->tempFrame;
}

Bench::~Bench()
{
    delete ui;
}

void Bench::dual_triggered(bool status)
{
    dualScreen = status;
    showWidgets(0);
}

void Bench::changeThrustUnits(bool inN)
{
    thrustInN = inN;
    if (inN) {ui->thrustGraph->yAxis->setLabel("Thrust [N]");}
    else {ui->thrustGraph->yAxis->setLabel("Thrust [kg]");}
}

void Bench::updateTerminal(QStringListModel* data_show, QStringListModel* order_show)
{
    ui->serialPrint->setModel(data_show);
    ui->orderPrint->setModel(order_show);
}

void Bench::plot(QVector<QVector<double>> data, QVector<double> maxData, char nGraph)
{
    using namespace BenchSymb;
    if (thrustInN)
    {
        ui->thrustGraph->graph(nGraph)->setData(data[timeInS], data[nBenchData + nTCouples], true);
        ui->thrustGraph->yAxis->setRange(0, maxData[thrustInKg] * gravity + 10);
    }
    else
    {
        ui->thrustGraph->graph(nGraph)->setData(data[timeInS], data[thrustInKg], true);
        ui->thrustGraph->yAxis->setRange(0, maxData[thrustInKg] + 10);
    }
    ui->thrustGraph->xAxis->setRange(0, maxData[timeInS]);
    ui->thrustGraph->replot(QCustomPlot::rpQueuedReplot);

    if (showPressure && !dualScreen)
    {
        ui->pressureGraph->xAxis->setRange(0, maxData[timeInS]);
        ui->pressureGraph->yAxis->setRange(0, maxData[pressureInBar] + 10);
        ui->pressureGraph->graph(0)->setData(data[timeInS], data[pressureInBar], true);
        ui->pressureGraph->replot(QCustomPlot::rpQueuedReplot);
    }

    if (showTemps && !dualScreen)
    {
        ui->tempGraph->xAxis->setRange(0, maxData[timeInS]);
        ui->tempGraph->yAxis->setRange(0, maxData[THTemperature] + 10);
        for (int i = 0; i < nTCouples; ++i)
            ui->tempGraph->graph(i)->setData(data[timeInS], data[THTemperature + i], true);
        ui->tempGraph->replot(QCustomPlot::rpQueuedReplot);
    }
}

void Bench::showLegend(bool show)
{
    ui->thrustGraph->legend->setVisible(show);
    ui->thrustGraph->replot();

    ui->pressureGraph->legend->setVisible(show);
    ui->pressureGraph->replot();

    ui->tempGraph->legend->setVisible(show);
    ui->tempGraph->replot();
}

void Bench::resetTest()
{
    phase = Phases::standby;
    ui->startButton->setText("Start");
    ui->hydroStaticButton->setDisabled(false);
}

void Bench::saveGraph(QString nameFile)
{
    ui->thrustGraph->replot();
    nameFile.append("_thrustGraph.pdf");
    ui->thrustGraph->savePdf(nameFile);

    if (!dualScreen)
    {
        ui->pressureGraph->replot();
        nameFile.replace("_thrustGraph.pdf", "_pressureGraph.pdf");
        ui->pressureGraph->savePdf(nameFile);

        ui->tempGraph->replot();
        nameFile.replace("_pressureGraph.pdf", "_temperatureGraph.pdf");
        ui->tempGraph->savePdf(nameFile);
    }
}

void Bench::showTerminal(bool show)
{
    ui->serialFrame->setVisible(show);
}

void Bench::floatingTerminal_toggled(bool status)
{
    ui->orderFrame->setVisible(status);
}

void Bench::recieveDecreasedThrust(double timeS, double timeF)
{
    timeStarted = timeS;
    timeFinished = timeF;
}

void Bench::showWidgets(int show)
{
    // It shows or hides each graph
    switch (show)
    {
    case 3:
        showTemps = true;
        break;

    case 4:
        showTemps = false;        
        break;

    case 5:
        showPressure = true;
        ui->controltempLayout->addWidget(tempGF);
        break;

    case 6:
        showPressure = false;
        ui->thrustpressureLayout->addWidget(tempGF);
        break;
    }

    // Doesn't show it if dual screen activated
    ui->pressureFrame->setVisible(showPressure && !dualScreen);
    ui->pressureGraph->replot();

    ui->tempFrame->setVisible(showTemps && !dualScreen);
    ui->tempGraph->replot();
}

void Bench::on_startButton_clicked()
{
    switch (phase)
    {
    case Phases::standby: // Not tared yet
        QMessageBox::question(this, "Too fast my friend", "You need to tare first", QMessageBox::Ok);
        break;
    case Phases::tared: // First click: starts
        ui->startButton->setText("Stop");
        ++phase;
        break;
    case Phases::ready: // Second click: stops
        ++phase;
    case Phases::ignited:
        ui->startButton->setText("Get data");
        ++phase;
        break;
    case Phases::finished: // Third click: gets data
        bool ok;
        double timeF, timeS = QInputDialog::getDouble(this, tr("Bench data"), tr("Set start time "), timeStarted, 0, 1000000, 3 , &ok, Qt::WindowFlags(), 1);;

        if (ok) {timeF = QInputDialog::getDouble(this, tr("Bench data"), tr("Set finish time "), timeFinished, 0, 1000000, 3 , &ok, Qt::WindowFlags(), 1);}
        if (ok) {emit getImpulse(timeS, timeF);}
        break;
    }

    emit phaseChanged(phase);
}

void Bench::on_tareButton_clicked()
{
    if (phase < Phases::tared) {phase = Phases::tared;}
    emit dataToSend(tare, AppModes::benchMode);
}

void Bench::on_hydroStaticButton_clicked(bool status)
{
    if (status)
    {
        ui->hydroStaticButton->setText("Hydrostatic disabled");
        emit dataToSend(disableHydroStatic, AppModes::benchMode);
    }
    else
    {
        ui->hydroStaticButton->setText("Hydrostatic enabled");
        emit dataToSend(enableHydroStatic, AppModes::benchMode);
    }
    emit hydroStatic_triggered(!status);
}

/*
    if (phase < Phases::ready)
    {
        nTCouples = QInputDialog::getInt(this, "Selecting number of thermocouples", "Nº Thermocouples", nTCouples, 0, 12, 1);

        emit nTHChanged(nTCouples);
    }
    else
    {
        QMessageBox::question(this, "Too slow my friend", "You had to restart, function not available while started.", QMessageBox::Ok);
    }
*/

void Bench::on_transducerButton_clicked(bool checked)
{
    if (checked)
    {
        ui->transducerButton->setText("Transducer disabled");
        emit dataToSend(Orders::disableTransducer, AppModes::benchMode);
    }
    else
    {
        ui->transducerButton->setText("Transducer enabled");
        emit dataToSend(Orders::enableTransducer, AppModes::benchMode);
    }
}

void Bench::showImpulse(QString info)
{
    QMessageBox::information(this, "Bench data", info, QMessageBox::Ok);
}

void Bench::changeStylesheet(float factor)
{
    whiteButton.replace("borderRadius", QString::number(20*factor));
    whiteButton.replace("buttonHeight", QString::number(40*factor));
    ui->startButton->setStyleSheet(whiteButton);
    ui->tareButton->setStyleSheet(whiteButton);

    checkedButton.replace("borderRadius", QString::number(20*factor));
    checkedButton.replace("buttonHeight", QString::number(40*factor));
    ui->transducerButton->setStyleSheet(checkedButton);
    ui->hydroStaticButton->setStyleSheet(checkedButton);

    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->thrustFrame->setStyleSheet(frames);
    ui->pressureFrame->setStyleSheet(frames);
    ui->tempFrame->setStyleSheet(frames);
    frames.replace("QFrame", "#serialFrame");
    ui->serialFrame->setStyleSheet(frames);
    frames.replace("#serialFrame", "#orderFrame");
    ui->orderFrame->setStyleSheet(frames);

    ui->mainLayout->setSpacing(int(20*factor));
    ui->controlLayout->setSpacing(int(20*factor));
    ui->thrustpressureLayout->setSpacing(int(20*factor));
    ui->controltempLayout->setSpacing(int(20*factor));
    ui->buttonLayout->setSpacing(int(14*factor));

    ui->mainLayout->setContentsMargins(int(26*factor), int(26*factor), int(26*factor), int(26*factor));
}

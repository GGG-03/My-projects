#include "ignitors.h"
#include "ui_ignitors.h"

Ignitors::Ignitors(QWidget *parent) : QWidget(parent), ui(new Ui::Ignitors)
{
    ui->setupUi(this);

    // LCD setup
    ui->currentLCD->setDigitCount(7);
    ui->pressureLCD->setDigitCount(7);
    ui->tempLCD->setDigitCount(7);

    // Graphs setup
    ui->pressureGraph->addGraph();
    ui->currentGraph->addGraph();

    ui->pressureGraph->graph(0)->setAdaptiveSampling(true);
    ui->pressureGraph->setNoAntialiasingOnDrag(true);
    ui->currentGraph->graph(0)->setAdaptiveSampling(true);
    ui->currentGraph->setNoAntialiasingOnDrag(true);

    // Graph stylesheet
    ui->pressureGraph->xAxis->setLabel("Time[s]");
    ui->pressureGraph->yAxis->setLabel("Pressure[Bar]");
    ui->currentGraph->xAxis->setLabel("Time[s]");
    ui->currentGraph->yAxis->setLabel("Current[A]");

    QFont graphFont ("Consolas",11);
    ui->pressureGraph->xAxis->setTickLabelFont(graphFont);
    ui->pressureGraph->yAxis->setTickLabelFont(graphFont);
    ui->pressureGraph->xAxis->setLabelFont(graphFont);
    ui->pressureGraph->yAxis->setLabelFont(graphFont);
    ui->currentGraph->xAxis->setTickLabelFont(graphFont);
    ui->currentGraph->yAxis->setTickLabelFont(graphFont);
    ui->currentGraph->xAxis->setLabelFont(graphFont);
    ui->currentGraph->yAxis->setLabelFont(graphFont);
}

Ignitors::~Ignitors()
{
    delete ui;
}

void Ignitors::updateGauges(QVector<double> data)
{
    ui->pressureLCD->display(data[0]);
    ui->currentLCD->display(data[1]);
    ui->tempLCD->display(data[2]);
}

void Ignitors::plot(QVector<QVector<double>> data, QVector<double> maxData, char)
{
    ui->currentGraph->graph(0)->setData(data[0], data[1], true);
    ui->currentGraph->xAxis->setRange(0, maxData[0]);
    ui->currentGraph->yAxis->setRange(0, maxData[1] + 10);
    ui->currentGraph->replot();

    ui->pressureGraph->graph(0)->setData(data[0], data[2], true);
    ui->pressureGraph->xAxis->setRange(0, maxData[0]);
    ui->pressureGraph->yAxis->setRange(0, maxData[2] + 10);
    ui->pressureGraph->replot();
}

void Ignitors::on_startButton_clicked()
{

}

void Ignitors::resetTest()
{
    ui->startButton->setText("Start");
}

void Ignitors::saveGraph(QString nameFile)
{
    ui->pressureGraph->replot();
    ui->currentGraph->replot();
    nameFile.append("_pressureGraph.pdf");
    ui->pressureGraph->savePdf(nameFile);
    nameFile.replace("_pressureGraph.pdf", "_currentGraph.pdf");
    ui->currentGraph->savePdf(nameFile);
}

void Ignitors::changeStylesheet(float factor)
{
    whiteButtons.replace("borderRadius", QString::number(15*factor));
    ui->startButton->setStyleSheet(whiteButtons);

    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->pressureFrame->setStyleSheet(frames);
    ui->currentFrame->setStyleSheet(frames);
    frames.replace("QFrame", "#pressureLCDFrame");
    frames.append("QFrame { background-color: white; }");
    ui->pressureLCDFrame->setStyleSheet(frames);
    frames.replace("#pressureLCDFrame", "#currentLCDFrame");
    ui->currentLCDFrame->setStyleSheet(frames);
    frames.replace("#currentLCDFrame", "#tempLCDFrame");
    ui->tempLCDFrame->setStyleSheet(frames);

    ui->mainLayout->setSpacing(int(20*factor));
    ui->graphLayout->setSpacing(int(20*factor));
    ui->LCDLayout->setSpacing(int(20*factor));

    ui->mainLayout->setContentsMargins(int(20*factor), int(20*factor), int(20*factor), int(20*factor));
}

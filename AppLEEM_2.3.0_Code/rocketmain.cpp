#include "rocketmain.h"
#include "myconfig.h"
#include "ui_rocketmain.h"

#include <QVariant>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointF>
#include <QVector>
#include <QFile>
#include <QIODevice>
#include <QQuickItem>
#include <QQmlContext>
#include <Q3DTheme>

using namespace RocketSymb;

RocketMain::RocketMain(QWidget *parent): QWidget(parent), ui(new Ui::RocketMain)
{
    ui->setupUi(this);

    // Rocket controller
    // QML widgets setup
    ui->speed->setSource(QUrl(QStringLiteral("qrc:/qmlResources/speedGauge.qml")));
    ui->acc->setSource(QUrl(QStringLiteral("qrc:/qmlResources/accGauge.qml")));
    ui->gyroscope->setSource(QUrl(QStringLiteral("qrc:/qmlResources/Gyroscope.qml")));

    ui->speed->show();
    ui->acc->show();
    ui->gyroscope->show();

    // LCD setups
    ui->timeLCD->setDigitCount(7);
    ui->altitudLCD->setDigitCount(7);
    ui->maxAltitudLCD->setDigitCount(7);
    ui->statusLCD->setText(PhasesName[Phases::standby]);

    // LCD counters setup
    ui->LCD2Frame->setVisible(false);

    // Rockets controller
    // Map
    ui->map->setSource(QUrl(QStringLiteral("qrc:/qmlResources/map.qml")));
    ui->map->show();

    // 3D graph
    ui->altitudeStack->addWidget(altitude3DContainer);

    // 2D graph setup
    ui->altitudeGraph->addGraph();
    ui->altitudeGraph->addGraph();

    ui->altitudeGraph->graph(0)->setPen(QPen(QColor(40, 110, 132)));
    ui->altitudeGraph->graph(1)->setPen(QPen(QColor(255, 0, 0)));

    // Improves 2D graph performance
    ui->altitudeGraph->graph(0)->setAdaptiveSampling(true);
    ui->altitudeGraph->setNoAntialiasingOnDrag(true);
    ui->altitudeGraph->setNotAntialiasedElements(QCP::aeAll);

    // 2D graph stylesheet
    // Title
    QFont title ("Consolas", 12, QFont::Bold);
    QCPTextElement *altTitle = new QCPTextElement(ui->altitudeGraph);
    altTitle->setText("Rocket Altitude");
    altTitle->setFont(title);
    ui->altitudeGraph->plotLayout()->insertRow(0);
    ui->altitudeGraph->plotLayout()->addElement(0, 0, altTitle);

    // Axis
    ui->altitudeGraph->xAxis->setLabel("Time [s]");
    ui->altitudeGraph->yAxis->setLabel("Altitude [m]");

    QFont graphFont ("Consolas",11);
    ui->altitudeGraph->xAxis->setTickLabelFont(graphFont);
    ui->altitudeGraph->yAxis->setTickLabelFont(graphFont);
    ui->altitudeGraph->xAxis->setLabelFont(graphFont);
    ui->altitudeGraph->yAxis->setLabelFont(graphFont);

    // Legend
    ui->altitudeGraph->legend->setFont(graphFont);
    ui->altitudeGraph->graph(0)->setName("Altitude");
    ui->altitudeGraph->graph(1)->setName("Estimated Altitude");

    // 3D graph stylesheet
    Q3DTheme *currentTheme = altitudeScatter->activeTheme();
    currentTheme->setType(Q3DTheme::ThemeDigia);
    currentTheme->setColorStyle(Q3DTheme::ColorStyleUniform);
    currentTheme->setFont(graphFont);
    altitudeScatter->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    scatterData->setMeshSmooth(true);
    altitudeScatter->axisX()->setLabelFormat("%f");
    altitudeScatter->axisZ()->setLabelFormat("%f");
}

RocketMain::~RocketMain()
{
    delete ui;
}

void RocketMain::changeZoom(bool status)
{
    zoom = status;
}

void RocketMain::updateGauges(QVector<double> data)
{
    emit changeAcc              (data[Gauges::acceleration]);
    emit changeSpeed            (data[Gauges::speed]);
    emit changeRotX             (data[Gauges::rotX]);
    emit changeRotY             (data[Gauges::rotY]);
    emit changeRotZ             (data[Gauges::rotZ]);

    ui->altitudLCD->display     (data[Gauges::altitude]);
    ui->maxAltitudLCD->display  (data[Gauges::maxAltitude]);
    ui->timeLCD->display        (data[Gauges::time_seconds]);
    ui->statusLCD->setText      (PhasesName[(int) data[Gauges::status]]);
}

void RocketMain::updateTerminal(QStringListModel* data_show, QStringListModel* order_show)
{
    ui->serialPrint->setModel(data_show);
    ui->orderPrint->setModel(order_show);
}

void RocketMain::floatingTerminal_toggled(bool status)
{
    ui->orderFrame->setVisible(status);
}

void RocketMain::plot(QVector<QVector<double>> data, QVector<double> maxData, char nGraph)
{
    ui->altitudeGraph->graph(nGraph)->setData(data[0], data[1], true);
    ui->altitudeGraph->xAxis->setRange(0, maxData[0]);
    ui->altitudeGraph->yAxis->setRange(0, maxData[1] + 10);
    ui->maxAltitudLCD->display(maxData[1]);
    if (altitude3D == 0)
        ui->altitudeGraph->replot();
}

void RocketMain::showAltitude3D(bool show)
{
    if (show)
    {
        altitudeScatter->addSeries(scatterData);
        altitude3D = 1;
        altitudeScatter->seriesList().at(0)->setBaseColor(QColor(40, 110, 132));
    }
    else
    {
        altitudeScatter->removeSeries(scatterData);
        altitude3D = 0;
    }

    ui->altitudeStack->setCurrentIndex(altitude3D);   
}

void RocketMain::plot3D(double x, double y, double z)
{
    scatterData->dataProxy()->addItem(QVector3D(x, y, z));
}

void RocketMain::showLegend(bool show)
{
    ui->altitudeGraph->legend->setVisible(show);
    ui->altitudeGraph->replot();
}

void RocketMain::updateMap(double lat, double lng, bool zoom)
{
    emit addPathCoordinates(lat, lng, zoom);
}

void RocketMain::on_ping_button_clicked()
{
    emit dataToSend(ping, AppModes::rocketMode);
}

void RocketMain::on_phase_button_clicked()
{
    emit dataToSend(change_phase, AppModes::rocketMode);
}

void RocketMain::on_tare_button_clicked()
{
    emit dataToSend(tare, AppModes::rocketMode);
}

void RocketMain::on_alarm_button_clicked(bool status)
{
    if (status)
        emit dataToSend(alarm_on, AppModes::rocketMode);
    else
        emit dataToSend(alarm_off, AppModes::rocketMode);
}

void RocketMain::resetTest()
{
    ui->tare_button->setDisabled(false);
    checkedButton.replace("rgb(212,235,242)", "rgb(114,190,212)");
    ui->tare_button->setStyleSheet(checkedButton);
    ui->ping_button->setStyleSheet(checkedButton);
    ui->ping_button->setDisabled(false);

    scatterData->dataProxy()->removeItems(0, 100000);
    emit clearPath();
}

void RocketMain::showTerminal(bool show)
{
    ui->LCD2Frame->setVisible(!show);
    ui->serialFrame->setVisible(show);
    showingTerminal = show;
}

void RocketMain::saveGraph(QString nameFile)
{
    ui->altitudeGraph->replot();
    nameFile.append("_altitude2DGraph.pdf");
    ui->altitudeGraph->savePdf(nameFile);
}

void RocketMain::makeConnections()
{
    auto map = ui->map->rootObject();
    auto speed = ui->speed->rootObject();
    auto acc = ui->acc->rootObject();
    auto gyroscope = ui->gyroscope->rootObject();

    connect(this, SIGNAL(addPathCoordinates(QVariant, QVariant, QVariant)), map, SLOT(addPathCoordinates(QVariant, QVariant, QVariant)));
    connect(this, SIGNAL(clearPath()), map, SLOT(clearPath()));
    connect(this, SIGNAL(mark(QVariant, QVariant)), map, SLOT(addMarker(QVariant, QVariant)));
    connect(this, SIGNAL(changeSpeed(QVariant)), speed, SLOT(changeGaugeValue(QVariant)));
    connect(this, SIGNAL(changeAcc(QVariant)), acc, SLOT(changeGaugeValue(QVariant)));
    connect(this, SIGNAL(changeRotX(QVariant)), gyroscope, SLOT(changeRotationX(QVariant)));
    connect(this, SIGNAL(changeRotY(QVariant)), gyroscope, SLOT(changeRotationY(QVariant)));
    connect(this, SIGNAL(changeRotZ(QVariant)), gyroscope, SLOT(changeRotationZ(QVariant)));
    connect(this, SIGNAL(setZoomRocket(QVariant)), gyroscope, SLOT(setZoomRocket(QVariant)));
    emit setZoomRocket(45);
}

void RocketMain::changeStylesheet(float factor)
{
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->mapFrame->setStyleSheet(frames);
    frames.replace("QFrame", "#altitudeFrame");
    ui->altitudeFrame->setStyleSheet(frames);
    frames.replace("#altitudeFrame", "#serialFrame");
    ui->serialFrame->setStyleSheet(frames);
    frames.replace("#serialFrame", "#orderFrame");
    ui->orderFrame->setStyleSheet(frames);

    whiteButton.replace("borderRadius", QString::number(20*factor));
    whiteButton.replace("buttonHeight", QString::number(40*factor));
    ui->tare_button->setStyleSheet(whiteButton);
    ui->ping_button->setStyleSheet(whiteButton);
    ui->phase_button->setStyleSheet(whiteButton);

    checkedButton.replace("borderRadius", QString::number(20*factor));
    checkedButton.replace("buttonHeight", QString::number(40*factor));
    ui->alarm_button->setStyleSheet(checkedButton);

    ui->mainLayout->setSpacing(int(20*factor));
    ui->dataLayout->setSpacing(int(20*factor));
    ui->mapLayout->setSpacing(int(20*factor));
    ui->buttonLayout->setVerticalSpacing(int(14*factor));
    ui->buttonLayout->setHorizontalSpacing(int(20*factor));
    ui->artifactsLayout->setContentsMargins(int(20*factor), int(10*factor), int(20*factor), 0);
    ui->artifactsLayout->setSpacing(int(15*factor));
    ui->gaugesLayout->setHorizontalSpacing(int(20*factor));
    ui->gaugesLayout->setVerticalSpacing(int(20*factor));
    ui->LCDLayout->setSpacing(int(20*factor));

    ui->mainLayout->setContentsMargins(int(20*factor), int(20*factor), int(20*factor), int(20*factor));
}

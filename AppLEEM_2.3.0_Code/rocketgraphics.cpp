#include "rocketgraphics.h"
#include "ui_rocketgraphics.h"

#include <QQuickItem>
#include <QQmlContext>
#include <Q3DTheme>
#include <QStringListModel>

RocketGraphics::RocketGraphics(QWidget *parent) : QWidget(parent), ui(new Ui::RocketGraphics)
{
    ui->setupUi(this);

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

RocketGraphics::~RocketGraphics()
{
    delete ui;
}

void RocketGraphics::changeZoom(bool status)
{
    zoom = status;
}

void RocketGraphics::updateTerminal(QStringListModel* data_show, QStringListModel* order_show)
{
    ui->serialPrint->setModel(data_show);
    ui->orderPrint->setModel(order_show);
}

void RocketGraphics::floatingTerminal_toggled(bool status)
{
    ui->orderFrame->setVisible(status);
}

void RocketGraphics::plot(QVector<QVector<double>> data, QVector<double> maxData, char nGraph)
{
    ui->altitudeGraph->graph(nGraph)->setData(data[0], data[1]);
    ui->altitudeGraph->xAxis->setRange(0, maxData[0]);
    ui->altitudeGraph->yAxis->setRange(0, maxData[1] + 10);
    if (altitude3D == 0) {ui->altitudeGraph->replot();}
}

void RocketGraphics::showAltitude3D(bool show)
{
    if (show)
    {
        altitudeScatter->addSeries(scatterData);
        altitudeScatter->seriesList().at(0)->setBaseColor(QColor(40, 110, 132));
        altitude3D = 1;
    }
    else
    {
        altitudeScatter->removeSeries(scatterData);
        altitude3D = 0;
    }

    ui->altitudeStack->setCurrentIndex(altitude3D);
}

void RocketGraphics::plot3D(double x, double y, double z)
{
    scatterData->dataProxy()->addItem(QVector3D(x, y, z));
}

void RocketGraphics::showLegend(bool show)
{
    ui->altitudeGraph->legend->setVisible(show);
    ui->altitudeGraph->replot();
}

void RocketGraphics::updateMap(double lat, double lng, bool zoom)
{
    emit addPathCoordinates(lat, lng, zoom);
}

void RocketGraphics::resetTest()
{
    scatterData->dataProxy()->removeItems(0, 100000);
    emit clearPath();
}

void RocketGraphics::showTerminal(bool show)
{
    ui->serialFrame->setVisible(show);
}

void RocketGraphics::saveGraph(QString nameFile)
{
    ui->altitudeGraph->replot();
    nameFile.append("_altitude2DGraph.pdf");
    ui->altitudeGraph->savePdf(nameFile);
}

void RocketGraphics::makeConnections()
{
    auto map = ui->map->rootObject();

    connect(this, SIGNAL(mark(QVariant, QVariant)), map, SLOT(addMarker(QVariant, QVariant)));
    connect(this, SIGNAL(addPathCoordinates(QVariant, QVariant, QVariant)), map, SLOT(addPathCoordinates(QVariant, QVariant, QVariant)));
    connect(this, SIGNAL(clearPath()), map, SLOT(clearPath()));
}

void RocketGraphics::changeStylesheet(float factor)
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

    ui->mainLayout->setSpacing(int(20*factor));
    ui->printFrame->setSpacing(int(20*factor));
    ui->bothLayout->setSpacing(int(20*factor));

    ui->mainLayout->setContentsMargins(int(20*factor), int(20*factor), int(20*factor), int(20*factor));
}

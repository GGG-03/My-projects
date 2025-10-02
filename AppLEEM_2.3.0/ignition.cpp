#include "ignition.h"
#include "ui_ignition.h"
#include "myconfig.h"

#include <QDebug>

Ignition::Ignition(QWidget *parent) : QDialog(parent), ui(new Ui::Ignition)
{
    ui->setupUi(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateBar()));
}

Ignition::~Ignition()
{
    delete ui;
}

void Ignition::countDown()
{
    ui->pushButton->setEnabled(false);
    ui->progressBar->setStyleSheet(bar);
    count = 0;
    timer->start(100);
    this->exec();
}

void Ignition::hydroStatic_triggered(bool enabled)
{
    hydroStatic_enabled = enabled;
}

void Ignition::updateBar()
{
    // Updates the button timer as well as the bar
    ui->pushButton->setText(QString::number(10 - ++count / 10.));
    ui->progressBar->setValue(20 + count * .8);

    // When finishes
    if (count == 100 || hydroStatic_enabled)
    {
        timer->stop();
        ui->progressBar->setStyleSheet(filledBar);
        ui->pushButton->setEnabled(true);
        ui->pushButton->setText("Ignition");
    }
}

void Ignition::on_pushButton_clicked()
{
    emit launching(BenchSymb::Orders::startIgnition, AppModes::benchMode);
    this->hide();
}

void Ignition::changeStylesheet(float factor)
{
    this->resize(factor*640, factor*640);

    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->frame->setStyleSheet(frames);

    buttons.replace("borderRadius", QString::number(200*factor));
    buttons.replace("buttonWidth", QString::number(400*factor));
    buttons.replace("buttonHeight", QString::number(400*factor));
    ui->pushButton->setStyleSheet(buttons);

    bar.replace("barHeight", QString::number(60*factor));
    bar.replace("barWidth", QString::number(500*factor));
    bar.replace("barBorder", QString::number(30*factor));
    bar.replace("borderRadius", QString::number(32*factor));
    ui->progressBar->setStyleSheet(bar);
    filledBar = bar;
    filledBar.replace("transparent", "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 255, 0, 213), stop:1 rgba(255, 0, 0, 255))");

    ui->progressBar->setVisible(true);

    ui->gridLayout->setContentsMargins(int(20*factor), int(20*factor), int(20*factor), int(20*factor));

    ui->gridLayout_2->setContentsMargins(int(30*factor), int(30*factor), int(30*factor), int(30*factor));
    ui->gridLayout_2->setVerticalSpacing(int(50*factor));
}

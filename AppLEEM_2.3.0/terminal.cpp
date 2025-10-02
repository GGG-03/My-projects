#include "terminal.h"
#include "ui_terminal.h"

Terminal::Terminal(QWidget *parent) : QDialog(parent), ui(new Ui::Terminal)
{
    ui->setupUi(this);
}

Terminal::~Terminal()
{
    delete ui;
}

void Terminal::closeEvent(QCloseEvent*)
{
    this->hide();
    emit closingTerminal(false);
}

void Terminal::updateTerminal(QStringListModel* data_show, QStringListModel* order_show)
{
    ui->serial_print->setModel(data_show);
    ui->order_print->setModel(order_show);
}

void Terminal::changeStylesheet(float factor)
{
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->serial_frame->setStyleSheet(frames);
    frames.replace("#serial_frame", "#order_frame");
    ui->order_frame->setStyleSheet(frames);

    ui->mainLayout->setSpacing(int(20*factor));

    this->resize(factor*960, factor*360);
}

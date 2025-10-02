#include "helpdialog.h"
#include "ui_helpdialog.h"
#include "myconfig.h"

#include <QFile>
#include <QDebug>

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent), ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    ui->instructions->setReadOnly(true);

    // Read each file with the instructions content
    for (int i = 0; i < 3; ++i)
    {
        QFile prueba(":/instructions/" + fileNames[i] + ".txt");
        QTextStream out(&prueba);
        out.setCodec("UTF-8");
        if (prueba.open(QIODevice::ReadOnly))
        {
            fileGuides[i] = out.readAll();
            prueba.close();
        }
    }

    // Set by default the default
    on_generalButton_clicked();
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::show_help()
{
    this->show();
}

void HelpDialog::on_generalButton_clicked()
{
   ui->instructions->setPlainText(fileGuides[AppModes::defaultMode]);
}

void HelpDialog::on_benchButton_clicked()
{
    ui->instructions->setPlainText(fileGuides[AppModes::benchMode]);
}

void HelpDialog::on_launchpadButton_clicked()
{
    ui->instructions->setPlainText(fileGuides[AppModes::rocketMode]);
}

void HelpDialog::changeStylesheet(float factor)
{
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->frame->setStyleSheet(frames);

    whiteButtons.replace("borderRadius", QString::number(14*factor));
    whiteButtons.replace("buttonHeight", QString::number(30*factor));
    ui->generalButton->setStyleSheet(whiteButtons);
    ui->benchButton->setStyleSheet(whiteButtons);
    ui->launchpadButton->setStyleSheet(whiteButtons);

    this->resize(factor*640, factor*480);
}

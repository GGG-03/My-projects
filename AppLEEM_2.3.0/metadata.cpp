#include "metadata.h"
#include "ui_metadata.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

Metadata::Metadata(QWidget *parent) : QDialog(parent), ui(new Ui::Metadata)
{
    ui->setupUi(this);
}

Metadata::~Metadata()
{
    delete ui;
}

void Metadata::on_doneButton_clicked()
{
    this->hide();
}

void Metadata::closeEvent(QCloseEvent*)
{
    if (waiting)
    {
        orderToSave(oldName);
        waiting = false;
    }
}

void Metadata::orderToSave(QString newName)
{
    oldName = newName;

    // In case they forgot to edit metadata
    if (!waiting)
    {
        oldName.append("_metadata.txt");
        if (QMessageBox::Yes == QMessageBox::question(this, "Metadata", "Do you want to edit metadata?", QMessageBox::Yes | QMessageBox::No))
        {
            // We wait until the metadata has been edited
            waiting = true;
            this->exec();
        }
    }

    // We create a file and store the metadata there
    QFile file(oldName);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << ui->specsEdit->toPlainText();
    }
}

void Metadata::cleanMetadata()
{
    ui->specsEdit->setPlainText("");
}

void Metadata::putImpulse(QString info)
{
    // Writes bench data
    ui->specsEdit->textCursor().movePosition(QTextCursor::End);
    ui->specsEdit->textCursor().insertText("\n"+ info);
}

void Metadata::changeStylesheet(float factor)
{
    frames.replace("borderRadius", QString::number(30*factor));
    frames.replace("borderSize", QString::number(5*factor));
    ui->frame->setStyleSheet(frames);

    whiteButtons.replace("borderRadius", QString::number(14*factor));
    whiteButtons.replace("buttonHeight", QString::number(30*factor));
    ui->doneButton->setStyleSheet(whiteButtons);

    this->resize(factor*640, factor*480);
}

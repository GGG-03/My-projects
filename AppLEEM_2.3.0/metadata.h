#ifndef METADATA_H
#define METADATA_H

#include <QDialog>

namespace Ui
{
    class Metadata;
}

class Metadata : public QDialog
{
    Q_OBJECT

    public:
        explicit Metadata(QWidget *parent = nullptr);
        ~Metadata();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::Metadata *ui;

        bool waiting = false;
        QString oldName;

        void closeEvent(QCloseEvent *bar);

        // StyleSheet
        QString frames =  "#frame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString whiteButtons = "QPushButton { height: buttonHeight; border-radius: borderRadius%; background-color: white; }";

    public slots:
        void orderToSave(QString);
        void cleanMetadata();

    private slots:
        void on_doneButton_clicked();
        void putImpulse(QString);

};

#endif // METADATA_H

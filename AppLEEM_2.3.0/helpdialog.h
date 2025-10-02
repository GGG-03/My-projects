#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui
{
    class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit HelpDialog(QWidget *parent = nullptr);
        ~HelpDialog();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::HelpDialog *ui;

        QString fileNames[3] = {"helpGeneral", "helpBench", "helpLaunch"};
        QString fileGuides[3] = {""};

        // StyleSheet
        QString frames =  "#frame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString whiteButtons = "QPushButton { height: buttonHeight; border-radius: borderRadius%; background-color: white; }";

    private slots:
        void show_help();

        void on_generalButton_clicked();
        void on_benchButton_clicked();
        void on_launchpadButton_clicked();
};

#endif // HELPDIALOG_H

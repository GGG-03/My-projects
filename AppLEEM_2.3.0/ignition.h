#ifndef IGNITION_H
#define IGNITION_H

#include <QDialog>
#include <QTimer>

namespace Ui
{
    class Ignition;
}

class Ignition : public QDialog
{
    Q_OBJECT

    public:
        explicit Ignition(QWidget *parent = nullptr);
        ~Ignition();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::Ignition *ui;

        QTimer *timer = new QTimer(this);
        int count = 0;
        bool hydroStatic_enabled = false;

        // StyleSheet
        QString frames = "#frame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";
        QString buttons = "QPushButton { height: buttonHeight; width: buttonWidth; border: 6px solid black; border-radius: borderRadiuspx; background-color: rgb(207,0,0); }";
        QString bar = "QProgressBar { height: barHeightpx; width: barWidthpx; background-color: transparent; border-radius: borderRadiuspx; border: 6px solid black; }  QProgressBar::chunk { border-radius: barBorderpx; background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 255, 0, 213), stop:0.215909 rgba(0, 255, 0, 218), stop:0.647727 rgba(255, 255, 0, 255), stop:0.977273 rgba(255, 0, 0, 255)); }";
        QString filledBar = "";

    public slots:
        void countDown();
        void hydroStatic_triggered(bool);

    private slots:
        void updateBar();
        void on_pushButton_clicked();

    signals:
        void launching(char, char);

};

#endif // IGNITION_H

#ifndef POWERMODULE_H
#define POWERMODULE_H

#include <QWidget>

namespace Ui {
class PowerModule;
}

class PowerModule : public QWidget
{
    Q_OBJECT

    public:
        explicit PowerModule(QWidget *parent = nullptr);
        ~PowerModule();

        void changeStylesheet(float);

    private slots:
        void updateLCDs(QVector<double> values);

    private:
        Ui::PowerModule *ui;
        QString frames = "#battery_frame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white;} QFrame {background-color: transparent;}";
        QString bar = "QProgressBar { height: barHeightpx; width: barWidthpx; background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0.15, y2:1, stop:0 rgba(19, 136, 8, 255), stop:1 rgba(255, 255, 255, 255)); border-radius: borderRadiuspx; border: 6px solid black; } QProgressBar::chunk { background-color: rgba(19, 136, 8, 255); border-radius: barBorderpx;}";
};

#endif // POWERMODULE_H

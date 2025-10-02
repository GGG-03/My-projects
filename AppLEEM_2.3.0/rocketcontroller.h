#ifndef ROCKETCONTROLLER_H
#define ROCKETCONTROLLER_H

#include <QWidget>
#include <QVariant>
#include <QStringListModel>

namespace Ui
{
    class RocketController;
}

class RocketController : public QWidget
{
    Q_OBJECT

    public:
        explicit RocketController(QWidget *parent = nullptr);
        ~RocketController();

        bool testing;

        void makeConnections();

        // Reset
        void resetTest();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::RocketController *ui;

        bool launchStarted = false;
        bool readyToLaunch = false;
        bool rocketLanding = false;
        bool landed = false;

        // StyleSheet
        QString whiteButton = "QPushButton { height: buttonHeightpx; border-radius: borderRadiuspx; background-color: white; color: black; }";
        QString checkedButton = "QPushButton { height: buttonHeightpx; border-radius: borderRadiuspx; background-color: white; color: black; }  QPushButton:checked { background-color: rgb(212,235,242); }";

    private slots:
        // Buttons
        void on_ping_button_clicked();
        void on_phase_button_clicked();
        void on_tare_button_clicked();
        void on_alarm_button_clicked(bool);

        // Gauges
        void updateGauges(QVector<double>);

    signals:
        void dataToSend(char, char);

        void changeSpeed(QVariant);
        void changeAcc(QVariant);
        void changeRotX(QVariant);
        void changeRotY(QVariant);
        void changeRotZ(QVariant);
        void setZoomRocket(QVariant);
};

#endif // ROCKETCONTROLLER_H

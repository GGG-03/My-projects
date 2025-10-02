#ifndef TERMINAL_H
#define TERMINAL_H

#include <QDialog>
#include <QStringListModel>

namespace Ui
{
    class Terminal;
}

class Terminal : public QDialog
{
    Q_OBJECT

    public:
        explicit Terminal(QWidget *parent = nullptr);
        ~Terminal();

        // Stylesheet
        void changeStylesheet(float);

    private:
        Ui::Terminal *ui;

        void closeEvent(QCloseEvent *bar);

        // Stylesheet
        QString frames = "#serial_frame { border-radius: borderRadiuspx; border: borderSizepx solid rgb(212,235,242); background-color: white; } ";

    private slots:
        // Terminal
        void updateTerminal(QStringListModel*, QStringListModel*);

    signals:
        void closingTerminal(bool);

};

#endif // TERMINAL_H

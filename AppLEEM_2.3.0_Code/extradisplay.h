#ifndef EXTRADISPLAY_H
#define EXTRADISPLAY_H

#include <QWidget>
#include <QMenuBar>

namespace Ui
{
    class ExtraDisplay;
}

class ExtraDisplay : public QWidget
{
    Q_OBJECT

    public:
        explicit ExtraDisplay(QWidget *parent = nullptr);
        ~ExtraDisplay();

        // Stylesheet
        void changeStylesheet(float factor);

    private:
        Ui::ExtraDisplay *ui;

        QMenuBar* menuBar = new QMenuBar();
        void closeEvent(QCloseEvent *bar);

        // Drag tab
        QWidget *draggedWidget;
        QString draggedText;
        QPointF draggedPos;
        int draggedIndex;

        // Stylesheet
        QString tabStyleSheet = "QWidget { background-color: transparent; }  QTabWidget::pane { background-color: transparent; color: black; }  QTabBar::tab { background-color: rgb(134,199,218); color: black; height: tabHeightpx; width: tabWidthpx; }  QTabBar::tab:hover { background-color: rgb(153,208,224); color: white; }  QTabBar::tab:selected { background-color: rgb(173,217,230); color: black; }";
        QString menuStyleSheet = "QMenu { background-color: rgb(134,199,218); }  QMenuBar { background-color: rgb(95,178,206); min-height: menuHeight%; }  QMenuBar::item:selected { background-color: rgb(134,199,218); }  QMenuBar::item:pressed { background-color: rgb(134,199,218); }";

    private slots:
        // Drag tab functions
        void receivingDragged(QWidget*, QString, QPointF);
        void on_tabWidget_tabBarClicked(int index);
        void dragEnterEvent(QDragEnterEvent*);
        void dragMoveEvent(QDragMoveEvent*);
        void dropEvent(QDropEvent*);

    public slots:
        void loadTab(QWidget *dir, QString name);
        void clearTab();

    signals:
        void closing(bool);
        void aboutToBeDragged(QWidget*, QString, QPointF);
};

#endif // EXTRADISPLAY_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bench.h"
#include "benchgraphs.h"
#include "dataprocessor.h"
#include "extradisplay.h"
#include "helpdialog.h"
#include "ignition.h"
#include "ignitors.h"
#include "metadata.h"
#include "myconfig.h"
#include "powermodule.h"
#include "rocketcontroller.h"
#include "rocketgraphics.h"
#include "rocketmain.h"
#include "serialconnector.h"
#include "switch.h"
#include "terminal.h"

#include <QMainWindow>
#include <QAction>
#include <QThread>
#include <QFont>
#include <QCloseEvent>
#include <QMessageBox>
#include <QAbstractButton>
#include <QUiLoader>
#include <QScreen>
#include <QDebug>
#include <QSysInfo>
#include <QFileDialog>
#include <QWidgetAction>
#include <QScreen>
#include <QDesktopWidget>

QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        // Threads
        QThread serialThread;

        // Widgets
        Ignition *ignition = new Ignition;
        ExtraDisplay *secondDisplay = new ExtraDisplay;
        SerialConnector *portDisplay = new SerialConnector;
        Metadata *metadata = new Metadata;
        Terminal *floatingTerminal = new Terminal;
        HelpDialog *helpWindow = new HelpDialog;

        // Processor
        DataProcessor *processor = new DataProcessor;

        // Bench
        Bench *bench = new Bench;
        BenchGraphs *spBench = new BenchGraphs;

        // Rocket
        RocketController *rocketControl = new RocketController;
        RocketGraphics *rocketGraphs = new RocketGraphics;
        RocketMain *rocketGround = new RocketMain;

        // Battery monitors
        PowerModule *powerPCB = new PowerModule;

        // Ignitors
        Ignitors *ignitors = new Ignitors;

    private:
        Ui::MainWindow *ui;

        int appMode = 0;
        float dpi = 72.0;
        float screenFactor = 0;
        float scaleSize = 1;

        // FPS
        int fps = 50;
        QStringList FPSList = {"20", "30", "40", "50"};

        // Switches and actions
        void createSwitches();
        void createJustOneSwitch(Switch*, QWidgetAction*, QMenu*, bool);

        Switch *testingSwitch = new Switch("  Testing  ");
        QWidgetAction *testingSwitchAction;

        Switch *floatTSwitch = new Switch("  Floating Terminal  ");
        QWidgetAction *floatTSwitchAction;

        Switch *zoomSwitch = new Switch("  Follow  ");
        QWidgetAction *zoomSwitchAction;

        Switch *altitude3DSwitch = new Switch("  3D  ");
        QWidgetAction *altitude3DSwitchAction;

        Switch *serialBSwitch = new Switch("  Terminal  ");
        QWidgetAction *serialBSwitchAction;
        Switch *serialTSwitch = new Switch("  Terminal  ");
        QWidgetAction *serialTSwitchAction;

        Switch *tempSwitch = new Switch("  Temperatures  ");
        QWidgetAction *tempSwitchAction;

        Switch *pressureSwitch = new Switch("  Pressure  ");
        QWidgetAction *pressureSwitchAction;

        Switch *dualSwitch = new Switch("  Dual Screen  ");
        QWidgetAction *dualSwitchAction;

        Switch *legendBSwitch = new Switch("  Legend  ");
        QWidgetAction *legendBSwitchAction;

        Switch *legendTSwitch = new Switch("  Legend  ");
        QWidgetAction *legendTSwitchAction;

        Switch *thrustInNSwitch = new Switch("  Thrust in N  ");
        QWidgetAction *thrustInNSwitchAction;

        // Create menu buttons     
        void createMenuButtons();

        QAction *connectBoard = new QAction(" Connect ");
        QAction *reset = new QAction(" Reset ");
        QAction *help = new QAction(" Help ");
        QAction *actionFPS = new QAction("  Frame Rate ");

        QAction *actionBench = new QAction("  Bench ");
        QAction *actionLaunchPad = new QAction("  LaunchPad ");
        QAction *actionIgnitors = new QAction("  Ignitors ");
        QAction *actionIgnition = new QAction("  Ignition ");

        // Variables
        bool testing = true;
        bool secondDisplayOn = false;
        bool connected = false;
        bool tempGraphStatus = false;
        bool pressureGraphStatus = true;
        bool altitude3D = false;

        // Date
        QDateTime dated = QDateTime::currentDateTime();
        QString nameFile = "../" + dated.toString("dd_MM_yyyy") + "_Ensayo_";
        int nFile = 0;

        // Drag tab
        QWidget *draggedWidget;
        QString draggedText;
        QPointF draggedPos;
        int draggedIndex;

        // StyleSheet
        QString tabStyleSheet = "QWidget { background-color: transparent; }  QTabWidget::pane { background-color: transparent; color: black; }  QTabBar::tab { background-color: rgb(134,199,218); color: black; height: tabHeightpx; width: tabWidthpx; }  QTabBar::tab:hover { background-color: rgb(153,208,224); color: white; }  QTabBar::tab:selected { background-color: rgb(173,217,230); color: black; }";
        QString menuStyleSheet = "QMenu { background-color: rgb(134,199,218); }  QMenuBar { background-color: rgb(95,178,206); min-height: barmenuHeight%; }  QMenuBar::item:selected { background-color: rgb(134,199,218); }  QMenuBar::item:pressed { background-color: rgb(134,199,218); }  QMenu::item { margin: 0px; height: menuHeight%; }";

        void changeStylesheet(float);

    private slots:
        void closeEvent(QCloseEvent *event);

        void mainMode(int);
        void addNewController(QString, uint32_t, uint8_t);
        void reset_test();

        // Tab changed
        void on_tabWidget_currentChanged(int index);

        // File error
        void fileError(int);

        // Drag tab functions
        void receivingDragged(QWidget*, QString, QPointF);
        void on_tabWidget_tabBarClicked(int index);
        void dragEnterEvent(QDragEnterEvent*);
        void dragMoveEvent(QDragMoveEvent*);
        void dropEvent(QDropEvent*);

        // Menu functions
        void bench_triggered();
        void launchPad_triggered();
        void ignitors_triggered();
        void ignition_triggered();
        void fpsMenu_triggered();

        void on_actionSave_triggered();
        void on_actionDecodifiy_triggered();
        void on_actionCompare_triggered();

        void connect_serial();
        void float_triggered(bool status);
        void dual_triggered(bool status);

        // Switches function
        void testingMode(bool status);
        void altitude3D_triggered(bool status);
        void temp_triggered(bool status);
        void pressure_triggered(bool status);

    signals:
        void floatingTerminal_toggled(bool status);
        void showWidgets(int);
        void changeFPS(int);
        void aboutToBeDragged(QWidget*, QString, QPointF);
        void connectArduino(QString, uint32_t);

};
#endif // MAINWINDOW_H

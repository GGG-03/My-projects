#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(QVector<QVector<double>>)
Q_DECLARE_METATYPE(uint32_t)

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    qRegisterMetaType<QVector<QVector<double>> >("QVector<QVector<double>>");
    qRegisterMetaType<uint32_t>("uint32_t");

    ui->setupUi(this);
    setAcceptDrops(true);

    // Threads
    processor->moveToThread(&serialThread); // Moves the data processor to a different thread, so there is a main thread (UI) and a data thread
    connect(&serialThread, SIGNAL(started()), processor, SLOT(startUp()));
    serialThread.start();
    QThread::currentThread()->setPriority(QThread::HighPriority);

    // Stylesheet
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    screenFactor = screenGeometry.width() / 1920.;

    if (QSysInfo::productType() == "windows")  {dpi = 96.0;}
    else if (QSysInfo::productType() == "osx") {dpi = 72.0;}

    scaleSize = qApp->desktop()->logicalDpiX()/dpi; // Calculates scaling ratio

    // Sets the style for all windows
    changeStylesheet(scaleSize);
    secondDisplay->changeStylesheet(scaleSize);
    bench->changeStylesheet(scaleSize);
    spBench->changeStylesheet(scaleSize);
    rocketGround->changeStylesheet(scaleSize);
    rocketGraphs->changeStylesheet(scaleSize);
    rocketControl->changeStylesheet(scaleSize);
    portDisplay->changeStylesheet(scaleSize);
    ignitors->changeStylesheet(scaleSize);
    metadata->changeStylesheet(scaleSize);
    floatingTerminal->changeStylesheet(scaleSize);
    helpWindow->changeStylesheet(scaleSize);
    ignition->changeStylesheet(scaleSize);
    powerPCB->changeStylesheet(scaleSize);

    // Create switches
    createSwitches();

    // Create menu buttons
    createMenuButtons();

    // Establish main connections
    connect(portDisplay, SIGNAL(connectController(QString, uint32_t, uint8_t)), this, SLOT(addNewController(QString, uint32_t, uint8_t)));
    connect(this, SIGNAL(changeFPS(int)), processor, SLOT(changeFPS(int)));
    connect(secondDisplay, SIGNAL(closing(bool)), this, SLOT(dual_triggered(bool))); // Connects the floating terminal
    connect(ignition, SIGNAL(launching(char, char)), processor->mainSerial, SLOT(sendOrdersSerial(char, char))); // Connects the ignition part
    connect(ui->metadataEdit, SIGNAL(triggered()), metadata, SLOT(open()));

    connect(this, SIGNAL(aboutToBeDragged(QWidget*, QString, QPointF)), secondDisplay, SLOT(receivingDragged(QWidget*, QString, QPointF))); // Connects the drag
    connect(secondDisplay, SIGNAL(aboutToBeDragged(QWidget*, QString, QPointF)), this, SLOT(receivingDragged(QWidget*, QString, QPointF))); // between screens

    // Hide function menus
    ui->rocketWidgets->menuAction()->setVisible(false);
    ui->benchWidgets->menuAction()->setVisible(false);

    // Create data folder if not created
    QDir::current().mkdir("data");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mainMode(int mode)
{
    // Disables all possible connections without including the main ones
    processor->disconnect();
    rocketGround->disconnect();
    rocketControl->disconnect();
    rocketGraphs->disconnect();
    bench->disconnect();
    spBench->disconnect();
    ignition->disconnect();

    // Disables conexions and reboots switches

    // Hides the exclusive menus
    ui->benchWidgets->menuAction()->setVisible(false);
    ui->rocketWidgets->menuAction()->setVisible(false);

    connect(processor, SIGNAL(fileNotOpened(int)), this, SLOT(fileError(int))); // Connects the file error warning
    connect(processor, SIGNAL(updateTerminal(QStringListModel*, QStringListModel*)), floatingTerminal, SLOT(updateTerminal(QStringListModel*, QStringListModel*)));
    connect(ignition, SIGNAL(launching(char, char)), processor->mainSerial, SLOT(sendOrdersSerial(char, char))); // Connects the ignition part

    // Changes the appmode
    appMode = mode;
    processor->changeMode(mode);

    // Sets the correct background
    ui->centralwidget->setStyleSheet("background-color: rgb(134,199,218);");
    tabStyleSheet.replace("transparent", "rgb(173,217,230)");
    ui->tabWidget->setStyleSheet(tabStyleSheet);

    // Clear tabs
    ui->tabWidget->clear();
    secondDisplay->clearTab();

    switch (mode)
    {
    case AppModes::benchMode:
        // Makes all the needed connections
        connect(bench, SIGNAL(dataToSend(char, char)), processor->mainSerial, SLOT(sendOrdersSerial(char, char)));
        connect(bench, SIGNAL(phaseChanged(int)), processor, SLOT(phaseChanged(int)));
        connect(processor, SIGNAL(plot(QVector<QVector<double>>, QVector <double>, char)), bench, SLOT(plot(QVector<QVector<double>>, QVector <double>, char)));
        connect(processor, SIGNAL(updateTerminal(QStringListModel*, QStringListModel*)), bench, SLOT(updateTerminal(QStringListModel*, QStringListModel*)));
        connect(processor, SIGNAL(sendDecreasedThrust(double, double)), bench, SLOT(recieveDecreasedThrust(double, double)));
        connect(processor, SIGNAL(showImpulse(QString)), bench, SLOT(showImpulse(QString)));
        connect(processor, SIGNAL(showImpulse(QString)), metadata, SLOT(putImpulse(QString)));
        connect(bench, SIGNAL(getImpulse(double, double)), processor, SLOT(getImpulse(double, double)));
        connect(bench, SIGNAL(startingLaunch()), ignition, SLOT(countDown()));
        connect(bench, SIGNAL(nTHChanged(int)), processor, SLOT(nTHChanged(int)));
        connect(bench, SIGNAL(nTHChanged(int)), spBench, SLOT(nTHChanged(int)));
        connect(bench, SIGNAL(hydroStatic_triggered(bool)), ignition, SLOT(hydroStatic_triggered(bool)));

        // Switches and menus
        connect(this, SIGNAL(showWidgets(int)), bench, SLOT(showWidgets(int)));
        connect(this, SIGNAL(showWidgets(int)), spBench, SLOT(showWidgets(int)));
        connect(this, SIGNAL(floatingTerminal_toggled(bool)), bench, SLOT(floatingTerminal_toggled(bool)));

        // In case of a dual window, it puts the least important information in the second window by default
        if (secondDisplayOn)
        {
            connect(processor, SIGNAL(plot(QVector<QVector<double>>, QVector <double>, char)), spBench, SLOT(plot(QVector<QVector<double>>, QVector <double>, char)));

            ui->tabWidget->addTab(bench, QIcon(QString("")), "Bench");
            if (tempGraphStatus || pressureGraphStatus) secondDisplay->loadTab(spBench, "Chamber pressure");
        }
        else
        {
            ui->tabWidget->addTab(bench, QIcon(QString("")), "Bench");
        }

        // Shows the bench menu
        ui->benchWidgets->menuAction()->setVisible(true);
        break;

    case AppModes::rocketMode:
        connect(processor, SIGNAL(updatePower(QVector<double>)), powerPCB, SLOT(updateLCDs(QVector<double>)));

        if (secondDisplayOn)
        {
            rocketControl->makeConnections();
            rocketGraphs->makeConnections();

            connect(this, SIGNAL(floatingTerminal_toggled(bool)), rocketGraphs, SLOT(floatingTerminal_toggled(bool)));

            connect(rocketControl, SIGNAL(dataToSend(char, char)), processor->mainSerial, SLOT(sendOrdersSerial(char, char)));
            connect(processor, SIGNAL(mark(QVariant, QVariant)), rocketGraphs, SIGNAL(mark(QVariant, QVariant)));
            connect(processor, SIGNAL(updateMap(double, double, bool)), rocketGraphs, SLOT(updateMap(double, double, bool)));
            connect(processor, SIGNAL(plot(QVector<QVector<double>>, QVector <double>, char)), rocketGraphs, SLOT(plot(QVector<QVector<double>>, QVector <double>, char)));
            connect(processor, SIGNAL(updateTerminal(QStringListModel*, QStringListModel*)), rocketGraphs, SLOT(updateTerminal(QStringListModel*, QStringListModel*)));
            connect(processor, SIGNAL(updateGauges(QVector<double>)), rocketControl, SLOT(updateGauges(QVector<double>)));
            connect(processor, SIGNAL(plot3D(double, double, double)), rocketGraphs, SLOT(plot3D(double, double, double)));

            ui->tabWidget->addTab(rocketGraphs, QIcon(QString("")), "Graphs");
            secondDisplay->loadTab(rocketControl, "Control");
            secondDisplay->loadTab(powerPCB, "Power monitor");
        }
        else
        {
            rocketGround->makeConnections();

            connect(this, SIGNAL(floatingTerminal_toggled(bool)), rocketGround, SLOT(floatingTerminal_toggled(bool)));

            connect(rocketGround, SIGNAL(dataToSend(char, char)), processor->mainSerial, SLOT(sendOrdersSerial(char, char)));
            connect(processor, SIGNAL(mark(QVariant, QVariant)), rocketGround, SIGNAL(mark(QVariant, QVariant)));
            connect(processor, SIGNAL(updateMap(double, double, bool)), rocketGround, SLOT(updateMap(double, double, bool)));
            connect(processor, SIGNAL(plot(QVector<QVector<double>>, QVector <double>, char)), rocketGround, SLOT(plot(QVector<QVector<double>>, QVector <double>, char)));
            connect(processor, SIGNAL(updateTerminal(QStringListModel*, QStringListModel*)), rocketGround, SLOT(updateTerminal(QStringListModel*, QStringListModel*)));
            connect(processor, SIGNAL(updateGauges(QVector<double>)), rocketGround, SLOT(updateGauges(QVector<double>)));
            connect(processor, SIGNAL(plot3D(double, double, double)), rocketGround, SLOT(plot3D(double, double, double)));

            ui->tabWidget->addTab(rocketGround, QIcon(QString("")), "Rocket");
            ui->tabWidget->addTab(powerPCB, QIcon(QString("")), "Power monitor");
        }

        // Switches and menus
        ui->rocketWidgets->menuAction()->setVisible(true);
        break;

    case AppModes::ignitorsMode:
        connect(ignitors, SIGNAL(dataToSend(char)), processor->mainSerial, SLOT(sendOrdersSerial(char)));
        connect(processor, SIGNAL(plot(QVector<QVector<double>>, QVector <double>, char)), ignitors, SLOT(plot(QVector<QVector<double>>, QVector <double>, char)));
        connect(processor, SIGNAL(updateGauges(QVector<double>)), ignitors, SLOT(updateGauges(QVector<double>)));

        ui->tabWidget->clear();
        ui->tabWidget->addTab(ignitors, QIcon(QString("")), "Ignitors");
        break;
    }
}

void MainWindow::addNewController(QString portName, uint32_t baudRate, uint8_t device)
{
    switch (device)
    {
        case 0:
            connect(this, SIGNAL(connectArduino(QString, uint32_t)), processor->mainSerial, SLOT(connectArduino(QString, uint32_t)));
            emit connectArduino(portName, baudRate);
            disconnect(this, SIGNAL(connectArduino(QString, uint32_t)), processor->mainSerial, SLOT(connectArduino(QString, uint32_t)));
            break;
    }
}

void MainWindow::on_tabWidget_currentChanged(int)
{

}

void MainWindow::testingMode(bool status)
{
    // Testing and performing
    testing = status;
    bench->testing = status;
    rocketGround->testing = status;
}

void MainWindow::connect_serial()
{    
    // Connect Serial Port
    qInfo() << "con";
    portDisplay->selectPort();
    connected = true;
}

void MainWindow::bench_triggered()
{
    // Select bench
    if (!connected) {connect_serial();}
    mainMode(AppModes::benchMode);
}

void MainWindow::launchPad_triggered()
{
    // Select LaunchPad
    if (!connected) {connect_serial();}
    mainMode(AppModes::rocketMode);
}

void MainWindow::ignitors_triggered()
{
   // Select Ignitors
   if (!connected) {connect_serial();}
   mainMode(AppModes::ignitorsMode);
}

void MainWindow::ignition_triggered()
{
    // Select ignition
    if (!connected) {connect_serial();}
    ignition->countDown();
}

void MainWindow::on_actionCompare_triggered()
{
    // Compares the theoretical data with the test one
    if (processor->phase != BenchSymb::Phases::finished)
    {
        QMessageBox::question(this, "Too fast my friend", "You need to perform a test first", QMessageBox::Ok);
        return;
    }

    bool odeke = (QMessageBox::Yes == QMessageBox::question(this, "Type of file", "Is if from Odeke?", QMessageBox::Yes | QMessageBox::No));
    processor->compareGraph(QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Data (*.txt *.csv *)")), odeke);
}

void MainWindow::on_actionSave_triggered()
{
    // Saves the metadata, graphs and data
    QString firstNameFile;
    do {firstNameFile = nameFile + QString::number(++nFile);} while (QDir(firstNameFile).exists());

    // Gets the name of the folder
    QString folderDir = QFileDialog::getSaveFileName(this, tr("Save File"), firstNameFile);

    // No name provided so it just closes
    if (folderDir == "")
    {
        QMessageBox::critical(this, tr("Error"), tr("Can't save"));
        return;
    }

    // We modified the string gotten to put it as a folder address
    QString nameFile = folderDir.section('/', -1);
    QDir dir(folderDir.left(folderDir.lastIndexOf("/") + 1));
    dir.mkdir(nameFile);

    nameFile.prepend(folderDir + "/");

    // Saves all
    metadata->orderToSave(nameFile);
    processor->orderToSave(nameFile);

    switch (appMode)
    {
        case AppModes::benchMode:
            bench->saveGraph(nameFile);
            spBench->saveGraph(nameFile);
            break;

        case AppModes::rocketMode:
            if (secondDisplayOn) {rocketGraphs->saveGraph(nameFile);}
            else {rocketGround->saveGraph(nameFile);}
            break;

        case AppModes::ignitorsMode:
            ignitors->saveGraph(nameFile);
            break;
    }
}

void MainWindow::fpsMenu_triggered()
{
    bool ok;
    QString FPSString = QInputDialog::getItem(this, tr("Change FPS"), tr("FPS:"), FPSList, 0, false, &ok);

    if (ok) {emit changeFPS(FPSString.toInt());}
}

void MainWindow::on_actionDecodifiy_triggered()
{
    // We search for the old file
    QString oldName = QFileDialog::getOpenFileName(this, tr("Open File"), "../" , tr("Data (*.txt *.csv *)"));
    QFile oldFile(oldName);
    if (!oldFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can't open the file"));
        return;
    }
    oldFile.close();

    // We open a new file
    QString newName = QFileDialog::getSaveFileName(this, tr("Save File"), "../", tr("*.txt"));
    QFile newFile(newName);
    if (!newFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can't save the file"));
        return;
    }
    newFile.close();

    processor->decodifyFile(oldName, newName);
}

void MainWindow::altitude3D_triggered(bool status)
{
    // Enables the 3D altitude graph
    rocketGraphs->showAltitude3D(status);
    rocketGround->showAltitude3D(status);
    altitude3D = status;

    legendTSwitch->setVisible(!status);
}

void MainWindow::float_triggered(bool checked)
{
    // Shows a floating terminal
    if (checked)
    {
        floatingTerminal->show();
        emit floatingTerminal_toggled(!checked);
        serialBSwitch->setChecked(false);
        serialTSwitch->setChecked(false);
    }
    if (!checked)
    {
        floatingTerminal->hide();
        emit floatingTerminal_toggled(!checked);
        floatTSwitch->setChecked(false);
        serialTSwitch->setChecked(true);
        serialBSwitch->setChecked(true);
    }
}

void MainWindow::temp_triggered(bool status)
{
    // Shows the bench's temperature legend
    if (status)
        emit showWidgets(3);
    else
        emit showWidgets(4);

    tempGraphStatus = status;
    secondDisplay->clearTab();
    if (secondDisplayOn && (tempGraphStatus || pressureGraphStatus))
        secondDisplay->loadTab(spBench, "Chamber pressure");
}

void MainWindow::pressure_triggered(bool status)
{
    // Shows the bench's pressure legend
    if (status)
        emit showWidgets(5);
    else
        emit showWidgets(6);

    pressureGraphStatus = status;
    secondDisplay->clearTab();
    if (secondDisplayOn && (tempGraphStatus || pressureGraphStatus))
        secondDisplay->loadTab(spBench, "Chamber pressure");
}

void MainWindow::dual_triggered(bool status)
{
    // Enables dual screen
    secondDisplayOn = status;
    mainMode(appMode);
    bench->dual_triggered(status);

    if (status)
    {
        secondDisplay->show();
        secondDisplay->windowHandle()->setScreen(qApp->screens()[0]);
        secondDisplay->setWindowState(Qt::WindowMaximized);
    }
    else
    {
        secondDisplay->close();
        dualSwitch->setChecked(false);
    }
}

void MainWindow::reset_test()
{
    // In case it was a missclicked
    if (QMessageBox::No == QMessageBox::question(this, "Resetting data", "Are you sure?", QMessageBox::Yes | QMessageBox::No)) {return;}

    // Resets the try
    metadata->cleanMetadata();
    processor->resetTest();

    switch (appMode)
    {
    case AppModes::benchMode :
        bench->resetTest();
        break;

    case AppModes::rocketMode :
        if (secondDisplayOn)
        {
            rocketControl->resetTest();
            rocketGraphs->resetTest();
        }
        else rocketGround->resetTest();
        break;

    case AppModes::ignitorsMode :
        ignitors->resetTest();
        break;
    }
}

void MainWindow::fileError(int codeError)
{
    switch (codeError)
    {
    case fileErrorCode::oldFile:     QMessageBox::question(this, "File error", "File to save data could not be opened.", QMessageBox::Ok);                      break;
    case fileErrorCode::oldFileOpen: QMessageBox::question(this, "File error", "File where the data was being stored could not be opened.", QMessageBox::Ok);   break;
    case fileErrorCode::newFileSave: QMessageBox::question(this, "File error", "New file could not be opened.", QMessageBox::Ok);                               break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Closes the app but has a save if we didn't save alert when not testing
    event->ignore(); // Wait for the closing

    if (!testing && (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Save Data?", QMessageBox::Yes | QMessageBox::No)))
        on_actionSave_triggered();

    serialThread.quit(); // We remove the thread
    event->accept(); // Finally it gets closed
}

void MainWindow::receivingDragged(QWidget *w, QString t, QPointF pos)
{
    draggedWidget = w;
    draggedText = t;
    draggedPos = pos;
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    draggedIndex = index;
    draggedWidget = ui->tabWidget->widget(index);
    draggedText = ui->tabWidget->tabText(index);
    draggedPos = QCursor::pos();
    emit aboutToBeDragged(draggedWidget, draggedText, draggedPos);

    ui->tabWidget->setTabVisible(index, false);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText(ui->tabWidget->tabText(index));
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->answerRect().intersects(ui->tabWidget->geometry()))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent*)
{
    if ((QCursor::pos() - draggedPos).manhattanLength() > QApplication::startDragDistance())
    {
        ui->tabWidget->addTab(draggedWidget, draggedText);

        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    }
    else
    {
        ui->tabWidget->setTabVisible(draggedIndex, true);
    }
}

void MainWindow::createMenuButtons()
{
    QFont menuFont("Consolas", 10);

    // Create Buttons
    reset->setFont(menuFont); // Set the fontBench
    ui->menubar->addAction(reset); // Adds the action to the menu
    connect(reset, SIGNAL(triggered()), this, SLOT(reset_test())); // Connects the signal with the slot

    connectBoard->setFont(menuFont);
    ui->menubar->addAction(connectBoard);
    connect(connectBoard, SIGNAL(triggered()), this, SLOT(connect_serial()));

    help->setFont(menuFont);
    ui->menubar->addAction(help);
    connect(help, SIGNAL(triggered()), helpWindow, SLOT(show_help()));

    actionBench->setFont(menuFont);
    ui->menuApp->addAction(actionBench);
    connect(actionBench, SIGNAL(triggered()), this, SLOT(bench_triggered()));

    actionLaunchPad->setFont(menuFont);
    ui->menuApp->addAction(actionLaunchPad);
    connect(actionLaunchPad, SIGNAL(triggered()), this, SLOT(launchPad_triggered()));

    actionIgnitors->setFont(menuFont);
    //ui->menuApp->addAction(actionIgnitors);
    connect(actionIgnitors, SIGNAL(triggered()), this, SLOT(ignitors_triggered()));

    actionIgnition->setFont(menuFont);
    ui->menuApp->addAction(actionIgnition);
    connect(actionIgnition, SIGNAL(triggered()), this, SLOT(ignition_triggered()));

    actionFPS->setFont(menuFont);
    ui->menuTab->addAction(actionFPS);
    connect(actionFPS, SIGNAL(triggered()), this, SLOT(fpsMenu_triggered()));
}

void MainWindow::createSwitches()
{
    // Testing mode switch
    createJustOneSwitch(testingSwitch, testingSwitchAction, ui->menuApp, true);
    connect(testingSwitch, SIGNAL(toggled(bool)), this, SLOT(testingMode(bool))); // Connects the signal of the switch triggering with the appropiate slot

    // Serial terminal bench switch
    createJustOneSwitch(serialBSwitch, serialBSwitchAction, ui->benchWidgets, true);
    connect(serialBSwitch, SIGNAL(toggled(bool)), bench, SLOT(showTerminal(bool)));

    // Serial terminal launch switch
    createJustOneSwitch(serialTSwitch, serialTSwitchAction, ui->rocketWidgets, true);
    connect(serialTSwitch, SIGNAL(toggled(bool)), rocketGraphs, SLOT(showTerminal(bool)));
    connect(serialTSwitch, SIGNAL(toggled(bool)), rocketGround, SLOT(showTerminal(bool)));

    // Floating terminal switch
    createJustOneSwitch(floatTSwitch, floatTSwitchAction, ui->menuTab, false);
    connect(floatTSwitch, SIGNAL(toggled(bool)), this, SLOT(float_triggered(bool)));
    connect(floatingTerminal, SIGNAL(closingTerminal(bool)), this, SLOT(float_triggered(bool)));

    // Map zoom launch switch
    createJustOneSwitch(zoomSwitch, zoomSwitchAction, ui->rocketWidgets, true);
    connect(zoomSwitch, SIGNAL(toggled(bool)), rocketGround, SLOT(changeZoom(bool)));
    connect(zoomSwitch, SIGNAL(toggled(bool)), rocketGraphs, SLOT(changeZoom(bool)));

    // 3D rocket graph launch switch
    createJustOneSwitch(altitude3DSwitch, altitude3DSwitchAction, ui->rocketWidgets, false);
    connect(altitude3DSwitch, SIGNAL(toggled(bool)), this, SLOT(altitude3D_triggered(bool)));

    // Temperature graph bench switch
    createJustOneSwitch(tempSwitch, tempSwitchAction, ui->benchWidgets, true);
    connect(tempSwitch, SIGNAL(toggled(bool)), this, SLOT(temp_triggered(bool)));

    // Pressure graph bench switch
    createJustOneSwitch(pressureSwitch, pressureSwitchAction, ui->benchWidgets, true);
    connect(pressureSwitch, SIGNAL(toggled(bool)), this, SLOT(pressure_triggered(bool)));

    // Dual switch
    createJustOneSwitch(dualSwitch, dualSwitchAction, ui->menuTab, false);
    connect(dualSwitch, SIGNAL(toggled(bool)), this, SLOT(dual_triggered(bool)));
    dualSwitch->setChecked(false);

    // Bench legend switch
    createJustOneSwitch(legendBSwitch, legendBSwitchAction, ui->benchWidgets, false);
    connect(legendBSwitch, SIGNAL(toggled(bool)), bench, SLOT(showLegend(bool)));
    connect(legendBSwitch, SIGNAL(toggled(bool)), spBench, SLOT(showLegend(bool)));

    // Launch legend switch
    createJustOneSwitch(legendTSwitch, legendTSwitchAction, ui->rocketWidgets, false);
    connect(legendTSwitch, SIGNAL(toggled(bool)), rocketGround, SLOT(showLegend(bool)));
    connect(legendTSwitch, SIGNAL(toggled(bool)), rocketGraphs, SLOT(showLegend(bool)));

    // Thrust in N switch
    createJustOneSwitch(thrustInNSwitch, thrustInNSwitchAction, ui->benchWidgets, false);
    connect(thrustInNSwitch, SIGNAL(toggled(bool)), bench, SLOT(changeThrustUnits(bool)));
    connect(thrustInNSwitch, SIGNAL(toggled(bool)), processor, SLOT(changeThrustUnits(bool)));
}

void MainWindow::createJustOneSwitch(Switch* swi, QWidgetAction* action, QMenu* menu, bool checked)
{
    action = new QWidgetAction(menu); // Creates an action
    swi->setLayoutDirection(Qt::RightToLeft); //Sets the layout direction as we wish
    action->setDefaultWidget(swi); // We put the action inside the switch
    menu->addAction(action); // We add the action (switch) to the menu
    swi->setChecked(checked); // Set the switch to checked or not
}

void MainWindow::changeStylesheet(float factor)
{
    // Changes the tab menu to the desired height
    tabStyleSheet.replace("tabWidth", QString::number(200*factor));
    tabStyleSheet.replace("tabHeight", QString::number(30*factor));
    ui->tabWidget->setStyleSheet(tabStyleSheet);

    // Changes the barMenu to the desired height
    menuStyleSheet.replace("barmenuHeight", QString::number(22*factor));
    menuStyleSheet.replace("menuHeight", QString::number(26*factor));
    ui->menubar->setStyleSheet(menuStyleSheet);

    // Changes all the switches to the desired height
    testingSwitch->changeSize(factor);
    zoomSwitch->changeSize(factor);
    altitude3DSwitch->changeSize(factor);
    serialBSwitch->changeSize(factor);
    serialTSwitch->changeSize(factor);
    tempSwitch->changeSize(factor);
    pressureSwitch->changeSize(factor);
    dualSwitch->changeSize(factor);
    legendBSwitch->changeSize(factor);
    legendTSwitch->changeSize(factor);
    floatTSwitch->changeSize(factor);
    thrustInNSwitch->changeSize(factor);
}

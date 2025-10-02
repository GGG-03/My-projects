#include "dataprocessor.h"
#include "myconfig.h"

#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <functional>
#include <cmath>

DataProcessor::DataProcessor(QObject *parent) : QObject{parent}
{
    dataTerminal.reserve(200);
    orderTerminal.reserve(200);
    dataReceived.reserve(100);

    // Launch
    maxAltitudeData.resize(nLaunchData);
    altitudeData.resize(nLaunchData);
    dataGauges.resize(9);
    powerGauges.resize(10);
    for (auto& i: altitudeData) {i.reserve(1000000);}

    // Bench (including thrust in N)
    maxBenchData.resize(nBenchData + 1);
    benchData.resize(nBenchData + maxTCouples + 1);
    for (auto& i: benchData) {i.reserve(1000000);}

    // Ignitors
    maxIgnitorsData.resize(nIgnitorsData);
    ignitorsData.resize(nIgnitorsData);
    LCDData.resize(3);
    for (auto& i: ignitorsData) {i.reserve(1000000);}

    // Comparing
    comparingData.resize(2);
    for (auto& i: comparingData) {i.reserve(1000000);}
}

void DataProcessor::startUp()
{
    mainSerial = new SerialDevice;
    connect(mainSerial, SIGNAL(dataReceived(QVector<double>&)), this, SLOT(dataProcessing(QVector<double>&)));
    connect(mainSerial, SIGNAL(orderReceived(uint8_t, uint8_t)), this, SLOT(orderProcessing(uint8_t, uint8_t)));

    // Creates the FPS control timer
    frameTimer = new QTimer(this);
    connect(frameTimer, SIGNAL(timeout()), this, SLOT(refresh()));
    changeFPS(50);
    frameTimer->start();
}

void DataProcessor::changeMode(char mod)
{
    dataTerminal.clear();
    orderTerminal.clear();

    switch (mod)
    {
    case AppModes::benchMode:
        dataTerminal.append(benchLabel);
        break;

    case AppModes::rocketMode:
        dataTerminal.append(launchLabel);
        break;
    }

    orderTerminal.append(orderLabel);

    appMode = mod;
}

void DataProcessor::orderProcessing(uint8_t mode, uint8_t order)
{
    QString text;

    qInfo() << order;

    // To show in the terminal the received orders
    switch (mode)
    {
    case AppModes::benchMode:
        switch (order % 100)
        {
        using namespace BenchSymb;
        case Orders::arduinoConnected:     text = "     Device connected";                 break;
        case Orders::enableTransducer:     text = "     Transducer enabled";               break;
        case Orders::disableTransducer:    text = "     Transducer disabled";              break;
        case Orders::enableHydroStatic:    text = "     Hydrostatic enabled";              break;
        case Orders::disableHydroStatic:   text = "     Hydrostatic disabled";             break;
        case Orders::tare:                 text = "     Tare completed";                   break;
        case Orders::startPerforming:      text = "     Starting to performance";          break;
        case Orders::stopPerforming:       text = "     Performance completed";            break;
        case Orders::startIgnition:        text = "     Ignition started";                 break;
        case Orders::stopIgnition:         text = "     Ignition stopped";                 break;
        case Orders::errorSD:              text = "     Error initializing SD";            break;
        case Orders::errorSDFile:          text = "     Error initializing SD file";       break;
        case Orders::errorFlash:           text = "     Error initializing flash";         break;
        case Orders::errorFlashFile:       text = "     Error initializing flash file";    break;
        }

        if (order > 100)
            text.append(" (BREDA)");
        else
            text.append(" (HERMES)");
        break;

    case AppModes::rocketMode:
        using namespace RocketSymb;
        switch (order % 50)
        {
            case device_connected:                  text = "     Device connected";                     break;
            case ping:                              text = "     Pinged";                               break;
            case start_to_debug:                    text = "     Phase changed to debug";               break;
            case debug_to_standby:                  text = "     Phase changed to standby";             break;
            case standby_to_armed:                  text = "     Phase changed to armed";               break;
            case armed_to_ready:                    text = "     Phase changed to ready";               break;
            case ready_to_boost:                    text = "     Phase changed to boost";               break;
            case boost_to_apogee:                   text = "     Phase changed to apogee";              break;
            case apogee_to_drogue_descent:          text = "     Phase changed to drogue descent";      break;
            case drogue_descent_to_main_deployed:   text = "     Phase changed to main parachute";      break;
            case main_deployed_to_main_descent:     text = "     Phase changed to main descent";        break;
            case main_descent_to_recovery:          text = "     Phase changed to landed";              break;
            case error_SD:                          text = "     SD error";                             break;
        }

        if (order > 50)
            text.append(" (POLARIS)");
        else
            text.append(" (PMU)");
        break;
    }

    orderTerminal.insert(1, text);
    if (orderTerminal.size() > 100) {orderTerminal.removeLast();}
    refresh();
}

void DataProcessor::dataProcessing(QVector<double>& dataIncoming)
{
    dataReceived = dataIncoming;

    // Time elapsed
    timeElapsed = dataReceived[0] - timeStarted;

    QString show = " " + QString("%1").arg(timeElapsed, 11, 'f', 3);

    // Thrust in case it is in N
    if (thrustInN) {show.append(QString("%1").arg(dataReceived[1] * gravity, 11, 'f', 3));}
    else {show.append(QString("%1").arg(dataReceived[1], 11, 'f', 3));}

    // Rest of variables
    const int size = dataReceived.size();
    for (int i = 2; i < size; ++i) {show.append(QString("%1").arg(dataReceived[i], 11, 'f', 3));}

    // Update terminal
    dataTerminal.insert(1, show);
    if (dataTerminal.size() > 100) {dataTerminal.removeLast();}

    if ((phase == BenchSymb::Phases::ready) || (phase == BenchSymb::Phases::ignited))
    {       
        // Saves raw data
        for (auto i: qAsConst(dataReceived)) {rawOut << QString("%1, ").arg(i, 0, 'f', 3);}
        rawOut << "\n";

        // Data Processing
        switch (appMode)
        {
        case AppModes::benchMode:
            benchProcessing();
            break;

        case AppModes::rocketMode:
            telemetriaProcessing();
            break;

        case AppModes::ignitorsMode:
            ignitorsProcessing();
            break;
        }
    }
}

void DataProcessor::benchProcessing()
{
    using namespace BenchSymb;
    // When it detects a thrust's increase
    switch(phase)
    {
    case Phases::ready:
        // Adding in the buffer
        for (int i = 0; i < nBenchData; ++i) {benchData[i].append(dataReceived[i]);} // General
        for (int i = 0; i < maxTCouples; ++i) // TP
        {
            if ((abs(dataReceived[i + 5])) < 1000)
                benchData[THTemperature + i].append(dataReceived[i + 5]);
            else
            {
                qInfo() << "a";
                benchData[THTemperature + i].append(benchData[THTemperature + i].back());  // TP
            }
        }

        // Buffer of 300 samples while not graphing
        if (benchData[timeInS].size() > 300) {for (int i = 0; i < nBenchData + maxTCouples; ++i) {benchData[i].remove(0);}}

        // To detect an increase in thrust, so the graphing starts
        if (dataReceived[1] > 1.0 && avoidSpikes > 2)
        {
            if (++avoidSpikes > 5)
            {
                phase = Phases::ignited;
                avoidSpikes = 0;

                // Time correction
                timeStarted = double(benchData[timeInS][0]);
                for (auto& i : benchData[timeInS]) {i -= timeStarted;}

                //  Maximum value
                for (int i = 0; i < nBenchData; ++i) {maxBenchData[i] = *std::max_element(benchData[i].begin(), benchData[i].end());}
                maxBenchData[THTemperature] = 0;
                for (int i = 0; i < maxTCouples; ++i) {maxBenchData[THTemperature] = std::max(maxBenchData[THTemperature], *std::max_element(benchData[THTemperature + i].begin(), benchData[THTemperature + i].end()));}

                // Integration purposes
                timeSA -= timeStarted;
                emit sendDecreasedThrust(timeSA, timeFinished);

                // Thrust
                for (auto& i : benchData[1]) {benchData[nBenchData + maxTCouples].append(i * gravity);}
            }
        }
        // To make the curve to integrate
        else if (dataReceived[1] > 0.4) {if (++avoidSpikes == 2) {timeSA = dataReceived[timeInS];}}
        break;

    case Phases::ignited:
        // Time collection and maximum value
        maxBenchData[timeInS] = timeElapsed;
        benchData[timeInS].append(timeElapsed);

        // Data collection
        for (int i = 1; i < nBenchData; ++i) {benchData[i].append(dataReceived[i]);} // General
        for (int i = 0; i < maxTCouples; ++i) // TP
        {
            if ((abs(dataReceived[i + 5])) < 1000)
                benchData[THTemperature + i].append(dataReceived[i + 5]);
            else
            {
                qInfo() << "a";
                benchData[THTemperature + i].append(benchData[THTemperature + i].back());  // TP
            }
        }

        //  Maximum value
        for (int i = 1; i < nBenchData; ++i) {maxBenchData[i] = std::max(maxBenchData[i], dataReceived[i]);}
        for (int i = 0; i < maxTCouples; ++i) {maxBenchData[THTemperature] = std::max(maxBenchData[THTemperature], dataReceived[THTemperature + i]);}

        // Thrust in N
        benchData[nBenchData + maxTCouples].append(dataReceived[thrustInKg] * gravity);

        // In order to integrate thrust to get impulse, this gives a fair aproximation to the end of thrust.
        if ((dataReceived[1] < 0.5) && (avoidSpikes < 6))
        {
            if (++avoidSpikes == 5)
            {
                timeFinished = dataReceived[timeInS] - timeStarted;
                timeSA -= timeStarted;
                emit sendDecreasedThrust(timeSA, timeFinished);
            }
        }
        break;
    }
}

void DataProcessor::telemetriaProcessing()
{
    using namespace RocketSymb;
    // Status of the Rocket
    phase = round(dataReceived[raw_phase]);
    double time_raw = dataReceived[raw_time];
    double altitude_raw = dataReceived[raw_altitude];

    // Show data in gauges and labels
    {
        dataGauges[Gauges::acceleration]  = sqrt(pow(dataReceived[raw_acc_x], 2) + pow(dataReceived[raw_acc_y], 2) + pow(dataReceived[raw_acc_z], 2));
        dataGauges[Gauges::speed]         = sqrt(pow(dataReceived[raw_speed_x], 2) + pow(dataReceived[raw_speed_y], 2) + pow(dataReceived[raw_speed_z], 2));
        dataGauges[Gauges::rotX]          = dataReceived[raw_yaw];
        dataGauges[Gauges::rotY]          = dataReceived[raw_pitch];
        dataGauges[Gauges::rotZ]          = dataReceived[raw_roll];
        dataGauges[Gauges::altitude]      = altitude_raw;
        dataGauges[Gauges::maxAltitude]   = maxAltitudeData[1];
        dataGauges[Gauges::time_seconds]  = time_raw;
        dataGauges[Gauges::status]        = phase;

        // Power Gauges
        using namespace PowerSymb;
        powerGauges[Bat_Volt] = dataReceived[raw_bat_volt];
        powerGauges[Bat_Amp] = dataReceived[raw_bat_amp];
        powerGauges[Pyro1_Amp] = dataReceived[raw_pyro1_amp];
        powerGauges[Pyro2_Amp] = dataReceived[raw_pyro2_amp];
    }


    // To mark where we are in the map and center
    if (firstLocation)
    {
        emit mark(dataReceived[raw_lat], dataReceived[raw_long]);
        firstLocation = false;
    }

    // Stauts when it launches
    if (phase > 3)
    {
        // Altitude graph
        altitudeData[0].append(time_raw);
        altitudeData[1].append(altitude_raw);

        maxAltitudeData[0] = time_raw;
        maxAltitudeData[1] = std::max(maxAltitudeData[1], altitude_raw);
    }

    // Parachute is deployed or deploying
    if (phase == 5) //!parachuteDeployed)
    {
        emit parachuteStatus(true);
        //parachuteDeployed = true;
    }

    // Rocket landed
    if (phase == 6)
    {
        if (++lastData == 200)
        {
            frameTimer->stop();
            rawData.close();
            //ensayoFinished = true;
        }
    }
}

void DataProcessor::ignitorsProcessing()
{

}

void DataProcessor::nTHChanged(int th)
{
    nTCouples = th;
}

void DataProcessor::changeFPS(int fps)
{
    frameTimer->setInterval(1000/fps);
}

void DataProcessor::refresh()
{
    // Terminal
    dataTerminalModel->setStringList(dataTerminal);
    orderTerminalModel->setStringList(orderTerminal);
    emit updateTerminal(dataTerminalModel, orderTerminalModel);

    // Graphs, gauges and maps
    switch (appMode)
    {
    case AppModes::benchMode:
        if (phase == BenchSymb::Phases::ignited)
            emit plot(benchData, maxBenchData, 0);
        break;

    case AppModes::rocketMode:
        emit updateGauges(dataGauges);
        emit updatePower(powerGauges);
        emit updateMap(dataReceived[18], dataReceived[19], zoom);
        emit plot(altitudeData, maxAltitudeData, 0);
        emit plot3D(dataReceived[18], dataReceived[2], dataReceived[19]);
        break;

    case AppModes::ignitorsMode:
        break;
    }
}

void DataProcessor::phaseChanged(int newPhase)
{
    phase = newPhase;
    switch (appMode)
    {
    case AppModes::benchMode:
        using namespace BenchSymb;
        switch(phase)
        {
        case Phases::ready:
            mainSerial->sendOrdersSerial(Orders::startPerforming, AppModes::benchMode);
            openFile();
            break;

        case Phases::finished:
            mainSerial->sendOrdersSerial(Orders::stopPerforming, AppModes::benchMode);
            rawData.close();
            break;
        }
        break;

    case AppModes::rocketMode:
        break;

    case AppModes::ignitorsMode:
        break;
    }
}

void DataProcessor::resetTest()
{
    // Reset variables and graphs
    phase = 0;

    switch (appMode)
    {
    case AppModes::benchMode:
        using namespace BenchSymb;
        // Cleans all variables
        for (int i = 0; i < nBenchData + 1; ++i)
        {
            benchData[i].clear();
            maxBenchData[i] = 0;
        }

        emit plot(benchData, maxBenchData, 0);
        emit plot(benchData, maxBenchData, 1);

        avoidSpikes = 0;

        mainSerial->sendOrdersSerial(Orders::stopPerforming, AppModes::benchMode);
        break;

    case AppModes::rocketMode:
        for (int i = 0; i < nLaunchData; ++i)
        {
            altitudeData[i].clear();
            maxAltitudeData[i] = 0;
        }

        emit plot(altitudeData, maxAltitudeData, 0);
        emit plot(altitudeData, maxAltitudeData, 1);

        firstLocation = true;
        break;

    case AppModes::ignitorsMode:
        for (int i = 0; i < nIgnitorsData; ++i)
        {
            ignitorsData[i].clear();
            maxIgnitorsData[i] = 0;
        }

        emit plot(ignitorsData, maxIgnitorsData, 0);
        emit plot(ignitorsData, maxIgnitorsData, 1);
        break;
    }

    // Closes file
    rawData.close();
}

void DataProcessor::decodifyFile(QString oldName, QString newName)
{
    // Buffer and streams
    QByteArray decBuffer;

    // Set file name, open files and set streams
    codFile.setFileName(oldName);
    decFile.setFileName(newName);

    codIn.setDevice(&codFile);
    decOut.setDevice(&decFile);

    codFile.open(QIODevice::ReadOnly);
    decFile.open(QIODevice::WriteOnly);

    // Converter
    while (!codIn.atEnd())
    {
        // Adds to the buffer little by little (4 packs more or less)
        decBuffer += codIn.readLine().toLatin1();

        // Decodifies
        //decodifier(decBuffer, false);
    }

    // Close both files
    codFile.close();
    decFile.close();
}

void DataProcessor::decodifyFileProcessing(QVector<double>& dataIncoming)
{
    // Writes the formatted data
    for (auto i = dataIncoming.begin(); i != dataIncoming.end() - 1; ++i) {decOut << QString("%1, ").arg(*i, 0, 'f', 3);}
    decOut << QString("%1\n").arg(dataIncoming.last(), 0, 'f', 3);
}

void DataProcessor::changeThrustUnits(bool)
{
    emit plot(benchData, maxBenchData, 0);
}

void DataProcessor::getImpulse(double timeS, double timeF)
{
    // Start and end positions
    int s = std::lower_bound(benchData[0].begin(), benchData[0].end(), timeS) - benchData[0].begin();
    int f = std::upper_bound(benchData[0].begin(), benchData[0].end(), timeF) - benchData[0].begin();

    // Calculate burn time
    double burnTime = benchData[0][f] - benchData[0][s];

    // Calculates trapeze integral (impulse)
    double impulse = 0.0;
    for (int i = s; i < f; ++i) {impulse += (benchData[0][i+1] - benchData[0][i]) * (benchData[1][i+1] + benchData[1][i]) / 2;}
    benchIntegralData[0] = impulse;

    // Calculates average thrust
    benchIntegralData[1] = impulse / burnTime;

    // Calculates max thrust position in the interval given
    benchIntegralData[2] = *std::max_element(benchData[1].begin() + s, benchData[1].begin() + f);

    QString stringData = "Burn time is "  + QString::number(burnTime) + " s.";

    for (int i = 0; i < 3; ++i)
    {
        stringData += benchIntegralHeader[i] + QString::number(benchIntegralData[i]) + benchIntegralUnits[i];
        stringData += " / " + QString::number(benchIntegralData[i] * 9.81) + benchIntegralUnits[i + 3];
    }

    // Sends data (impulse, avg, maxBenchData, burntime)
    emit showImpulse(stringData);
}

void DataProcessor::openFile()
{
    // File
    firstNameFile = QDir::currentPath() + "/data/" + QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss") + ".txt";
    rawData.setFileName(firstNameFile);
    rawOut.setDevice(&rawData);

    // Opens the file
    if (!rawData.open(QIODevice::WriteOnly)) {emit fileNotOpened(0);}

    // Writes header
    switch (appMode)
    {
    case AppModes::benchMode:       rawOut << benchFileHeader;      break;
    case AppModes::rocketMode:      rawOut << launchFileHeader;     break;
    case AppModes::ignitorsMode:    rawOut << ignitorsFileHeader;   break;
    }
}

void DataProcessor::orderToSave(QString nameFile)
{
    QString name = nameFile + "_data.txt";

    if (appMode == AppModes::benchMode)
    {
        // Opens files
        QFile raw(firstNameFile);
        QFile fresh(name);

        if (!raw.open(QIODevice::ReadOnly))
        {
            emit fileNotOpened(1);
            return;
        }

        if (!fresh.open(QIODevice::WriteOnly))
        {
            emit fileNotOpened(2);
            return;
        }

        QTextStream in(&raw);
        QTextStream out(&fresh);

        // Header line
        out << in.readLine() << "\n";

        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList splittedLine = line.split(", ");

            double dataDouble = (splittedLine[0].trimmed()).toDouble();

            // Writes just lines where the thrust was detected
            if (dataDouble >= timeStarted)
            {
                out << dataDouble - timeStarted;

                const int size = splittedLine.size();
                for (int i = 1; i < size; ++i) {out << ", " << splittedLine[i].trimmed().toDouble();}
                out << "\n";
            }
        }
        fresh.close();
    }
    else if ((appMode == AppModes::rocketMode) || (appMode == AppModes::ignitorsMode))
    {
        rawData.rename(name);
    }
}

void DataProcessor::compareGraph(QString nameFile, bool odeko)
{
    if (appMode == AppModes::benchMode)
    {
        QFile toCompare(nameFile);

        if (!toCompare.open(QIODevice::ReadOnly))
        {
            qCritical() << "Could not open file" << toCompare.errorString();
            return;
        }

        QTextStream in(&toCompare);

        if (odeko)
        {
            QStringList splittedTime;
            QStringList splittedThrust;

            for (int i = 0; i < 10; ++i) {in.readLine();}

            while (!in.atEnd())
            {
                QString line = in.readLine();

                if (line.contains("Time"))
                {
                    splittedTime = line.split(";");
                    splittedTime.removeAt(0);
                    splittedTime.removeAt(0);
                }

                if (line.contains("Thrust"))
                {
                    splittedThrust = line.split(";");
                    splittedThrust.removeAt(0);
                    splittedThrust.removeAt(0);
                    for (QString &i: splittedThrust) {comparingData[1].append(i.toDouble() / 9.81);}
                }
            }

            double timeMax = benchData[0][std::distance(benchData[1].begin(), std::max_element(benchData[1].begin(), benchData[1].end())) - 1];
            double comparingTimeMax = (splittedTime.at(std::distance(comparingData[1].begin(), std::max_element(comparingData[1].begin(), comparingData[1].end())) - 1)).toDouble();

            // We adjust the time axis so the thrust peak are in the same spot
            for (int i = 0; i < splittedThrust.size(); ++i) {comparingData[0].append((splittedTime.at(i)).toDouble() + timeMax - comparingTimeMax);}

            // Max thrust data
            maxBenchData[1] = std::max(maxBenchData[1], *std::max_element(comparingData[1].begin(), comparingData[1].end()));
        }
        else
        {
            in.readLine();

            while (!in.atEnd())
            {
                QString line = in.readLine();
                QStringList splittedData = line.split(", ");

                for (int i = 0; i < 2; ++i) {comparingData[i].append(splittedData.at(i).toDouble());}
            }

            maxBenchData[0] = std::max(maxBenchData[0], comparingData[0].last());
            maxBenchData[1] = std::max(maxBenchData[1], *std::max_element(comparingData[1].begin(), comparingData[1].end()));
            emit plot(comparingData, maxBenchData, 1);
        }

        toCompare.close();
    }
    else if (appMode == AppModes::rocketMode)
    {
        QFile raw(nameFile);

        QStringList splittedData;

        if (!raw.open(QIODevice::ReadOnly)) qCritical() << "Could not open file" << raw.errorString();
        {
            QTextStream in(&raw);

            while (!in.atEnd())
            {
                QString line = in.readLine();

                if (!line.contains("#"))
                {
                    splittedData = line.split(",");
                    for (int i = 0; i < 2; ++i) {comparingData[i].append((splittedData.at(i)).toDouble());}
                }
            }

            for (int i = 0; i < 2; ++i)
            {
                double max = *std::max_element(comparingData[i].begin(), comparingData[i].end());
                maxAltitudeData[i] = std::max(maxAltitudeData[i], max);
            }
            emit plot(comparingData, maxAltitudeData, 1);
        }
    }
}

void DataProcessor::accIntegrator(float accZ, float accT, float time)
{
    zN += speedZN*time + pow(time, 2)*accT/2;
    speedTN += accT*time;
    speedZN += accZ*time;
}

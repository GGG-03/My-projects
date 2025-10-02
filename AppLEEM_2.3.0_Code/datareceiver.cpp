#include "datareceiver.h"

dataReceiver::dataReceiver(QObject *parent) : QObject{parent}
{

}

void dataReceiver::readSerial()
{
    betProcess.start();

    serialData = arduino->readAll();
    serialBuffer += QString::fromStdString(serialData.toStdString());
    if (serialBuffer.contains("go") && serialBuffer.contains(",ya"))
    {
        int startPos = serialBuffer.indexOf("go") + 2;
        int lengthData = serialBuffer.indexOf(",ya", startPos) - startPos;
        QString prioData = serialBuffer.mid(startPos, lengthData);
        justData = prioData.split(",");
        serialBuffer.remove(0, startPos+lengthData + 3);
        //qDebug() << justData;
        emit dataReady(justData);

        if (true)
        {
            count++;
            totalTimeProc += betProcess.elapsed();
            qInfo() << count << "  Between process " << betProcess.elapsed() << " ms   Avg Proc " << totalTimeProc/count << " ms";
        }
    }
}

#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <QObject>

class dataReceiver : public QObject
{
    Q_OBJECT
    public:

    explicit dataReceiver(QObject *parent = nullptr);

    signals:

    public slots: readSerial()

};

#endif // DATARECEIVER_H

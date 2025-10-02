#ifndef ROUTEPROVIDER_H
#define ROUTEPROVIDER_H

#include <QObject>
#include <qqml.h>
#include <QMetaClassInfo>
#include <QGeoPath>

class RouteProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList path READ path NOTIFY pathUpdated)
    QML_ELEMENT

    public:
        explicit RouteProvider(QObject *parent = nullptr);
        QVariantList path();
        QVariantList m_path;

    public slots:
        void addCoordinates(double lat, double lng);

    private:


    signals:
        void pathUpdated();

};

#endif // ROUTEPROVIDER_H

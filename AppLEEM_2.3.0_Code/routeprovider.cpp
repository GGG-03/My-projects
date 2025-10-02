#include "routeprovider.h"

RouteProvider::RouteProvider(QObject *parent) : QObject(parent)
{
    m_path.reserve(200000);
}

QVariantList RouteProvider::path()
{
    return m_path;
}

void RouteProvider::addCoordinates(double lat, double lng)
{
    m_path.append(QVariant::fromValue(QGeoCoordinate(lat, lng)));
}

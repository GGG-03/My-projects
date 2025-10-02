import QtQuick 2.0
import QtLocation 5.6
import QtPositioning 5.6
import QtGraphicalEffects 1.12

Rectangle
{
    id: window
    
    anchors.centerIn: parent

    color: "#add9e6"
    radius: width/40

    property double oldLng: -3.436472
    property double oldLat: 39.610444
    property string myString
    property real lat
    property real lng

    property Component comMarker: mapMarker

    Plugin
    {
        id: mapPlugin
        name: 'osm';
        PluginParameter
        {
            name: 'osm.mapping.offline.directory'
            value: ':/tiles/'
        }
    }

    Map
    {
        id: map
        anchors.fill: parent
        activeMapType: map.supportedMapTypes[1]
        plugin: mapPlugin
        center: QtPositioning.coordinate(oldLat, oldLng);
        zoomLevel: 19
        maximumZoomLevel: 19
        //minimumZoomLevel: 12

        layer.enabled: true
        layer.effect: OpacityMask
        {
            maskSource: Rectangle
            {
                width: window.width
                height: window.height
                radius: window.radius
            }
        }

        MapPolyline
        {
            id: line
            line.width: 5
            line.color: "#286e84"
            smooth: true
            opacity: 1
        }
    }

    Component
    {
        id: mapMarker
        MapQuickItem
        {
            id: markerImg
            anchorPoint.x: image.width/4
            anchorPoint.y: image.height
            coordinate: position
            sourceItem: Image
            {
                id: image
                source: 'qrc:/models/marker.png'
            }
        }
    }

    function addMarker(lat, lng)
    {
        var item = comMarker.createObject(window, {coordinate: QtPositioning.coordinate(lat, lng)})
        map.addMapItem(item)
    }

    function addPathCoordinates(lat, lng, recenter)
    {
        line.addCoordinate(QtPositioning.coordinate(lat, lng))
        //console.log(map.visibleArea)
        //console.log(map.visibleRegion)
        if (recenter) map.center = QtPositioning.coordinate(lat, lng)
    }

    function clearPath()
    {
        var max = line.pathLength()
        for (var i = 0; i < max; i++)
        {
            line.removeCoordinate(0)
        }
    }
}

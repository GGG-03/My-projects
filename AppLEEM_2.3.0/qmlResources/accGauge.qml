import QtQuick 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

Rectangle
{
    color: "#add9e6"
    anchors.fill: parent
    anchors.centerIn: parent

    CircularGauge
    {
        id: accgauge
        maximumValue: 20

        style: CircularGaugeStyle
        {
            tickmarkStepSize: 2
            labelStepSize: 2

            function degreesToRadians(degrees)
            {
                return degrees * (Math.PI / 180);
            }

            background: Canvas
            {
                onPaint:
                {
                    var ctx = getContext("2d");
                    ctx.reset();

                    ctx.beginPath();
                    ctx.strokeStyle = "#e34c22";
                    ctx.lineWidth = outerRadius * 0.02;

                    ctx.arc(outerRadius, outerRadius, outerRadius - ctx.lineWidth / 2,
                        degreesToRadians(valueToAngle(16) - 90), degreesToRadians(valueToAngle(20) - 90));
                    ctx.stroke();
                }
            }

            tickmark: Rectangle
            {
                visible: styleData.value < 16 || styleData.value % 2 == 0
                implicitWidth: outerRadius * 0.02
                antialiasing: true
                implicitHeight: outerRadius * 0.06
                color: styleData.value >= 16 ? "#e34c22" : "black"
            }

            minorTickmark: Rectangle
            {
                visible: styleData.value < 16
                implicitWidth: outerRadius * 0.01
                antialiasing: true
                implicitHeight: outerRadius * 0.03
                color: "black"
            }

            tickmarkLabel:  Text
            {
                font.pixelSize: Math.max(6, outerRadius * 0.1)
                text: styleData.value
                color: styleData.value >= 16 ? "#e34c22" : "black"
                antialiasing: true
            }

            foreground: Item
            {
                Rectangle
                {
                    width: outerRadius * 0.2
                    height: width
                    radius: width / 2
                    color: "black"
                    anchors.centerIn: parent
                }

                Text
                {
                    text: accgauge.value.toFixed(0)
                    font.pixelSize: outerRadius * 0.28
                    color: "black"
                    antialiasing: true
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter

                }
            }
        }

        property bool accelerating: false

        value: accelerating ? maximumValue : 0
        anchors.centerIn: parent
        anchors.fill: parent

        Keys.onSpacePressed: accelerating = true
        Keys.onReleased:
        {
            if (event.key === Qt.Key_Space) {
                accelerating = false;
                event.accepted = true;
            }
        }

        Component.onCompleted: forceActiveFocus()

        Behavior on value
        {
            NumberAnimation
            {
                duration: 500
            }
        }
    }

    function changeGaugeValue(valor)
    {
        accgauge.value = valor
    }
}

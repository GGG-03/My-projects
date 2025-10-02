import QtQuick.Controls 2.2
import QtQuick 2.15
import QtQuick.Scene3D 2.15
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Rectangle
{
    id: rect
    color: "#add9e6"
    anchors.fill: parent
    anchors.centerIn: parent

    property real userAngleX: 0.0
    property real userAngleY: 0.0
    property real userAngleZ: 0.0
    property real zoomCamera: 30.0

    Scene3D
    {
        id: window
        anchors.fill: parent
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        property int valor


        Entity
        {
            id: sceneRoot

            Camera
            {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 30
                aspectRatio: 16/9
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( rect.zoomCamera, 0.0, 0.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            OrbitCameraController
            {
                camera: camera
            }

            PhongMaterial
            {
                id: rocketMat
                ambient: "white"
            }

            PhongMaterial
            {
                id: xAxisMat
                ambient: "red"
            }

            PhongMaterial
            {
                id: yAxisMat
                ambient: "green"
            }

            Mesh
            {
                id: rocketMesh
                source: "qrc:/models/LEEMUR.obj"
            }

            Mesh
            {
                id: xAxisMesh
                source: "qrc:/models/Xaxis.obj"
            }

            Mesh
            {
                id: yAxisMesh
                source: "qrc:/models/Yaxis.obj"
            }

            Transform
            {
                id: rocketTransform
                rotationX: rect.userAngleX
                rotationY: rect.userAngleY
                rotationZ: rect.userAngleZ
            }


            components:
                [
                RenderSettings
                {
                    activeFrameGraph: ForwardRenderer
                    {
                        clearColor: "transparent"
                        camera: camera
                    }
                },

                InputSettings { }
            ]

            Entity
            {
                id: rocket
                components: [rocketMesh, rocketMat, rocketTransform]
            }

            Entity
            {
                id: xAxis
                components: [xAxisMesh, xAxisMat, rocketTransform]
            }

            Entity
            {
                id: yAxis
                components: [yAxisMesh, yAxisMat, rocketTransform]
            }

            Axis
            {
                id: axis
            }

        }
    }

    function changeRotationX(value)
    {
        rect.userAngleX = value*1.0;
    }
    function changeRotationY(value)
    {
        rect.userAngleY = value*1.0;
    }
    function changeRotationZ(value)
    {
        rect.userAngleZ = value*1.0;
    }
    function setZoomRocket(value)
    {
        rect.zoomCamera = value*1.0;
    }
}





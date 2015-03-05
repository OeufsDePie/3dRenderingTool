import QtQuick 2.0
import OpenGLUnderQML 1.0

Item {

    width: 320
    height: 480

    PointCloud {
        id: pc
        pathPly: "./ply/castle.ply"
    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }
}

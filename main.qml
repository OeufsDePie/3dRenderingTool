import QtQuick 2.0
import OpenGLUnderQML 1.0

Item {

    width: 320
    height: 480

    PointCloud {
        id: pc
//        pathPly: "./ply/castle.ply"
    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "This is a point cloud renderer using openGL under QML, which is nice"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }
}

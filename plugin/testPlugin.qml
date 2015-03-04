import QtQuick 2.0
import PointCloudPlugin 1.0

PointCloudWrapper{
    focus: true
    PointCloud{
        id: pointcloud
       pathPly: "ply/castle.ply"
    }
    Keys.onPressed: {
         if (event.key == Qt.Key_0) {
             console.log("Key 0");
             event.accepted = true;
         }
    }
}

import QtQuick 2.0
import PointCloudPlugin 1.0

PointCloudWrapper{
    PointCloud{
        focus: true
        id: pointcloud
        pathPly: "ply/villa.ply"
        Keys.onPressed: {
            if (event.key == Qt.Key_0) {
                event.accepted = true;
                nextCam();
             }
        }
    }
}

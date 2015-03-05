QT += qml quick

HEADERS += ../src/PointCloud.h ../src/PointCloudRenderer.h ../src/Ply.h
SOURCES += ../src/PointCloud.cpp ../src/PointCloudRenderer.cpp ../src/Ply.cpp main.cpp
RESOURCES += pcgl.qrc

target.path = pcgl 
INSTALLS += target

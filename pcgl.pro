CONFIG += static
QT += qml quick

HEADERS += PointCloud.h PointCloudRenderer.h
SOURCES += PointCloud.cpp PointCloudRenderer.cpp Ply.cpp main.cpp
RESOURCES += pcgl.qrc

target.path = /home/loux/Bureau/TestQML/pcgl 
INSTALLS += target

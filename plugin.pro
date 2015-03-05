TEMPLATE = lib
CONFIG += plugin
QT += quick qml

DESTDIR = PointCloud/
TARGET = pcplugin
SOURCES += src/PointCloud.cpp src/PointCloudRenderer.cpp src/Ply.cpp
HEADERS += src/PointCloud.h src/PointCloudRenderer.h src/Ply.h

TEMPLATE = lib
CONFIG += plugin
QT += qml

DESTDIR = imports/PointCloudPlugin/
TARGET = pcplugin
SOURCES += PointCloud.cpp PointCloudRenderer.cpp Ply.cpp

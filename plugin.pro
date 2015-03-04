TEMPLATE = lib
CONFIG += plugin
QT += quick qml

DESTDIR = plugin/PointCloudPlugin/
TARGET = pcplugin
SOURCES += PointCloud.cpp PointCloudRenderer.cpp Ply.cpp
HEADERS += PointCloud.h PointCloudRenderer.h Ply.h

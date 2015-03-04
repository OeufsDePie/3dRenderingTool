#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include "../PointCloud.h"
#include <iostream>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<PointCloud>("OpenGLUnderQML", 1, 0, "PointCloud");
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    return app.exec();
}

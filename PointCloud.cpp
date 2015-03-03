#include "PointCloud.h"
#include <QtQuick/qquickwindow.h>

/////////////////////////// PointCloud implementation ///////////////////////////
PointCloud::PointCloud() : renderer(0),pathPly(QString())
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

void PointCloud::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
        connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}
void PointCloud::setpathPly(QString path) {
    if (path == pathPly) {
        return;
    }
    pathPly = path;
    emit pathChanged();
    if (window())
        window()->update();
}

void PointCloud::cleanup()
{
    if (renderer) {
        delete renderer;
        renderer = 0;
    }
}

void PointCloud::sync()
{
    if (!renderer) {
        renderer = new PointCloudRenderer();
        connect(window(),SIGNAL(beforeRendering()),renderer,SLOT(paint()),Qt::DirectConnection);
    }
    renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    renderer->setpathPly(pathPly);
}


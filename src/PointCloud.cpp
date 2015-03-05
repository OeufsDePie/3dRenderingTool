#include <QQuickWindow>
#include <QQmlExtensionPlugin>
#include "PointCloud.h"

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

void PointCloud::nextCam() {
    if (renderer)
         renderer->nextCam();
    if (window())
        window()->update();
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

/////////////////////////// PointCloud Plugin ///////////////////////////
class PointCloudPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("PointCloud"));
        qmlRegisterType<PointCloud>(uri, 1, 0, "PointCloud");
    }
};


//No idea what this line does, but without it, nothing works.
#include "PointCloud.moc"


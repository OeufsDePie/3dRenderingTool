#include <QQuickItem>
#include "PointCloudRenderer.h"

/* The PointCloud object must extend QQuickItem so that we can easily access the QQuickItem.window() function.
*/
class PointCloud : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString pathPly READ ply WRITE setpathPly NOTIFY pathChanged)
public:
    PointCloud();
public slots:
    void sync();
    void cleanup();
    QString ply() { return pathPly; }
    void setpathPly(QString path);
    Q_INVOKABLE void nextCam();
signals:
    void pathChanged();
private slots:
    void handleWindowChanged(QQuickWindow *win);
private:
    PointCloudRenderer *renderer;
    QString pathPly;
};




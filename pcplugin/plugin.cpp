#include "../PointCloud.h"
#include <QQmlExtensionPlugin>

class QExampleQmlPlugin : public QQmlExtensionPlugin
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

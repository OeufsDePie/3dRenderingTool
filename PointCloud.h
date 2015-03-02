#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* PointCloudRenderer must extend QObject since we are using it with signals and slot. It must also extend QOpenGLFunctions to use context and some OpenGL functions. 
The initialization of everything related with openGL and everything related with the .ply is made in the constructor. When this is done, we can use the pain() function 
to print the pointcloud. 
The renderer is separated from the PointCloud object since the renderer will live in the rendering thread while the PointCloud object will live in the GUI thread. When the GUI
sends the beforeRendering signal, the GUI will be blocked and the renderer will start rendering then sends back what has to be rendered to the GUI.  
*/
class PointCloudRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    PointCloudRenderer();
    ~PointCloudRenderer();
    void setViewportSize(const QSize &size) { viewportSize = size; }
    void setpathPly(QString path);
public slots:
    void paint();
private:
    bool initBuffers();
    bool readPLY(const std::string& filePath, GLfloat*& buffer, GLfloat*& colorbuffer);
private:
    bool isinitBuffers;
    QString pathPly;
    QSize viewportSize;
    GLfloat *vertex,*colors;
    QOpenGLVertexArrayObject *vao;
    QOpenGLBuffer *positionBuffer;
    QOpenGLBuffer *colorBuffer;
    QOpenGLShaderProgram *shaderProgram;
    size_t _dataSize;
};

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
signals:
    void pathChanged();
private slots:
    void handleWindowChanged(QQuickWindow *win);
private:
    PointCloudRenderer *renderer;
    QString pathPly;
};





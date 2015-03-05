#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* PointCloudRenderer must extend QObject since we are using it with signals and slot. It must also extend QOpenGLFu$
The initialization of everything related with openGL and everything related with the .ply is made in the constructor$
to print the pointcloud. 
The renderer is separated from the PointCloud object since the renderer will live in the rendering thread while the $
sends the beforeRendering signal, the GUI will be blocked and the renderer will start rendering then sends back what$
*/
class PointCloudRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    PointCloudRenderer();
    ~PointCloudRenderer();
    void setViewportSize(const QSize &size) { viewportSize = size; }
    void setpathPly(QString path);
    void nextCam();
public slots:
    void paint();
private:
    bool initBuffers();
    bool readPLY(const std::string& filePath, GLfloat*& buffer, GLfloat*& colorbuffer);
    void readCoordinates();
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
    glm::vec3* cameraCoordinates;
    int currentCam;
    int numberCameras;
    glm::vec3 center;
};

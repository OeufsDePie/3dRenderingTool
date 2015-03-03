#include "Ply.h"
#include "PointCloudRenderer.h"
#include <QtGui/QOpenGLContext>

/////////////////////////// PointCloudRenderer implementation ///////////////////////////
const char* vertex_source = ""\
    "#version 120\n"\
    "attribute vec3 in_Position;\n"\
    "attribute vec3 in_Color;\n"\
    "varying vec3 ex_Color;\n"\
    "uniform mat4 model;\n"\
    "uniform mat4 proj;\n"\
    "uniform mat4 view;\n"\
    "void main(void) {\n"\
    "    ex_Color = in_Color;\n"\
    "    gl_Position = proj*view*model*vec4(in_Position.xyz, 1.0);\n"\
    "}\n";

const char* fragment_source = ""\
    "#version 120\n"\
    "varying vec3 ex_Color;\n"\
    "void main(void) {\n"\
    " gl_FragColor = vec4(ex_Color,1.0);\n"\
    "}\n";

PointCloudRenderer::PointCloudRenderer() : isinitBuffers(false) {
    initializeOpenGLFunctions();
    shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vertex_source);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fragment_source);
    shaderProgram->link();
    shaderProgram->bind();
    // Depth test
    glEnable(GL_DEPTH_TEST);
}

void PointCloudRenderer::setpathPly(QString path) {
//pathPly must contain a valid ply file path
    if (path==pathPly) {
        return;
    }
    if (path == QString()) {
       return;
    }
    pathPly = path;
    std::string plyfile = pathPly.toStdString();
    readPLY(plyfile,vertex,colors);
    if (initBuffers()){
        isinitBuffers = true;
    }
}
bool PointCloudRenderer::initBuffers() {
    // Create VAO
    vao = new QOpenGLVertexArrayObject();
    vao->create();
    vao->bind();
    // Create VBO for position
    positionBuffer = new QOpenGLBuffer();
    positionBuffer->create();
    positionBuffer->setUsagePattern( QOpenGLBuffer::StreamDraw );
    positionBuffer->bind();
    positionBuffer->allocate(vertex, sizeof(float) * _dataSize );
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3 );
    shaderProgram->enableAttributeArray(0);
    // Create VBO for color
    colorBuffer = new QOpenGLBuffer();
    colorBuffer->create();
    colorBuffer->setUsagePattern( QOpenGLBuffer::StreamDraw );
    colorBuffer->bind();
    colorBuffer->allocate(colors, sizeof(float) * _dataSize );
    shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3 );
    shaderProgram->enableAttributeArray(1);

    // unbind buffers
    vao->release();
    positionBuffer->release();
    colorBuffer->release();
    shaderProgram->release();
    return true;
}

PointCloudRenderer::~PointCloudRenderer()
{
    colorBuffer->release();
    positionBuffer->release();
    shaderProgram->release();
    delete shaderProgram;
    /* Cleanup all the things we bound and allocated */
}

void PointCloudRenderer::paint()
{
    // Change the window size
    glViewport(0, 0, viewportSize.width(), viewportSize.height());
    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // If buffers are ready, we can draw the point cloud
    if (isinitBuffers) {
        shaderProgram->bind();
        vao->bind();
        // Initialiaze some usefull matrix
        GLint shaderId = shaderProgram->programId();
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 model      = glm::mat4(1.0f);
        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 proj = glm::perspective(10.0f, 1400.0f / 900.0f, 0.1f, 100.0f);
        // Camera matrix
        glm::vec3 pointcam = glm::vec3(0,0,0);
        glm::mat4 view       = glm::lookAt(
            pointcam, // Camera is at (4,3,3), in World Space
            glm::vec3(7,0,30), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        // Set attributes
        GLint uniModel = glGetUniformLocation(shaderId, "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        GLint uniProj = glGetUniformLocation(shaderId, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        GLint uniView = glGetUniformLocation(shaderId, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        // Draw
        glEnable(GL_POINT_SMOOTH);
        glPointSize(2);
        glDrawArrays(GL_POINTS, 0, _dataSize/3);
        // Unbind
        vao->release();
        shaderProgram->release();
    }
}


bool PointCloudRenderer::readPLY(const std::string& filePath, GLfloat*& buffer, GLfloat*& colorbuffer)
{
    Ply ply;
    if(!ply.open(filePath))
    {
        ply.close();
        return false;
    }

    for(Ply::ElementsIterator it = ply.elements_begin(); it != ply.elements_end(); ++it)
    {
        const Ply::Element& element = *it;
        if(element.name() != "vertex")
        {
            if(!ply.skip(element))
            {
                ply.close();
                return false;
            }
            continue;
        }

        std::vector<GLfloat> vertices;
        std::vector<GLfloat> colors;
        size_t verticeCount = 0;
        float tmp = 0.f;
        for(size_t i = 0; i != element.count(); ++i)
        {
            ply.read_begin(element);
            for(Ply::PropertiesIterator it2 = element.properties_begin();
                it2 != element.properties_end(); ++it2)
            {
                const Ply::Property& property = *it2;
                if(property.name() == "x")
                {
                    ply.read(property, tmp);
                    vertices.push_back(tmp);
                    verticeCount++;
                }
                else if(property.name() == "y")
                {
                    ply.read(property, tmp);
                    vertices.push_back(tmp);
                }
                else if(property.name() == "z")
                {
                    ply.read(property, tmp);
                    vertices.push_back(tmp);
                }
                else if(property.name() == "red" || property.name() == "diffuse_red")
                {
                    ply.read(property, tmp);
                    colors.push_back(tmp/255.);
                }
                else if(property.name() == "green" || property.name() == "diffuse_green")
                {
                    ply.read(property, tmp);
                    colors.push_back(tmp/255.);
                }
                else if(property.name() == "blue" || property.name() == "diffuse_blue")
                {
                    ply.read(property, tmp);
                    colors.push_back(tmp/255.);
                }
                else if(!ply.skip(property))
                {
                    ply.close();
                    return false;
                }
            }
            ply.read_end();
        }

        //!! hardcore copy
        _dataSize = verticeCount * 3;
        buffer = new GLfloat[_dataSize];
        colorbuffer = new GLfloat[_dataSize];
        std::copy(vertices.begin(), vertices.end(), buffer);
        std::copy(colors.begin(), colors.end(), colorbuffer);
        break; //!! break after the first 'vertex' element
    }

    ply.close();
    return true;
}

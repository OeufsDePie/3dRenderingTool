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

PointCloudRenderer::PointCloudRenderer() : isinitBuffers(false),currentCam(-1) {
    initializeOpenGLFunctions();
    shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vertex_source);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fragment_source);
    shaderProgram->link();
    shaderProgram->bind();
    // Depth test
    glEnable(GL_DEPTH_TEST);
}

void PointCloudRenderer::nextCam(){
    if (currentCam!=-1) //currentCam has be initialized
        currentCam = (currentCam+1)%numberCameras;
}
void PointCloudRenderer::setpathPly(QString path) {
//pathPly must contain a valid ply file path
//setpathPly is called at each synchronization, so we have to check if pathPly actually changed
// path could still be empty (QString()), in that case, we do nothing
    if (path==pathPly) {
        return;
    }
    if (path == QString()) {
       return;
    }
    pathPly = path;
    std::string plyfile = pathPly.toStdString();
    if (!readPLY(plyfile,vertex,colors))
        return;
    readCoordinates();
    // Set the current camera to the first camera 0
    currentCam=0;
    if (initBuffers()){
        // If GL is ready, then we can use the paint() function to draw the point cloud
        isinitBuffers = true;
    }
}

// Extract coordinates of cameras and calculate pointcloud center
void PointCloudRenderer::readCoordinates() {
    // Read the camera from the vertex buffer. vertex buffer must be initialized ...
    // We calculate the number of cameras
    int i=_dataSize-1;
    numberCameras=0;
    while (colors[i]==0 && colors[i-1]==1 && colors[i-2]==0){
        i=i-3;
        numberCameras++;
    }
    cameraCoordinates = new glm::vec3 [numberCameras];
    int j=0;
    for (int i=0; i<numberCameras; i++){
        cameraCoordinates[i] = glm::vec3((float)vertex[_dataSize-j-2],(float)vertex[_dataSize-j-1],(float)vertex[_dataSize-j]);
        j=j+3;
    }
    // Calculate the centre of the point cloud without cameras. We will be looking at this center
    // this might not be a good idea if the point cloud is not distributed homogeneously, but sometimes you
    // gotta roll the hard six ...
    float centerX=0;
    float centerY=0;
    float centerZ=0;
    for (int i=0;i<((int)_dataSize)-numberCameras-1;i=i+3){
        centerX = centerX + vertex[i];
        centerY = centerY + vertex[i+1];
        centerZ = centerZ + vertex[i+2];
    }
    centerX = centerX/(_dataSize/3);
    centerY = centerY/(_dataSize/3);
    centerZ = centerZ/(_dataSize/3);
    center = glm::vec3(centerX,centerY,centerZ);
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
        glm::mat4 proj = glm::perspective(30.0f, 4.0f/3.0f, 0.1f, 100.0f);
        // Camera matrix
        glm::mat4 view       = glm::lookAt(
            cameraCoordinates[currentCam], // Camera in World Space
            center, // is looking at ...
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
        glPointSize(3);
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

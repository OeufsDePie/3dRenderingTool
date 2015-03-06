# 3dRenderingTool
Standalone tool for 3d point-cloud-rendering and model visualization

#GLM Dependency
Before launching qmake, take a look at this.
sudo apt-get install libglm-dev

# QML plugin 
You can now import the C++ code inside any QML code as a plugin and create PointCloud object to print a point cloud (you have to specify the .ply path in the QML code)

To build the plugin, simply do (don't forget to remove the PointCloudPlugin/libpcplugin.so file before, otherwise it won't be rewritten):

```
qmake plugin.pro && make

```

The plugin is in the PointCloud directory (libpcplugin.so). To use it, simply copy the PointCloud folder wherever you want, and add the path directory to your QML.
Before using the PointCloud module, you have to know this :
* Module name is PointCloud (for the QML import), version is now 1.0
* To create a PointCloud object, use PointCloud { ... }
* You have to specify a ply path in order to print the pointcloud. To do so, simply add a *pathPly* entry in the PointCloud{} object
* You can look at the point cloud with different camera simply by calling *nextCam()*

# C++ renderer with QML
You can use the C++ renderer without using the QML module

To build the renderer :
```
cd testOpenGLUnderQML/
qmake pcgl.pro && make
```

To launch it ;
```
./pcgl
```

Again, you have to specify a ply path in main.qml to not have a black screen. 

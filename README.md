# 3dRenderingTool
Standalone tool for 3d point-cloud-rendering and model visualization

# QML plugin 
You can now the C++ code inside any QML code and create PointCloud object to print a point cloud (you have to specify the .ply path in the QML code)

To build the plugin, simply do :
```
qmake plugin.pro && make

```

The plugin is in the imports directory. To use it, you have to know this :
* Plugin name is PointCloudPlugin, version is now 1.0
* To create a PointCloud object, use PointCloud { ... }
* You have to specify a ply path in order to print the pointcloud. To do so, simply add a *pathPly* entry in the PointCloud{} object
* The PointCloud object can be inside PointCloudWrapper{}, but we still have to work on PointCloudWrapper. 

# C++ renderer with QML

To build the renderer :
```
qmake pcgl.pro && make
```

To launch it ;
```
./pcgl
```

Again, you have to specify a ply path in main.qml to not have a black screen. 

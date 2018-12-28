# RobotStudio
2018-12-28
添加了单元测试界面的`部分`实现。

---

## Building Steps

1. 首先从[https://www.qt.io/download](https://www.qt.io/download)下载Qt，按步骤安装Qt Creator，选择安装目录（本文档Qt安装目录为E:\）,  并选择Qt源码版本为Qt5.9.3，并选择MinGw5.3.0 32bit编译器。然后修改环境变量PATH添加E:\Qt\5.9.2\mingw53_32\bin和E:\Qt\Tools\mingw530_32\bin两个路径

2.  拷贝软件程序\系统配置与参数管理软件目录下的Python27文件夹到自定义目录（本文档将Python27拷贝到E:\），修改用户环境变量PATH，添加E:\Python27;E:\Python27\Scripts两个路径。注：Python为32位版本。

3. 拷贝软件程序\系统配置与参数管理软件目录下的PLCPython文件夹到自定义目录（本文档拷贝至E:\）,新建环境变量PYTHONPATH，并按顺序添加路径E:\Qt\PLCPython;E:\Qt\PLCPython\plcopen;E:\Qt\PLCPython\util;E:\Qt\PLCPython\xmlclass这四个路径

4. 添加PYTHONHOME环境变量，并添加路径E:\Python27

5. 可能会缺少python27.dll动态链接库文件，添加到C:\Windows目录下即可，修改完所有环境变量后，可能需要重启。

6. 如果需要编译PLC程序并生成字节码文件，需要将RobotStudio.exe可执行文件与iec_compiler.exe编译器和translator.exe字节码器存放在同一路径。

7. 使用QT打开后，修改RobotStudio.pro文件。

   ```makefile
   INCLUDEPATH += -I E:\Python27\include
   LIBS += -L E:\Python27\libs -lpython27
   ```

8. 下载lxml包，打开cmd输入

   ```
   pip install lxml
   ```

9. 编译运行打开Estun成功

---


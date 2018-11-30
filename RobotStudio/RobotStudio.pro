#-------------------------------------------------
#
# Project created by QtCreator 2017-12-25T15:04:56
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RobotStudio
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    solutiontree.cpp \
    robotconfig.cpp \
    xmluntity.cpp \
    addiodlg.cpp \
    plcresourceconf.cpp \
    plcpouconf.cpp \
    plcprojectctrl.cpp \
    combodelegate.cpp \
    taskintervaldelegate.cpp \
    ftptransfer.cpp \
    rsiconfig.cpp \
    plclibrarytreewidget.cpp

HEADERS += \
        mainwindow.h \
    solutiontree.h \
    robotconfig.h \
    xmluntity.h \
    addiodlg.h \
    plcresourceconf.h \
    plcpouconf.h \
    plcprojectctrl.h \
    combodelegate.h \
    taskintervaldelegate.h \
    ftptransfer.h \
    rsiconfig.h \
    plclibrarytreewidget.h

RESOURCES += \
    icon.qrc \
    graph.qrc

INCLUDEPATH += -I C:\Python27\include
LIBS += -L C:\Python27\libs -lpython27

DISTFILES += \
    PLCPython/plcopen/TC6_XML_V101.pdf \
    PLCPython/plcopen/__init__.pyc \
    PLCPython/plcopen/definitions.pyc \
    PLCPython/plcopen/plcopen.pyc \
    PLCPython/plcopen/structures.pyc \
    PLCPython/util/__init__.pyc \
    PLCPython/util/BitmapLibrary.pyc \
    PLCPython/util/ExceptionHandler.pyc \
    PLCPython/util/MiniTextControler.pyc \
    PLCPython/util/misc.pyc \
    PLCPython/util/paths.pyc \
    PLCPython/util/ProcessLogger.pyc \
    PLCPython/util/TranslationCatalogs.pyc \
    PLCPython/xmlclass/__init__.pyc \
    PLCPython/xmlclass/xmlclass.pyc \
    PLCPython/xmlclass/xsdschema.pyc \
    PLCPython/plcopen/Additional_Function_Blocks.xml \
    PLCPython/plcopen/Standard_Function_Blocks.xml \
    PLCPython/plcopen/TC6_XML_V10.xsd \
    PLCPython/plcopen/TC6_XML_V10_B.xsd \
    PLCPython/plcopen/tc6_xml_v201.xsd \
    PLCPython/xmlclass/po.xml \
    PLCPython/xmlclass/test.xsd \
    PLCPython/plcopen/instance_tagname.xslt \
    PLCPython/plcopen/instances_path.xslt \
    PLCPython/plcopen/pou_block_instances.xslt \
    PLCPython/plcopen/pou_variables.xslt \
    PLCPython/plcopen/variables_infos.xslt \
    PLCPython/plcopen/iec_std.csv \
    PLCPython/plcopen/instance_tagname.ysl2 \
    PLCPython/plcopen/instances_path.ysl2 \
    PLCPython/plcopen/pou_block_instances.ysl2 \
    PLCPython/plcopen/pou_variables.ysl2 \
    PLCPython/plcopen/variables_infos.ysl2 \
    PLCPython/plcopen/__init__.py \
    PLCPython/plcopen/definitions.py \
    PLCPython/plcopen/plcopen.py \
    PLCPython/plcopen/structures.py \
    PLCPython/util/__init__.py \
    PLCPython/util/BitmapLibrary.py \
    PLCPython/util/ExceptionHandler.py \
    PLCPython/util/MiniTextControler.py \
    PLCPython/util/misc.py \
    PLCPython/util/paths.py \
    PLCPython/util/ProcessLogger.py \
    PLCPython/util/TranslationCatalogs.py \
    PLCPython/xmlclass/__init__.py \
    PLCPython/xmlclass/xmlclass.py \
    PLCPython/xmlclass/xsdschema.py \
    PLCPython/myTest.py \
    PLCPython/PLCControler.py \
    PLCPython/PLCGenerator.py \
    PLCPython/ProjectControllerPy.py \
    PLCPython/plcopen/TC6_XML_V101.pdf \
    PLCPython/newtest.exe \
    PLCPython/plcopen/__init__.pyc \
    PLCPython/plcopen/definitions.pyc \
    PLCPython/plcopen/plcopen.pyc \
    PLCPython/plcopen/structures.pyc \
    PLCPython/py_ext/__init__.pyc \
    PLCPython/py_ext/py_ext.pyc \
    PLCPython/py_ext/PythonFileCTNMixin.pyc \
    PLCPython/util/__init__.pyc \
    PLCPython/util/BitmapLibrary.pyc \
    PLCPython/util/ExceptionHandler.pyc \
    PLCPython/util/MiniTextControler.pyc \
    PLCPython/util/misc.pyc \
    PLCPython/util/paths.pyc \
    PLCPython/util/ProcessLogger.pyc \
    PLCPython/util/TranslationCatalogs.pyc \
    PLCPython/xmlclass/__init__.pyc \
    PLCPython/xmlclass/xmlclass.pyc \
    PLCPython/xmlclass/xsdschema.pyc \
    PLCPython/CodeFileTreeNode.pyc \
    PLCPython/ConfigTreeNode.pyc \
    PLCPython/features.pyc \
    PLCPython/NativeLib.pyc \
    PLCPython/PLCControler.pyc \
    PLCPython/PLCGenerator.pyc \
    PLCPython/POULibrary.pyc \
    PLCPython/ProjectControllerPy.pyc \
    PLCPython/plcopen/Additional_Function_Blocks.xml \
    PLCPython/plcopen/Standard_Function_Blocks.xml \
    PLCPython/plcopen/TC6_XML_V10.xsd \
    PLCPython/plcopen/TC6_XML_V10_B.xsd \
    PLCPython/plcopen/tc6_xml_v201.xsd \
    PLCPython/py_ext/pous.xml \
    PLCPython/py_ext/py_ext_xsd.xsd \
    PLCPython/xmlclass/po.xml \
    PLCPython/xmlclass/test.xsd \
    PLCPython/NativeLib.xml \
    PLCPython/plc.xml \
    PLCPython/plcopen/instance_tagname.xslt \
    PLCPython/plcopen/instances_path.xslt \
    PLCPython/plcopen/pou_block_instances.xslt \
    PLCPython/plcopen/pou_variables.xslt \
    PLCPython/plcopen/variables_infos.xslt \
    PLCPython/plcopen/iec_std.csv \
    PLCPython/plcopen/instance_tagname.ysl2 \
    PLCPython/plcopen/instances_path.ysl2 \
    PLCPython/plcopen/pou_block_instances.ysl2 \
    PLCPython/plcopen/pou_variables.ysl2 \
    PLCPython/plcopen/variables_infos.ysl2 \
    PLCPython/generated_plc.st \
    PLCPython/plcopen/__init__.py \
    PLCPython/plcopen/definitions.py \
    PLCPython/plcopen/plcopen.py \
    PLCPython/plcopen/structures.py \
    PLCPython/py_ext/__init__.py \
    PLCPython/py_ext/py_ext.py \
    PLCPython/py_ext/PythonFileCTNMixin.py \
    PLCPython/util/__init__.py \
    PLCPython/util/BitmapLibrary.py \
    PLCPython/util/ExceptionHandler.py \
    PLCPython/util/MiniTextControler.py \
    PLCPython/util/misc.py \
    PLCPython/util/paths.py \
    PLCPython/util/ProcessLogger.py \
    PLCPython/util/TranslationCatalogs.py \
    PLCPython/xmlclass/__init__.py \
    PLCPython/xmlclass/xmlclass.py \
    PLCPython/xmlclass/xsdschema.py \
    PLCPython/CodeFileTreeNode.py \
    PLCPython/ConfigTreeNode.py \
    PLCPython/features.py \
    PLCPython/myTest.py \
    PLCPython/NativeLib.py \
    PLCPython/PLCControler.py \
    PLCPython/PLCGenerator.py \
    PLCPython/POULibrary.py \
    PLCPython/ProjectControllerPy.py \
    PLCPython/py_ext/README

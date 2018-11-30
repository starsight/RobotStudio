#ifndef PLCPROJECTCTRL_H
#define PLCPROJECTCTRL_H

#include <QWidget>
#include <Python.h>

static const int ITEM_DATATYPES = 8;
static const int ITEM_FUNCTION = 9;
static const int ITEM_FUNCITONBLOCK = 10;
static const int ITEM_PROGRAM = 11;
static const int ITEM_CONFIGURATIONS = 14;
static const int ITEM_RESOURCES = 15;

class PLCProjectCtrl:public QObject
{
    Q_OBJECT
public:
    PLCProjectCtrl(QWidget* windows);
    ~PLCProjectCtrl();
    bool createController();
    void setProjectBuildPath(QString path);
    void NewPLCProject(QString path);
    void LoadPLCProject(QString path);
    void SaveProject();
    void generatePLCSTCode();
    // 获取xml资源配置函数
    QString getProjectCurrentConfigName();
    QStringList getBlockResource();
    QStringList getEditedResourceVariables(QString);
    void getEditedResourceInfos(QString);
    void getEditedResourceElemnetVariables(const QString& configName,const QString& resourceName, QList<QStringList>& varAttributes);
    // 设置xml资源配置相关函数
    void projectAddConfigurationResource(QString configName, QString resourceName);
    void projectRemoveConfigurationResource(QString configName,QString resourceName);
    void setEditedResourceInfos(QString tagName,QList<QHash<QString,QString>>& curTasksInfo, QList<QHash<QString,QString>>& curInstancesInfo);
    void setEditedResourceElementVariables(const QString& configName,const QString& resourceName,const QList<QStringList>& varsList);
    // 获取xml变量相关函数
    void getBaseTypes(QStringList& baseType);
    void getDataTypes(QStringList& dataType);
    void getConfNodeDataTypes(QStringList& libName, QList<QStringList>& libTypes);
    void getFunctionBlockTypes(const QString& tagName,QStringList& fbTypes);
    void getEditedElementInterfaceVars(const QString& tagName, QList<QStringList>& varAttributes);
    QString getEditedElementInterfaceReturnType(const QString& tagName);
    // 设置xml变量相关函数
    void setPouInterfaceReturnType(const QString& tagName, const QString& return_type);
    void setPouInterfaceVars(const QString& name, const QList<QStringList>& varsList);
    // POU增删修改函数
    void projectAddPou(const QString& pou_name, const QString& pou_type);
    void projectRemovePou(const QString& pou_name);
    void changePouName(const QString& old_name, const QString& new_name);
    void getEditedPouElementText(const QString& tagName, QString& text);
    void setEditedPouElementText(const QString& tagName,const QString& text);
    // 获取功能块库
    void getLibraryNodes(QStringList& libNames, QList<QStringList>& libContents);
signals:
    void updateProjectTree(QStringList,QList<int>);
private:
    bool parsePyDict2QtHashStr(PyObject* pyDict, QHash<QString,QString>& qtHash);
    void getProjectInfos();
    void getProjectInfosHelper(PyObject* itemDict,int itemType,QStringList& treeItemNameList, QList<int>& treeItemTypeList);
    PyObject* parseQtHashStr2PyDict(QHash<QString,QString>& qtHash);
public:
    QList<QHash<QString,QString>> tasksInfo;
    QList<QHash<QString,QString>> instancesInfo;
private:
    PyObject* PLCModule;
    PyObject* ModuleDict;
    PyObject* PLCController;

    QString* ProjectPath;

    QString confName;
};

#endif // PLCPROJECTCTRL_H

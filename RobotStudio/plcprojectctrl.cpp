#include "plcprojectctrl.h"

#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <iostream>

PLCProjectCtrl::PLCProjectCtrl(QWidget* windows):QObject(windows)
{
    Py_Initialize();
    qDebug() << "Python initialize success";
    if(!Py_IsInitialized()){
        QMessageBox::warning(windows,tr("Python初始化错误"),tr("请检查python配置"));
        return;
    }
    PyRun_SimpleString("import sys");
//    PyRun_SimpleString("sys.path.append('../RobotStudio/PLCPython')");
    PyRun_SimpleString("print sys.path");
    qDebug() << "import sys success";
    PLCModule = PyImport_ImportModule("ProjectControllerPy");

    if(!PLCModule){
        QMessageBox::warning(windows,tr("模块加载错误"),tr("ProjectControllerPy导入错误"));
        return;
    }
    qDebug() << "import Module ProjectControllerPy success";
    ModuleDict = PyModule_GetDict(PLCModule);
    qDebug() << "get module dictionary successfully";
    PyObject* pFun = PyDict_GetItemString(ModuleDict, "convertProjectPathFromC");
    qDebug() << "================================";

    QDir dir;
    QString path = dir.currentPath();
    std::string stdpath = path.toStdString();
    qDebug() << stdpath.c_str();
    PyObject_CallFunction(pFun, (char*)"s", stdpath.c_str());
    PLCController = nullptr;
}

PLCProjectCtrl::~PLCProjectCtrl()
{
    Py_CLEAR(PLCController);
    Py_CLEAR(ModuleDict);
    Py_CLEAR(PLCModule);
    Py_Finalize();
    PLCController = nullptr;
    ModuleDict = nullptr;
    PLCModule = nullptr;
    qDebug() << "====================== PLCProject Clear finish =======================";
}


bool PLCProjectCtrl::createController()
{
    if(PLCController != nullptr) Py_DecRef(PLCController);
    PyObject* controllerClass = PyDict_GetItemString(ModuleDict,"ProjectControllerPy");
    if(!controllerClass){
        QMessageBox::warning(nullptr,tr("未找到模块"),tr("没有ProjectControllerPy"));
        return false;
    }
    PLCController = PyInstance_New(controllerClass,NULL,NULL);
    if(!PLCController){
        QMessageBox::warning(nullptr,tr("无法创建实例"),tr("无法创建controller实例"));
        return false;
    }
    return true;
}

void PLCProjectCtrl::NewPLCProject(QString path)
{
    if(createController()){
        std::string stdpath = path.toStdString();
        PyObject_CallMethod(PLCController,(char*)"NewProject",(char*)"s",stdpath.c_str());
        setProjectBuildPath(path);
    }
}

void PLCProjectCtrl::LoadPLCProject(QString path)
{
    if(createController()){
        std::string stdpath = path.toStdString();
        qDebug() << stdpath.c_str() << "================";
        PyObject_CallMethod(PLCController,(char*)"LoadProject",(char*)"s",stdpath.c_str());
        getProjectInfos();
    }
}

void PLCProjectCtrl::setProjectBuildPath(QString path)
{
    std::string stdpath = path.toStdString();
    PyObject_CallMethod(PLCController,(char*)"_setBuildPath",(char*)"s",stdpath.c_str());
}

void PLCProjectCtrl::SaveProject()
{
    PyObject_CallMethod(PLCController,(char*)"SaveProject",NULL);
}

void PLCProjectCtrl::generatePLCSTCode()
{
    PyObject_CallMethod(PLCController,(char*)"Generate_PLC_ST",NULL);
}

//============================ 获取xml文件资源配置函数 =====================================================
/**
 * @brief getProjectCurrentConfigName
 * @return
 */
QString PLCProjectCtrl::getProjectCurrentConfigName()
{
//    if(confName.isEmpty()){
        PyObject* confname = PyObject_CallMethod(PLCController,(char*)"getProjectCurrentConfigName",NULL);
        if(!confname){
            QMessageBox::warning(nullptr,tr("PLC控制器方法错误"),tr("getProjectCurrentConfigName调用失败"));
            return QString();
        }else
            qDebug() << "getProjectCurrentConfigName成功";
        QString retname = QString(PyString_AsString(confname));
//        qDebug() << retname;
        confName = QString(PyString_AsString(confname));
        Py_DECREF(confname);
        return retname;
//    }
//    return confName;
}

/**
 * @brief PLCProjectCtrl::getBlockResource  获取用户定义的所有Program
 * @return
 */
QStringList PLCProjectCtrl::getBlockResource()
{
    QStringList progStrList;
    PyObject* progList = PyObject_CallMethod(PLCController,(char*)"getBlockResource",NULL);
    if(!progList){
        QMessageBox::warning(nullptr,tr("PLC控制器方法错误"),tr("getBlockResource调用失败"));
        return progStrList;
    }
    else
        qDebug() << "模块调用成功";
    if(PyList_Check(progList)){
        int len = PyList_Size(progList);
        for(int i = 0; i < len; ++i){
            PyObject* progStr = PyList_GetItem(progList,i);
            QString tmp = QString(PyString_AsString(progStr));
            progStrList << tmp;
//            Py_DECREF(progStr); 这里不能一个一个删
        }
    }
    Py_DECREF(progList);
    return progStrList;
}

/**
 * @brief PLCProjectCtrl::getEditedResourceVariables  获取资源配置中的可以用作中断触发的全局变量
 * @param tagName
 * @return
 */
QStringList PLCProjectCtrl::getEditedResourceVariables(QString tagName)
{
    std::string tmpName = tagName.toStdString();
    QStringList varNameList;
//    qDebug() << tmpName.c_str();
    if(!PLCController)
    {
        QMessageBox::warning(nullptr,tr("PLC控制器错误"),tr("控制器指针为空"));
        return varNameList;
    }else
        qDebug() << "控制器指针正常";
    PyObject* varList = PyObject_CallMethod(PLCController,(char*)"getEditedResourceVariables",(char*)"s",tmpName.c_str());
    if(!varList){
        QMessageBox::warning(nullptr,tr("PLC控制器方法错误"),tr("getEditedResourceVariables调用失败"));
        return varNameList;
    }
    else
        qDebug() << "模块调用成功";
    if(PyList_Check(varList)){
        int len = PyList_Size(varList);
        for(int i = 0; i < len; ++i){
            PyObject* varObj = PyList_GetItem(varList,i);
            QString varName = QString(PyString_AsString(varObj));
            varNameList << varName;
            Py_DECREF(varObj);
        }
    }
    Py_DECREF(varList);
    return varNameList;
}

/**
 * @brief PLCProjectCtrl::parsePyDict2QtHashStr  将python中的dict类型转换为Qt中的QHash<QString,QString>
 * @param pyDict
 * @param qtHash
 * @return 判断输入参数，若为Python中的dict类型，返回true，否则返回false
 */
bool PLCProjectCtrl::parsePyDict2QtHashStr(PyObject* pyDict, QHash<QString,QString>& qtHash)
{
    if(PyDict_Check(pyDict)){
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while(PyDict_Next(pyDict,&pos,&key,&value))
        {
            QString QtKey = QString(PyString_AsString(key));
            QString QtVal = QString(PyString_AsString(value));
            qtHash[QtKey] = QtVal;
        }
        return true;
    }
    return false;
}

/**
 * @brief PLCProjectCtrl::getEditedResourceInfos  获取XML文件资源中所有任务和实例信息
 * @param tagName
 */
void PLCProjectCtrl::getEditedResourceInfos(QString tagName)
{
    tasksInfo.clear();
    instancesInfo.clear();
    std::string tmpName = tagName.toStdString();
//    qDebug() << "============== getEditedResourceInfos 0============= " << tagName;
    if(!PLCController)
    {
        QMessageBox::warning(nullptr,tr("PLC控制器错误"),tr("控制器指针为空"));
        return;
    }else
        qDebug() << "控制器指针正常";
    PyObject* infoTuple = PyObject_CallMethod(PLCController,(char*)"getEditedResourceInfos",(char*)"s",tmpName.c_str());
    if(!infoTuple){
        QMessageBox::warning(nullptr,tr("PLC控制器方法错误"),tr("getEditedResourceInfos调用失败"));
        return;
    }
//    qDebug() << "======================== hhhh === " << PyTuple_Size(infoTuple);
    if(PyTuple_Size(infoTuple) > 0)
    {
        PyObject* firstItem = PyTuple_GetItem(infoTuple,0);
        if(PyList_Check(firstItem)){
            int len = PyList_Size(firstItem);
//            qDebug() << "============== getEditedResourceInfos 4============= " << tagName << " " << len;
            for(int i = 0; i < len; ++i)
            {
                PyObject* taskDict = PyList_GetItem(firstItem,i);
                QHash<QString,QString> oneTask;
                if(parsePyDict2QtHashStr(taskDict,oneTask))
                    tasksInfo.append(oneTask);
                Py_XDECREF(taskDict);
            }
        }
        Py_DECREF(firstItem);

        PyObject* secondItem = PyTuple_GetItem(infoTuple,1);
        if(PyList_Check(secondItem))
        {
            int len = PyList_Size(secondItem);
            for(int i = 0; i < len; ++i)
            {
                PyObject* instanceDict = PyList_GetItem(secondItem,i);
                QHash<QString,QString> oneInstance;
                if(parsePyDict2QtHashStr(instanceDict,oneInstance))
                    instancesInfo.append(oneInstance);
                Py_DECREF(instanceDict);
            }
        }
        Py_DECREF(secondItem);
    }
    Py_DECREF(infoTuple);
    qDebug() << "======================= Resource information from XML ==============================";
    qDebug() << tasksInfo;
    qDebug() << instancesInfo;
    qDebug() << "======================= Resource information from XML ==============================";
    qDebug() << "\n";
}

/**
 * @brief PLCProjectCtrl::getEditedResourceElemnetVariables 从XML中获取资源配置窗口变量
 * @param configName
 * @param resourceName
 * @param varAttributes
 */
void PLCProjectCtrl::getEditedResourceElemnetVariables(const QString& configName,const QString& resourceName,
                                                                                QList<QStringList>& varAttributes)
{
    std::string configStdName = configName.toStdString();
    std::string resourceStdName = resourceName.toStdString();
    PyObject* varsInfoPy = PyObject_CallMethod(PLCController,(char*)"getEditedResourceElemnetVariables",(char*)"ss",configStdName.c_str(),resourceStdName.c_str());
    if(varsInfoPy && PyList_Check(varsInfoPy)){
        int len = PyList_Size(varsInfoPy);
        for(int i = 0; i < len; ++i){
            PyObject* oneVarInfoPy = PyList_GetItem(varsInfoPy,i);
            if(oneVarInfoPy && PyList_Check(oneVarInfoPy)){
                QStringList oneVarInfo;
                int attrNum = PyList_Size(oneVarInfoPy);
                for(int j = 0; j < attrNum; ++j){
                    PyObject* attrPy = PyList_GetItem(oneVarInfoPy,j);
                    oneVarInfo.append(PyString_AsString(attrPy));
                }
                varAttributes.append(oneVarInfo);
            }
        }
    }
    Py_DECREF(varsInfoPy);
}

/**
 * @brief PLCProjectCtrl::getProjectInfosHelper 获取PLC工程信息辅助函数
 * @param itemDict              该对象的字典
 * @param itemType              该对象应该的类型
 * @param treeItemNameList[out] 返回的对象名列表
 * @param treeItemTypeList[out] 返回的对象类型列表
 */
void PLCProjectCtrl::getProjectInfosHelper(PyObject* itemDict,int itemType,QStringList& treeItemNameList, QList<int>& treeItemTypeList)
{
    if(itemDict && PyDict_Check(itemDict))
    {
        PyObject* valuekey = Py_BuildValue((char*)"s",(char*)"values");
        PyObject* typekey = Py_BuildValue((char*)"s",(char*)"type");
        PyObject* namekey = Py_BuildValue((char*)"s",(char*)"name");

        PyObject* thirdLevel = PyDict_GetItem(itemDict,valuekey);
        PyObject* typeval = PyDict_GetItem(itemDict,typekey);
        int elmtype = PyInt_AsLong(typeval);
        if(elmtype == itemType)
        {
            if(thirdLevel && PyList_Check(thirdLevel))
            {
                qDebug() << "Third level";
                int len = PyList_Size(thirdLevel);
                for(int i = 0; i < len; ++i)
                {
                    PyObject* thirdLevItem = PyList_GetItem(thirdLevel,i);
                    if(thirdLevItem && PyDict_Check(thirdLevItem)){
                        PyObject* nameVal = PyDict_GetItem(thirdLevItem,namekey);
                        QString nameQtStr(PyString_AsString(nameVal));
                        treeItemNameList.append(nameQtStr);
                        treeItemTypeList.append(elmtype);
                    }
                }
            }
        }else
        {
            switch(itemType)
            {
            case ITEM_DATATYPES:
                QMessageBox::warning(nullptr,tr("类型错误"),tr("不是数据类型"));break;
            case ITEM_FUNCTION:
                QMessageBox::warning(nullptr,tr("类型错误"),tr("不是函数类型"));break;
            case ITEM_FUNCITONBLOCK:
                QMessageBox::warning(nullptr,tr("类型错误"),tr("不是功能块类型")); break;
            case ITEM_PROGRAM:
                QMessageBox::warning(nullptr,tr("类型错误"),tr("不是程序类型"));break;
            case ITEM_RESOURCES:
                QMessageBox::warning(nullptr,tr("类型错误"),tr("不是资源类型")); qDebug() << elmtype; break;
            }
        }
        Py_XDECREF(valuekey);
        Py_XDECREF(typekey);
        Py_XDECREF(namekey);
        Py_XDECREF(thirdLevel);
        Py_XDECREF(typeval);
    }
}

/*=====================================================================================================================
{'values': [{'values': [{'values': [], 'type': 7, 'name': 'datatype0', 'tagname': 'D::datatype0'},
                        {'values': [], 'type': 7, 'name': 'datatype1', 'tagname': 'D::datatype1'}],
                'type': 8,
                'name': 'Data Types'},
            {'values': [{'values': [], 'type': 1, 'name': 'fun0', 'tagname': 'P::fun0'}],
                'type': 9,
                'name': 'Functions'},
            {'values': [{'values': [], 'type': 1, 'name': 'FB0', 'tagname': 'P::FB0'}],
                'type': 10,
                'name': 'Function Blocks'},
            {'values': [{'values': [], 'type': 1, 'name': 'program0', 'tagname': 'P::program0'},
                        {'values': [], 'type': 1, 'name': 'program1', 'tagname': 'P::program1'}],
                'type': 11,
                'name': 'Programs'},
            {'values': [{'values': [{'values': [{'values': [], 'type': 6, 'name': 'resource1', 'tagname': 'R::config::resource1'},
                                                {'values': [], 'type': 6, 'name': 'resource0', 'tagname': 'R::config::resource0'}],
                                    'type': 15,
                                    'name': 'Resources'}],
                        'type': 5,
                        'name': 'config',
                        'tagname': 'C::config'}],
                'type': 14,
                'name': 'Configurations'}],
 'type': 0,
 'name': 'Project1'}
 ==========================================================================================================================*/
/**
 * @brief PLCProjectCtrl::getProjectInfos
 */
void PLCProjectCtrl::getProjectInfos()
{
    PyObject* projInfo = PyObject_CallMethod(PLCController,(char*)"getProjectInfos",NULL);
    if(!projInfo){
        QMessageBox::warning(nullptr,tr("controller方法导入错误"),tr("getProjectInfos导入失败"));
        return;
    }
    if(PyDict_Check(projInfo))
    {
        qDebug() << "Project level";
        PyObject* valuekey = Py_BuildValue((char*)"s",(char*)"values");
        PyObject* typekey = Py_BuildValue((char*)"s",(char*)"type");
//        PyObject* namekey = Py_BuildValue((char*)"s",(char*)"name");
        PyObject* secLevel = PyDict_GetItem(projInfo,valuekey);
        if(secLevel && PyList_Check(secLevel))
        {
            QStringList treeItemNameList;
            QList<int> treeItemTypeList;
            qDebug() << "Second level";
            // 在Python的工程列表中 0：datatype 1：functions 2：function blocks 3：programs 4：configurations
            PyObject* secLevItem = PyList_GetItem(secLevel,0);
            getProjectInfosHelper(secLevItem,ITEM_DATATYPES,treeItemNameList,treeItemTypeList);
            Py_XDECREF(secLevItem);
            secLevItem = PyList_GetItem(secLevel,1);
            getProjectInfosHelper(secLevItem,ITEM_FUNCTION,treeItemNameList,treeItemTypeList);
            Py_XDECREF(secLevItem);
            secLevItem = PyList_GetItem(secLevel,2);
            getProjectInfosHelper(secLevItem,ITEM_FUNCITONBLOCK,treeItemNameList,treeItemTypeList);
            Py_XDECREF(secLevItem);
            secLevItem = PyList_GetItem(secLevel,3);
            getProjectInfosHelper(secLevItem,ITEM_PROGRAM,treeItemNameList,treeItemTypeList);
            Py_XDECREF(secLevItem);

            //加载资源
            secLevItem = PyList_GetItem(secLevel,4);
            if(secLevItem && PyDict_Check(secLevItem)){
                PyObject* typeval = PyDict_GetItem(secLevItem,typekey);
                int elmtype = PyInt_AsLong(typeval);
                if(elmtype == ITEM_CONFIGURATIONS){
                    PyObject* thirdLevel = PyDict_GetItem(secLevItem,valuekey);
                    qDebug() << "第三层元素个数 = "  << PyList_Size(thirdLevel);
                    if(thirdLevel && PyList_Check(thirdLevel)){
                        // 好像一般只有一个配置吧
                        PyObject* thirdLevItem = PyList_GetItem(thirdLevel,0);
                        if(thirdLevItem && PyDict_Check(thirdLevItem))
                        {
                            PyObject* fourthLevel = PyDict_GetItem(thirdLevItem,valuekey);
                            qDebug() << "第四层元素个数 = " << PyList_Size(fourthLevel);
                            if(fourthLevel && PyList_Check(fourthLevel)){
                                PyObject* resourceItem = PyList_GetItem(fourthLevel,0);
                                getProjectInfosHelper(resourceItem,ITEM_RESOURCES,treeItemNameList,treeItemTypeList);
                                Py_XDECREF(resourceItem);
                            }
                            Py_XDECREF(fourthLevel);
                        }
                        Py_XDECREF(thirdLevItem);
                    }
                    Py_XDECREF(thirdLevel);
                }else
                    QMessageBox::warning(nullptr,tr("类型错误"),tr("不是配置类型"));
                Py_XDECREF(typeval);
            }

            if(!treeItemNameList.isEmpty())
                emit updateProjectTree(treeItemNameList,treeItemTypeList);
        }
        Py_XDECREF(secLevel);
        Py_XDECREF(typekey);
        Py_XDECREF(valuekey);
    }
}

//============================ 设置 xml文件资源配置函数 =====================================================
/**
 * @brief PLCProjectCtrl::projectAddConfigurationResource 更新Project中的configuration节点
 * @param configName
 * @param resourceName
 */
void PLCProjectCtrl::projectAddConfigurationResource(QString configName, QString resourceName)
{
    std::string cfgStdName = configName.toStdString();
    std::string resStdName = resourceName.toStdString();
    PyObject_CallMethod(PLCController,(char*)"projectAddConfigurationResource",(char*)"ss",cfgStdName.c_str(),resStdName.c_str());
    SaveProject();  // for test
}

/**
 * @brief PLCProjectCtrl::projectRemoveConfigurationResource 更新Project中的configuration节点
 * @param configName
 * @param resourceName
 */
void PLCProjectCtrl::projectRemoveConfigurationResource(QString configName,QString resourceName)
{
    std::string cfgStdName = configName.toStdString();
    std::string resStdName = resourceName.toStdString();
    PyObject_CallMethod(PLCController,(char*)"projectRemoveConfigurationResource",(char*)"ss",cfgStdName.c_str(),resStdName.c_str());
    SaveProject();  // for test
}

/**
 * @brief PLCProjectCtrl::parseQtHashStr2PyDict
 * @param qtHash
 * @return  返回对应的字典
 */
PyObject* PLCProjectCtrl::parseQtHashStr2PyDict(QHash<QString,QString>& qtHash)
{
    PyObject* pyDict = PyDict_New();
    QHash<QString,QString>::iterator it = qtHash.begin();
    for(;it != qtHash.end();++it){
        std::string stdKey = it.key().toStdString();
        std::string stdVal = it.value().toStdString();
        PyObject* pyVal = Py_BuildValue((char*)"s",stdVal.c_str());
        int ret = PyDict_SetItemString(pyDict,stdKey.c_str(),pyVal);
        if(ret){
            qDebug() << "******** parseQtHashStr2PyDict error **********";
            return pyDict;
        }
    }
    return pyDict;
}

/**
 * @brief PLCProjectCtrl::setEditedResourceInfos 将任务模型与实例模型中更新值写入XML文件中
 * @param tagName               该资源节点名
 * @param curTasksInfo          任务信息
 * @param curInstancesInfo      实例信息
 */
void PLCProjectCtrl::setEditedResourceInfos(QString tagName,QList<QHash<QString,QString>>& curTasksInfo, QList<QHash<QString,QString>>& curInstancesInfo)
{
    qDebug() << "====================== In setEditedResourceInfos ==========================";
    qDebug() << curTasksInfo;
    qDebug() << curInstancesInfo;
    qDebug() << "====================== In setEditedResourceInfos ==========================";
    PyObject* pyMethodName = Py_BuildValue("s","setEditedResourceInfos");
    std::string tagStdName = tagName.toStdString();
    PyObject* pyTagName = Py_BuildValue((char*)"s",tagStdName.c_str());
    int tasksCnt = curTasksInfo.size();
    int instancesCnt = curInstancesInfo.size();
    PyObject* pyTaskList = PyList_New(tasksCnt);
    for(int i = 0; i < tasksCnt; ++i){
        PyObject* pyDict = parseQtHashStr2PyDict(curTasksInfo[i]);
        PyList_SetItem(pyTaskList,i,pyDict);
    }
    PyObject* pyInstanceList = PyList_New(instancesCnt);
    for(int i = 0; i < instancesCnt; ++i){
        PyObject* pyDict = parseQtHashStr2PyDict(curInstancesInfo[i]);
        PyList_SetItem(pyInstanceList,i,pyDict);
    }
    PyObject_CallMethodObjArgs(PLCController,pyMethodName,pyTagName,pyTaskList,pyInstanceList,NULL);
    Py_DECREF(pyTaskList);
    Py_DECREF(pyInstanceList);
    Py_DECREF(pyTagName);
    Py_DECREF(pyMethodName);
}

/**
 * @brief PLCProjectCtrl::setEditedResourceElementVariables 将资源配置界面中的变量更新到PLCControler
 * @param configName
 * @param resourceName
 * @param varsList
 */
void PLCProjectCtrl::setEditedResourceElementVariables(const QString& configName,const QString& resourceName,const QList<QStringList>& varsList)
{
    std::string configStdName = configName.toStdString();
    std::string resourceStdName = resourceName.toStdString();
    int varsCnt = varsList.size();
    PyObject* varsListPy = PyList_New(varsCnt);
    for(int i = 0; i < varsCnt; ++i)
    {
        int varAttrCnt = varsList[i].length();
        PyObject* oneVarPy = PyList_New(varAttrCnt);
        for(int j = 0; j < varAttrCnt; ++j){
            std::string attrStd = varsList[i][j].toStdString();
            PyObject* oneAttr = Py_BuildValue("s",attrStd.c_str());
            PyList_SetItem(oneVarPy,j,oneAttr);
        }
        PyList_SetItem(varsListPy,i,oneVarPy);
    }
    PyObject* methodNamePy = Py_BuildValue("s","setEditedResourceElementVariables");
    PyObject* configNamePy = Py_BuildValue("s",configStdName.c_str());
    PyObject* resourceNamePy = Py_BuildValue("s",resourceStdName.c_str());
    PyObject_CallMethodObjArgs(PLCController,methodNamePy,configNamePy,resourceNamePy,varsListPy,NULL);
    Py_DECREF(methodNamePy);
    Py_DECREF(resourceNamePy);
    Py_DECREF(configNamePy);
    Py_DECREF(varsListPy);
}


//========================================== 变量类型获取设置函数 ============================================
/**
 * @brief PLCProjectCtrl::getBaseTypes      获取基础类型
 * @param baseType[out]
 */
void PLCProjectCtrl::getBaseTypes(QStringList& baseTypeList)
{
    PyObject* baseTypePy = PyObject_CallMethod(PLCController,(char*)"getBaseTypes",NULL);
    if(baseTypePy && PyList_Check(baseTypePy))
    {
        int len = PyList_Size(baseTypePy);
        for(int i = 0; i < len; ++i){
            PyObject* oneTypePy = PyList_GetItem(baseTypePy,i);
            QString oneType(PyString_AsString(oneTypePy));
            baseTypeList.append(oneType);
        }
    }
    Py_DECREF(baseTypePy);
}

/**
 * @brief PLCProjectCtrl::getDataTypes   获取用户自定义类型
 * @param dataType[out]
 */
void PLCProjectCtrl::getDataTypes(QStringList& dataTypeList)
{
    PyObject* dataTypePy = PyObject_CallMethod(PLCController,(char*)"getDataTypes",NULL);
    if(dataTypePy && PyList_Check(dataTypePy)){
        int len = PyList_Size(dataTypePy);
        for(int i = 0; i < len; ++i){
            PyObject* oneTypePy = PyList_GetItem(dataTypePy,i);
            QString oneType = QString(PyString_AsString(oneTypePy));
            dataTypeList.append(oneType);
        }
    }
    Py_DECREF(dataTypePy);
}

/**
 * @brief PLCProjectCtrl::getConfNodeDataTypes  获取库内变量类型
 * @param libName[out]      获取库名
 * @param libTypes[out]     获取库内类型
 */
void PLCProjectCtrl::getConfNodeDataTypes(QStringList& libNameList, QList<QStringList>& libTypesList)
{
    PyObject* libTypesPy = PyObject_CallMethod(PLCController,(char*)"getConfNodeDataTypes",NULL);
    qDebug() << "======================= getConfNodeDataTypes ===============================";
    if(libTypesPy && PyList_Check(libTypesPy)){
        int len = PyList_Size(libTypesPy);
        PyObject* nameKey = Py_BuildValue((char*)"s","name");
        PyObject* listKey = Py_BuildValue((char*)"s","list");
        for(int i = 0; i < len; ++i)
        {
            qDebug() << "================ first LEVEL =====================================";
            qDebug() << len;
            PyObject* oneLibTypesPy = PyList_GetItem(libTypesPy,i);
            if(oneLibTypesPy && PyDict_Check(oneLibTypesPy))
            {
                PyObject* libNamePy = PyDict_GetItem(oneLibTypesPy,nameKey);
                QString libName = QString(PyString_AsString(libNamePy));
                libNameList.append(libName);
                PyObject* libTypesPy = PyDict_GetItem(oneLibTypesPy,listKey);
                QStringList libTypes;
                if(libTypesPy && PyList_Check(libTypesPy)){
                    qDebug() <<"===================== second LEVEL ==========================";
                    int oneLibLen = PyList_Size(libTypesPy);
                    qDebug() << oneLibLen;
                    for(int j = 0; j < oneLibLen; ++j){
                        PyObject* curLibOneTypePy = PyList_GetItem(libTypesPy,j);
                        QString curLibOneType = QString(PyString_AsString(curLibOneTypePy));
                        libTypes.append(curLibOneType);
                    }
                }
                libTypesList.append(libTypes);
//                Py_DECREF(libTypesPy);
//                Py_DECREF(libNamePy);
            }
        }
        Py_DECREF(listKey);
        Py_DECREF(nameKey);
    }
    Py_DECREF(libTypesPy);
}

/**
 * @brief PLCProjectCtrl::getFunctionBlockTypes  获取功能块类型
 * @param tagName[in]           当前需要获取功能块名称
 * @param fbTypes[out]          获取功能块类型
 */
void PLCProjectCtrl::getFunctionBlockTypes(const QString& tagName,QStringList& fbTypesList)
{
    std::string tagStdName = tagName.toStdString();
    PyObject* fbTypesListPy = PyObject_CallMethod(PLCController,(char*)"getFunctionBlockTypes",(char*)"s",tagStdName.c_str());
    if(fbTypesListPy && PyList_Check(fbTypesListPy)){
        int len = PyList_Size(fbTypesListPy);
        for(int i = 0; i < len; ++i){
            PyObject* oneFbTypePy = PyList_GetItem(fbTypesListPy,i);
            QString oneFbType = QString(PyString_AsString(oneFbTypePy));
            fbTypesList.append(oneFbType);
        }
    }
    Py_DECREF(fbTypesListPy);
}

/**
 * @brief PLCProjectCtrl::getEditedElementInterfaceVars  获取XML文件中变量原始信息
 * @param tagName
 * @param varAttributes
 */
void PLCProjectCtrl::getEditedElementInterfaceVars(const QString& tagName, QList<QStringList>& varAttributes)
{
    std::string tagStdName = tagName.toStdString();
    PyObject* varsInfoPy = PyObject_CallMethod(PLCController,(char*)"getEditedElementInterfaceVars",(char*)"s",tagStdName.c_str());
    if(varsInfoPy && PyList_Check(varsInfoPy)){
        int len = PyList_Size(varsInfoPy);
        for(int i = 0; i < len; ++i){
            PyObject* oneVarInfoPy = PyList_GetItem(varsInfoPy,i);
            if(oneVarInfoPy && PyList_Check(oneVarInfoPy)){
                QStringList oneVarInfo;
                int attrNum = PyList_Size(oneVarInfoPy);
                for(int j = 0; j < attrNum; ++j){
                    PyObject* attrPy = PyList_GetItem(oneVarInfoPy,j);
                    oneVarInfo.append(PyString_AsString(attrPy));
                }
                varAttributes.append(oneVarInfo);
            }
        }
    }
    Py_DECREF(varsInfoPy);
}

/**
 * @brief PLCProjectCtrl::getEditedElementInterfaceReturnType 获取初始返回值类型
 * @param tagName
 * @return
 */
QString PLCProjectCtrl::getEditedElementInterfaceReturnType(const QString& tagName)
{
    std::string tagStdName = tagName.toStdString();
    PyObject* retTypePy = PyObject_CallMethod(PLCController,(char*)"getEditedElementInterfaceReturnType",(char*)"s",tagStdName.c_str());
    return PyString_AsString(retTypePy);
}

/**
 * @brief PLCProjectCtrl::setPouInterfaceReturnType 设置函数返回类型
 * @param tagName[in]
 * @param return_type[in]
 */
void PLCProjectCtrl::setPouInterfaceReturnType(const QString& name, const QString& return_type)
{
    qDebug() << "=================== setPouInterfaceReturnType ========================";
    std::string stdName = name.toStdString();
    std::string retType = return_type.toStdString();
//    std::cout << stdName << " " << retType << std::endl;
    PyObject_CallMethod(PLCController,(char*)"setPouInterfaceReturnType",(char*)"ss",stdName.c_str(),retType.c_str());
}

/**
 * @brief PLCProjectCtrl::setPouInterfaceVars
 * @param name[in]
 * @param value[in]
 */
void PLCProjectCtrl::setPouInterfaceVars(const QString& name, const QList<QStringList>& varsList)
{
    std::string stdName = name.toStdString();
    int varsCnt = varsList.size();
    PyObject* varsListPy = PyList_New(varsCnt);
    for(int i = 0; i < varsCnt; ++i)
    {
        int varAttrCnt = varsList[i].length();
        PyObject* oneVarPy = PyList_New(varAttrCnt);
        for(int j = 0; j < varAttrCnt; ++j){
            std::string attrStd = varsList[i][j].toStdString();
            PyObject* oneAttr = Py_BuildValue("s",attrStd.c_str());
            PyList_SetItem(oneVarPy,j,oneAttr);
        }
        PyList_SetItem(varsListPy,i,oneVarPy);
    }
    PyObject* methodNamePy = Py_BuildValue("s","setPouInterfaceVars");
    PyObject* tagNamePy = Py_BuildValue("s",stdName.c_str());
    PyObject_CallMethodObjArgs(PLCController,methodNamePy,tagNamePy,varsListPy,NULL);
    Py_DECREF(methodNamePy);
    Py_DECREF(tagNamePy);
    Py_DECREF(varsListPy);
}

//========================================= POU增删修改函数 ============================================
/**
 * @brief PLCProjectCtrl::projectAddPou
 * @param pou_name
 * @param pou_type
 */
void PLCProjectCtrl::projectAddPou(const QString& pou_name, const QString& pou_type)
{
    std::string pouStdName = pou_name.toStdString();
    std::string pouStdType = pou_type.toStdString();
    PyObject_CallMethod(PLCController,(char*)"projectAddPou",(char*)"ss",pouStdName.c_str(),pouStdType.c_str());
}

/**
 * @brief PLCProjectCtrl::projectRemovePou
 * @param pou_name
 */
void PLCProjectCtrl::projectRemovePou(const QString& pou_name)
{
    std::string pouStdName = pou_name.toStdString();
    PyObject_CallMethod(PLCController,(char*)"projectRemovePou",(char*)"s",pouStdName.c_str());
}


/**
 * @brief PLCProjectCtrl::changePouName
 * @param old_name
 * @param new_name
 */
void PLCProjectCtrl::changePouName(const QString& old_name, const QString& new_name)
{
    std::string pouOldStdName = old_name.toStdString();
    std::string pouNewStdName = new_name.toStdString();
    PyObject_CallMethod(PLCController,(char*)"changePouName",(char*)"ss",pouOldStdName.c_str(),pouNewStdName.c_str());
}


/**
 * @brief PLCProjectCtrl::getEditedPouElementText
 * @param tagName[in]
 * @param text[out]
 */
void PLCProjectCtrl::getEditedPouElementText(const QString& tagName, QString& text)
{
    qDebug() <<"====================== getEditedPouElementText ==========================";
    std::string tagStdName = tagName.toStdString();
    qDebug() << tagStdName.c_str();
    PyObject* pouContentPy = PyObject_CallMethod(PLCController,(char*)"getEditedPouElementText",(char*)"s",tagStdName.c_str());
    if(pouContentPy)
        text = PyString_AsString(pouContentPy);
    Py_DECREF(pouContentPy);
}

/**
 * @brief PLCProjectCtrl::setEditedPouElementText
 * @param tagName[in]
 * @param text[in]
 */
void PLCProjectCtrl::setEditedPouElementText(const QString& tagName,const QString& text)
{
    qDebug() << "================= setEditedPouElemetnText =======================";
    std::string tagStdName = tagName.toStdString();
    std::string pouStdContent = text.toStdString();
//    QByteArray bytes = text.toUtf8();
    qDebug() << tagStdName.c_str();
//    qDebug() << bytes.data();
    qDebug() << pouStdContent.c_str();
    PyObject_CallMethod(PLCController,(char*)"setEditedPouElementText",(char*)"ss",tagStdName.c_str(),pouStdContent.c_str());
}

/**
 * @brief PLCProjectCtrl::getLibraryNodes
 * @param libNames
 * @param libContents
 */
void PLCProjectCtrl::getLibraryNodes(QStringList& libNames, QList<QStringList>& libContents)
{
    qDebug() << "======================== In getLibraryNodes ===========================";
//    PyObject* libsPy = PyObject_CallMethod(PLCController,(char*)"getProjectCurrentConfigName",NULL);
    PyObject* libsPy = PyObject_CallMethod(PLCController,(char*)"getLibraryNodes",NULL);
    if(!libsPy){
        QMessageBox::warning(nullptr,tr("Error"),tr("加载功能块库错误"));
        return;
    }
    if(PyTuple_Check(libsPy))
    {
        PyObject* libNameListPy = PyTuple_GetItem(libsPy,0);
        if(PyList_Check(libNameListPy))
        {
            int libscnt = PyList_Size(libNameListPy);
            for(int i = 0; i < libscnt; ++i){
                PyObject* oneLibnamePy = PyList_GetItem(libNameListPy,i);
                QString oneLibNameQString = PyString_AsString(oneLibnamePy);
                libNames.append(oneLibNameQString);
            }
        }
        PyObject* libContentListPy = PyTuple_GetItem(libsPy,1);
        if(PyList_Check(libContentListPy))
        {
            int libscnt = PyList_Size(libContentListPy);
            for(int i = 0; i < libscnt; ++i)
            {
                PyObject* onelibContentPy = PyList_GetItem(libContentListPy,i);
                QStringList oneLibContentQStringList;
                if(PyList_Check(onelibContentPy)){
                    int oneLibContentCnt = PyList_Size(onelibContentPy);
                    for(int j = 0; j < oneLibContentCnt; ++j){
                        PyObject* oneLibContentNamePy = PyList_GetItem(onelibContentPy,j);
                        QString oneLibContentNameQString = PyString_AsString(oneLibContentNamePy);
                        oneLibContentQStringList.append(oneLibContentNameQString);
                    }
                }
                libContents.append(oneLibContentQStringList);
            }
        }
        Py_DECREF(libsPy);
    }
}

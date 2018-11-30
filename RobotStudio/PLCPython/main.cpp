#include <Python.h>
#include <iostream>
#include <string>
using namespace std;

PyObject* PLCController = nullptr;

/**
 * @brief PLCProjectCtrl::getBlockResource  获取用户定义的所有Program
 * @return
 */
void getBlockResource()
{
    //QStringList progStrList;
    PyObject* progList = PyObject_CallMethod(PLCController,(char*)"getBlockResource",NULL);
	if(!progList){
        //QMessageBox::warning(nullptr,tr("PLC控制器方法错误"),tr("getBlockResource调用失败"));
        cout << "===================call getBlockResource error =============================" << endl; 
		return;
    }
    else
        cout << "getBlockResource module run success" << endl;
    if(PyList_Check(progList)){
        int len = PyList_Size(progList);
		cout << "list len = " << len <<endl;
        for(int i = 0; i < len; ++i){
            PyObject* progStr = PyList_GetItem(progList,i);
            /*
			QString tmp = QString(PyString_AsString(progStr));
            progStrList << tmp;
			*/
//			Py_DECREF(progStr);
        }
    }
    Py_DECREF(progList);
    //return progStrList;
}

/**
 * @brief PLCProjectCtrl::getEditedResourceInfos  获取XML文件资源中所有任务和实例信息
 * @param tagName
 */
void getEditedResourceInfos(string tagName)
{
//    taskInfo.clear();
//    instanceInfo.clear();
    std::string tmpName = tagName;
    cout << "============== getEditedResourceInfos 0============= " << tagName << endl;
    if(!PLCController)
    {
        cout << "Pointer to controller is Wrong" << endl;
        return;
    }else
        cout << "Pointer to controller is OK" << endl;
	//PyObject *args = Py_BuildValue("(s)", tagName.c_str());    
	//PyObject *keywords = PyDict_New();
	//PyDict_SetItemString(keywords, "somearg", Py_True);

	//PyObject* infoTuple = PyObject_Call(PyObject_GetAttrString(PLCController, (char*)"getEditedResourceInfos"), args, NULL);
	//Py_DECREF(args);
	//Py_DECREF(keywords);
    PyObject* infoTuple = PyObject_CallMethod(PLCController,(char*)"getEditedResourceInfos",(char*)"s",tmpName.c_str());
    if(!infoTuple){
        cout << "call getEditedResourceInfos error" << endl;
        return;
    }
    cout << "======================== hhhh === " << PyTuple_Size(infoTuple) << endl;
    /*
	if(PyTuple_Size(infoTuple) > 0)
    {
        PyObject* firstItem = PyTuple_GetItem(infoTuple,0);
        cout << "============== getEditedResourceInfos 2============= " << tagName << endl;
        if(PyList_Check(firstItem)){
            cout << "============== getEditedResourceInfos 3============= " << tagName << endl;
            int len = PyList_Size(firstItem);
            cout << "============== getEditedResourceInfos 4============= " << tagName << " " << len << endl;
            for(int i = 0; i < len; ++i)
            {
                PyObject* taskDict = PyList_GetItem(firstItem,i);
				/*
                QHash<QString,QString> oneTask;
                if(parsePyDict2QtHashStr(taskDict,oneTask))
                    taskInfo.append(oneTask);
                
				Py_XDECREF(taskDict);
            }
        }
        cout << "============== getEditedResourceInfos 6============= " << tagName << endl;
        Py_DECREF(firstItem);

        PyObject* secondItem = PyTuple_GetItem(infoTuple,1);
        if(PyList_Check(secondItem))
        {
            int len = PyList_Size(secondItem);
            for(int i = 0; i < len; ++i)
            {
                PyObject* instanceDict = PyList_GetItem(secondItem,i);
				/*
                QHash<QString,QString> oneInstance;
                if(parsePyDict2QtHashStr(instanceDict,oneInstance))
                    instanceInfo.append(oneInstance);
                
				Py_DECREF(instanceDict);
            }
        }
        Py_DECREF(secondItem);
    }
	*/
    Py_DECREF(infoTuple);
	/*
    qDebug() << taskInfo;
    qDebug() << instanceInfo;
    qDebug() << "\n\n";
	*/
}


int main()
{
	Py_Initialize();
	if(!Py_IsInitialized()){
		Py_Finalize();
		return -1;
	}
	PyObject* pModule = PyImport_ImportModule("ProjectControllerPy");
	if(!pModule)
	{
		std::cout << "can't open python file" << std::endl;
		Py_Finalize();
		return -1;
	}
	cout << "Module Initialize is OK" << endl;
	PyObject* ModuleDict = PyModule_GetDict(pModule);
	PyObject* controllerClass = PyDict_GetItemString(ModuleDict,"ProjectControllerPy");
	if(!controllerClass){
        cout << "can't find ProjectControllerPy class" << endl;
        return -1;
    }
    PLCController = PyInstance_New(controllerClass,NULL,NULL);
    if(!PLCController){
        cout << "can't create controller instance" << endl;
        return -1;
    }
	char* ProjPath = "E:/Qt/QTProjects/Learning/RobotStudio2.0/EstunTest";
	string tagName1("R::config::res1");
	string tagName2("R::config::res2");
	
	PyObject_CallMethod(PLCController,(char*)"LoadProject",(char*)"s",ProjPath);
	
	cout << "===================first============================================="<< endl;
	PyObject* confname = PyObject_CallMethod(PLCController,(char*)"getProjectCurrentConfigName",NULL);
    if(!confname)
		cout << "can't call function getProjectCurrentConfigName" << endl;
    else 
		cout << "confName = " << std::string(PyString_AsString(confname)) << endl;
	Py_DECREF(confname);
	
	cout << "==================== call getBlockResource ===========================" << endl;
	getBlockResource();
	cout << "==================== call ResourceEditInfos ==========================" << endl;
	getEditedResourceInfos(tagName1);
	
	cout << "=====================second===========================================" << endl;
	confname = PyObject_CallMethod(PLCController,(char*)"getProjectCurrentConfigName",NULL);
    if(!confname)
		cout << "can't call function getProjectCurrentConfigName" << endl;
    else 
		cout << "confName = " << std::string(PyString_AsString(confname)) << endl;
	Py_DECREF(confname);
	/*
	PyObject* ret1 = PyObject_CallMethod(PLCController,"printhello",NULL);
	if(ret1)
		cout << "=======================1 " << PyTuple_Size(ret1) << "======================" << endl;
	Py_DECREF(ret1);
	ret1 = PyObject_CallMethod(PLCController,"printhello",NULL);
	if(ret1)
		cout << "=======================2 " << PyTuple_Size(ret1) << "======================" << endl;
	Py_DECREF(ret1);
	ret1 = PyObject_CallMethod(PLCController,"printhello",NULL);
	if(ret1)
		cout << "=======================3 " << PyTuple_Size(ret1) << "======================" << endl;
	Py_DECREF(ret1);
	*/
	Py_Finalize();
	return 0;
}
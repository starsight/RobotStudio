# -*- coding: utf-8 -*-
from PLCControler import PLCControler
from PLCControler import _VariableInfos
import util.paths as paths
import os
import sys

import features
from util.paths import *
from datetime import datetime
from time import localtime
from util.misc import CheckPathPerm,GetClassImporter

#传入的是C字符串
def convertProjectPathFromC(ProjPath):
    print "In Python getString FromC_3.0"
    print ProjPath
    projectpath = os.path.abspath(AbsFile(ProjPath));
    return projectpath

def enumerate(sequence):
    enumeration = []
    nextIndex = 0
    for item in sequence:
        enumeration.append([nextIndex,item])
        newIndex = nextIndex+1
    return enumeration

#不知道为什么ProjectControllerPy不能继承PLCControler（C++调用python会失败）
class ProjectControllerPy:
    def __init__(self):
        self.controller = PLCControler()
#        PLCControler.__init__(self)
        self.BuildPath = None
        self.DefaultBuildPath = paths.AbsParentDir(__file__)
        self.RefreshConfNodesBlockLists()

    def printhello(self):
#        print("I am in class ProjectControllerPy")
        return ()

    def _setBuildPath(self,buildpath):
        print "======== In setBuildPath_3.0==================="
        self.BuildPath = convertProjectPathFromC(buildpath)
        print self.BuildPath
        self.DefaultBuildPath = None
        print self.BuildPath
        print "hhhhhhhhh==================================="

    def _getBuildPath(self):
        if self.BuildPath is not None:
            return self.BuildPath
        if self.DefaultBuildPath is not None:
            return self.DefaultBuildPath

    def _getIECgeneratedcodePath(self):
        return os.path.join(self._getBuildPath(),"generated_plc.st")

    def Generate_PLC_ST(self):
        _program,errors,warnings = self.controller.GenerateProgram(self._getIECgeneratedcodePath())
        if len(warnings) > 0 or len(errors) > 0:
            print("Generate ST code error\n")

    def AddProjectDefaultConfiguration(self, config_name="config", res_name="resource1"):
        self.controller.ProjectAddConfiguration(config_name)
#        self.controller.ProjectAddConfigurationResource(config_name, res_name)

#    新建工程时并判断当用户创建主程序时添加
    def SetProjectDefaultConfiguration(self):
        # Sets default task and instance for new project
        config = self.controller.Project.getconfiguration(self.GetProjectMainConfigurationName())
        resource = config.getresource()[0].getname()
        config = config.getname()
        resource_tagname = self.controller.ComputeConfigurationResourceName(config, resource)
        def_task = [
            {'Priority': '0', 'Single': '', 'Interval': 'T#20ms', 'Name': 'task0', 'Triggering': 'Cyclic'}]
        def_instance = [
            {'Task': def_task[0].get('Name'), 'Type': self.GetProjectPouNames()[0], 'Name': 'instance0'}]
        self.controller.SetEditedResourceInfos(resource_tagname, def_task, def_instance)

    def NewProject(self,ProjectPath,BuildPath=None):
        if not os.path.isdir(ProjectPath):
            print("It is not a folder. You can't use is for a new project")
            return False
        self.controller.CreateNewProject(
            {"rrojectName":"Unnamed",
            "productNaem":"Unnamed",
            "productVersion":"1",
            "companyName":"Unknown",
            "createDateTime":datetime(*localtime()[:6])})
        self.AddProjectDefaultConfiguration()

        # Change XSD into class members
#        self._AddParamsMembers()
#        self.Children = {}
        self.ProjectPath = ProjectPath
        self.SaveProject()

    def LoadProject(self,ProjPath):
        projectPath = convertProjectPathFromC(ProjPath)
        #add 2018/1/6
        self.ProjectPath = projectPath
        plc_file = os.path.join(projectPath,"plc.xml")
        print("=============================================")
        print("In Python LoadProject")
        print plc_file
        print("=============================================")
        if not os.path.isfile(plc_file):
            print("chosen folder doesn't contain a program.")
            return False
        error = self.controller.OpenXMLFile(plc_file)
        if error is not None:
            print("open XML file error");
            return False
        print self.controller.Project
        return True

    #只有调用了SaveProject才将Project写入XML文件
    def SaveProject(self):
        self.controller.SaveXMLFile(os.path.join(self.ProjectPath,'plc.xml'))

    def LoadLibraries(self):
        self.Libraries = []
        TypeStack = []
        for libname, clsname in features.libraries:
#            if self.BeremizRoot.Libraries is None or getattr(self.BeremizRoot.Libraries, "Enable_"+libname+"_Library"):
                Lib = GetClassImporter(clsname)()(self, libname, TypeStack)
                TypeStack.append(Lib.GetTypes())
                self.Libraries.append(Lib)

    def GetLibrariesTypes(self):
       self.LoadLibraries()
       return [lib.GetTypes() for lib in self.Libraries]

    # Update PLCOpenEditor ConfNode Block types from loaded confnodes
    def RefreshConfNodesBlockLists(self):
#       if getattr(self, "Children", None) is not None:
           self.controller.ClearConfNodeTypes()
           self.controller.AddConfNodeTypesList(self.GetLibrariesTypes())

    #和资源有关的获取设置函数
    def getProjectConfigNames(self):
        return self.controller.GetProjectConfigNames()

    def getProjectCurrentConfigName(self):
        config_names = self.getProjectConfigNames();
        if len(config_names) > 0:
            return config_names[0]
        return ""

    def getBlockResource(self):
        return self.controller.GetBlockResource()

    def getEditedResourceVariables(self,tagname):
        return self.controller.GetEditedResourceVariables(tagname)

    def getEditedResourceInfos(self,tagname):
        ret = self.controller.GetEditedResourceInfos(tagname);
        if ret is None:
            return ()
        return ret;

    def getEditedResourceElemnetVariables(self,configName,resourceName):
        values = self.controller.GetConfigurationResourceGlobalVars(configName,resourceName)
        print("================== in getEditedResourceVariables ==================")
        variables = []
        for oneValue in values:
            onevar = []
            onevar.append(getattr(oneValue,"Name"))
            onevar.append(getattr(oneValue,"Type"))
            onevar.append(getattr(oneValue,"Location"))
            onevar.append(getattr(oneValue,"InitialValue"))
            onevar.append(getattr(oneValue,"Option"))
            variables.append(onevar)
        return variables

    #添加新的资源时需要添加xml文件config节点的资源
    def projectAddConfigurationResource(self,config_name,resource_name):
        self.controller.ProjectAddConfigurationResource(config_name,resource_name)

    def projectRemoveConfigurationResource(self,config_name,resource_name):
        self.controller.ProjectRemoveConfigurationResource(config_name,resource_name)

    def setEditedResourceInfos(self,tagname,tasksInfo,instancesInfo):
        self.controller.SetEditedResourceInfos(tagname,tasksInfo,instancesInfo)

    def setEditedResourceElementVariables(self,configName,resourceName,varsList):
        values = []
        for oneVar in varsList:
            value = _VariableInfos("","","","","",True,"","INT",([],[]),0)
            setattr(value,"Name",oneVar[0])
            setattr(value,"Type",oneVar[1])
            setattr(value,"Location",oneVar[2])
            setattr(value,"InitialValue",oneVar[3])
            setattr(value,"Option",oneVar[4])
            setattr(value,"Class","Global")
            values.append(value)
        self.controller.SetConfigurationResourceGlobalVars(configName,resourceName,values)

    #获取整个工程信息
    def getProjectInfos(self):
        return self.controller.GetProjectInfos();

    #获取变量类型
    def getBaseTypes(self):
        return self.controller.GetBaseTypes()

    def getDataTypes(self):
        return self.controller.GetDataTypes(basetypes=False, confnodetypes=False)

    def getConfNodeDataTypes(self):
        return self.controller.GetConfNodeDataTypes()

    def getFunctionBlockTypes(self,tagName):
        return self.controller.GetFunctionBlockTypes(tagName)

    def getEditedElementInterfaceVars(self,tagName):
        values = self.controller.GetEditedElementInterfaceVars(tagName)
        variables = []
        for oneValue in values:
            onevar = []
            onevar.append(getattr(oneValue,"Name"))
            onevar.append(getattr(oneValue,"Class"))
            onevar.append(getattr(oneValue,"Type"))
            onevar.append(getattr(oneValue,"InitialValue"))
            onevar.append(getattr(oneValue,"Option"))
            variables.append(onevar)
        return variables

    def getEditedElementInterfaceReturnType(self,tagName):
        ret = self.controller.GetEditedElementInterfaceReturnType(tagName)
        if ret is None:
            return ""
        return ret

    # 将修改后的变量存入PLCcontroler中
    def setPouInterfaceReturnType(self,tagName,return_type):
        self.controller.SetPouInterfaceReturnType(tagName,return_type)

    def setPouInterfaceVars(self,name,varsList):
        values = []
        for oneVar in varsList:
            value = _VariableInfos("","","","","",True,"","INT",([],[]),0)
            setattr(value,"Name",oneVar[0])
            setattr(value,"Class",oneVar[1])
            setattr(value,"Type",oneVar[2])
            setattr(value,"InitialValue",oneVar[3])
            setattr(value,"Option",oneVar[4])
            values.append(value)
        self.controller.SetPouInterfaceVars(name,values)

    #POU的增删修改
    def projectAddPou(self,pou_name,pou_type,body_type="ST"):
        self.controller.ProjectAddPou(pou_name,pou_type,body_type)

    def projectRemovePou(self,pou_name):
        self.controller.ProjectRemovePou(pou_name)

    def changePouName(self,old_name,new_name):
        self.controller.ChangePouName(old_name,new_name)

    # 获取POU中编辑的程序
    def getEditedPouElementText(self,tagName):
        return self.controller.GetEditedElementText(tagName)

    # 将编辑框中的程序更新值PLCController
    def setEditedPouElementText(self,tagName,text):
        print("in Python setEditedPouElemetnText");
        print text
        encodedText = unicode(text, "utf-8")
        self.controller.SetEditedElementText(tagName,encodedText)

    # 获取库功能块名称和内容
    def getLibraryNodes(self):
        blocktypes = self.controller.GetBlockTypes();
        LibNameList = []
        LibContents = []

        if blocktypes is not None:
            for category in blocktypes:
                category_name = category["name"]
                LibNameList.append(category_name)
                tempList = []
                for blocktype in category["list"]:
                    tempList.append(blocktype["name"])
                LibContents.append(tempList)
        return (LibNameList,LibContents)

    def secondHello(self):
        print("I am python hello")
        return []

if __name__ == '__main__':
#    print type(__file__)
#    print AbsNeighbourFile(__file__,"plc.xml")
    controller = ProjectControllerPy()
    print("===================== get Library contents =======================")
    print controller.getLibraryNodes();
    ProjectDir = paths.AbsDir(__file__)
    print ProjectDir
#    print ProjectDir
#    filePath = os.path.join(ProjectDir,"plc.xml")
#    print filePath

    controller.LoadProject(ProjectDir)
#    controller.NewProject(ProjectDir)


#    print controller.getProjectCurrentConfigName()
    print("==================== getBlockResource ====================")
    print controller.getBlockResource()
    print("===================== getResoueceVariables ================")
    print controller.getEditedResourceVariables("R::config::res1")
#    print controller.getEditedResourceInfos("R::config::resource0")
    print("======================= first time ========================")
    print controller.getProjectCurrentConfigName()
    print controller.getEditedResourceInfos("R::config::res1")
    print("======================= second time =======================")
    print controller.getProjectCurrentConfigName()
    print("=================== Project Infos =========================")

    print("=================== SetResourcesInfos ======================")

#    controller.projectAddConfigurationResource("config","res2")
#    tagName = "R::config::res2"
#    def_task = [
#        {'Priority': '0', 'Single': '', 'Interval': 'T#50ms', 'Name': 'taskppp', 'Triggering': 'Cyclic'}]
#    def_instance = [
#        {'Task': def_task[0].get('Name'), 'Type': "progMe", 'Name': 'instanceppp'}]
#    controller.setEditedResourceInfos(tagName,def_task,def_instance)

#    controller.projectRemoveConfigurationResource("config","res1")
#    controller.SaveProject()

    print("=================== Get Base Type ==========================")
    print controller.getBaseTypes()
    print("=================== Get Data Type ==========================")
    print controller.getDataTypes()
    print("=================== Get Libs Types ==========================")
    print controller.getConfNodeDataTypes()
    print("=================== Get ProjectTypes ========================")
    print controller.getFunctionBlockTypes("P::fun0")
    print("=================== Get variables ===========================")
    print controller.getEditedElementInterfaceVars("P::fun")
    print("=================== set function return type ==================")
    controller.setPouInterfaceReturnType("fun0","REAL")
    print("================== get funciton return type ===================")
    print controller.getEditedElementInterfaceReturnType("P::fun0")

    print("=======================  setPouInterfaceVars ==================")
    valueList = [["var1","Input","REAL","8.0","Retain"]]
    controller.setPouInterfaceVars("fun0",valueList)

#    controller.projectAddPou("myPou","functionBlock")
#    controller.projectAddPou("myPou2","program")
#    controller.projectRemovePou("myPou2")

    print("==================== get Resource variables ====================")
    print controller.getEditedResourceElemnetVariables("config","resource0")

#    variableList = [["globalVar","BOOL","","True","Retain"]]
#    controller.setEditedResourceElementVariables("config","resource1",variableList)
#    controller.changePouName("myPou","myPou3")
#    controller.SaveProject()

    print("==================== get POU ===================================")
    print controller.getEditedPouElementText("P::program0")
    print controller.getEditedPouElementText("P::program1")
    print controller.getEditedPouElementText("P::fun6")

    print("===================== set POU content ==========================")
#    controller.setEditedPouElementText("P::program0","I am ychj；；\n She is cdj")
#    controller.SaveProject()




#    controller.printhello()
#    print controller.getProjectInfos();

#    controller._setBuildPath("E:/")
#    controller.printhello()

#    error = controller.OpenXMLFile(filePath)
#    if error is not None:
#        print("filePath is error\n")
#    if controller.Project is None:
#        print("Project is none\n")
#    controller.Generate_PLC_ST()

#include "plcresourceconf.h"
#include <QMessageBox>
#include <QDebug>

PLCResourceConf::PLCResourceConf(QString name,PLCProjectCtrl* ctrl,QWidget *parent)
    : QWidget(parent),tagName(name),controller(ctrl)
{
    tasksVarLabel = new QLabel(tr("任务间变量"));
    taskLabel = new QLabel(tr("Tasks"));
    instanceLabel = new QLabel(tr("Instances"));
    addVariableBtn = new QPushButton(tr("添加变量"));
    rmvVariableBtn = new QPushButton(tr("移除变量"));
    addTaskBtn = new QPushButton(tr("添加任务"));
    rmvTaskBtn = new QPushButton(tr("移除任务"));
    addInstanceBtn = new QPushButton(tr("添加实例"));
    rmvInstanceBtn = new QPushButton(tr("移除实例"));
    connect(addVariableBtn,SIGNAL(clicked(bool)),this,SLOT(addVariableSlot()));
    connect(rmvVariableBtn,SIGNAL(clicked(bool)),this,SLOT(removeVariableSlot()));
    connect(addTaskBtn,SIGNAL(clicked(bool)),this,SLOT(addTaskSlot()));
    connect(rmvTaskBtn,SIGNAL(clicked(bool)),this,SLOT(removeTaskSlot()));
    connect(addInstanceBtn,SIGNAL(clicked(bool)),this,SLOT(addInstanceSlot()));
    connect(rmvInstanceBtn,SIGNAL(clicked(bool)),this,SLOT(removeInstanceSlot()));

    setupTasksVarModel();
    setupTasksVarView();
    setupPLCTaskModel();
    setupPLCTaskView();
    setupPLCInstanceModel();
    setupPLCInstanceView();

    mainLayout = new QVBoxLayout(this);

    tasksVarLayout = new QVBoxLayout;
    varWidgetLayout = new QHBoxLayout;
    varWidgetLayout->addWidget(tasksVarLabel);
    varWidgetLayout->addStretch(1);
    varWidgetLayout->addWidget(addVariableBtn);
    varWidgetLayout->addWidget(rmvVariableBtn);
    tasksVarLayout->addLayout(varWidgetLayout);
    tasksVarLayout->addWidget(tasksVarView);

    PLCTaskLayout = new QVBoxLayout;
    taskWidgetLayout = new QHBoxLayout;
    taskWidgetLayout->addWidget(taskLabel);
    taskWidgetLayout->addStretch(1);
    taskWidgetLayout->addWidget(addTaskBtn);
    taskWidgetLayout->addWidget(rmvTaskBtn);
    PLCTaskLayout->addLayout(taskWidgetLayout);
    PLCTaskLayout->addWidget(PLCTaskView);

    PLCInstanceLayout = new QVBoxLayout;
    instanceWidgetLayout = new QHBoxLayout;
    instanceWidgetLayout->addWidget(instanceLabel);
    instanceWidgetLayout->addStretch(1);
    instanceWidgetLayout->addWidget(addInstanceBtn);
    instanceWidgetLayout->addWidget(rmvInstanceBtn);
    PLCInstanceLayout->addLayout(instanceWidgetLayout);
    PLCInstanceLayout->addWidget(PLCInstanceView);

    //设置变量视图代理
    QList<QVariant> variableOptions;
    variableOptions << "Constant" << "Retain" << "Non-Retain";
    varOptionDelegate = new ComboDelegate(variableOptions,this);
    varTypeDelegate = new VariableTypeDelegate(this);

    tasksVarView->setItemDelegateForColumn(1,varTypeDelegate);
    tasksVarView->setItemDelegateForColumn(4,varOptionDelegate);

    // 设置任务视图代理
    QList<QVariant> triggerType;
    triggerType << QString("Cyclic") << QString("Interrupt");
    taskTriggerDelegate = new ComboDelegate(triggerType,this);
    QList<QVariant> priortyNum;
    for(int i = 0; i < 100; ++i)    priortyNum << i;
    taskPriorityDelegate = new ComboDelegate(priortyNum,this);
    QList<QVariant> empty;
    taskSingleDelegate = new ComboDelegate(empty,this);
    taskIntervalDelegate = new TaskIntervalDelegate(this);
    PLCTaskView->setItemDelegateForColumn(1,taskTriggerDelegate);
    PLCTaskView->setItemDelegateForColumn(2,taskSingleDelegate);
    PLCTaskView->setItemDelegateForColumn(3,taskIntervalDelegate);
    PLCTaskView->setItemDelegateForColumn(4,taskPriorityDelegate);

    // 设置实例视图代理
    instanceTypeDelegate = new ComboDelegate(empty,this);
    instanceTaskDelegate = new ComboDelegate(empty,this);
    PLCInstanceView->setItemDelegateForColumn(1,instanceTypeDelegate);
    PLCInstanceView->setItemDelegateForColumn(2,instanceTaskDelegate);

    mainLayout->addLayout(tasksVarLayout);
    mainLayout->addLayout(PLCTaskLayout);
    mainLayout->addLayout(PLCInstanceLayout);

    // 创建新的资源实例时需要获取xml文件中的所有信息
    updateVariableDelegateTypes();
    updateVariables2VariableModel();

    controller->getEditedResourceInfos(tagName);
    QStringList variablesList = controller->getEditedResourceVariables(tagName);
    QStringList progList = controller->getBlockResource();
    updateResourcesModules(controller->tasksInfo,controller->instancesInfo,variablesList,progList);
}

void PLCResourceConf::setupTasksVarModel()
{
    tasksVarModel = new QStandardItemModel(0,5,this);
    tasksVarModel->setHeaderData(0,Qt::Horizontal,tr("变量名"));
    tasksVarModel->setHeaderData(1,Qt::Horizontal,tr("变量类型"));
    tasksVarModel->setHeaderData(2,Qt::Horizontal,tr("变量位置"));
    tasksVarModel->setHeaderData(3,Qt::Horizontal,tr("变量初始值"));
    tasksVarModel->setHeaderData(4,Qt::Horizontal,tr("变量附加属性"));
}

void PLCResourceConf::setupTasksVarView()
{
    tasksVarView = new QTableView;
    tasksVarView->setModel(tasksVarModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(tasksVarModel);
    tasksVarView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),tasksVarView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(onResourceVariableTableChanged()));
}

void PLCResourceConf::setupPLCTaskModel()
{
    PLCTaskModel = new QStandardItemModel(0,5,this);
    PLCTaskModel->setHeaderData(0,Qt::Horizontal,tr("任务名"));
    PLCTaskModel->setHeaderData(1,Qt::Horizontal,tr("任务工作方式"));
    PLCTaskModel->setHeaderData(2,Qt::Horizontal,tr("触发源"));
    PLCTaskModel->setHeaderData(3,Qt::Horizontal,tr("周期"));
    PLCTaskModel->setHeaderData(4,Qt::Horizontal,tr("任务优先级"));
}

void PLCResourceConf::setupPLCTaskView()
{
    PLCTaskView = new QTableView;
    PLCTaskView->setModel(PLCTaskModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(PLCTaskModel);
    PLCTaskView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),PLCTaskView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(updateViews()));
}

void PLCResourceConf::setupPLCInstanceModel()
{
    PLCInstanceModel = new QStandardItemModel(0,3,this);
    PLCInstanceModel->setHeaderData(0,Qt::Horizontal,tr("实例名"));
    PLCInstanceModel->setHeaderData(1,Qt::Horizontal,tr("实例类型"));
    PLCInstanceModel->setHeaderData(2,Qt::Horizontal,tr("任务名"));
}

void PLCResourceConf::setupPLCInstanceView()
{
    PLCInstanceView = new QTableView;
    PLCInstanceView->setModel(PLCInstanceModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(PLCInstanceModel);
    PLCInstanceView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),PLCInstanceView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

QString PLCResourceConf::getConfigName()
{
    return tagName.section("::",1,1);
}

QString PLCResourceConf::getResourceName()
{
    return tagName.section("::",2,2);
}


void PLCResourceConf::addDefaultTaskAndInstance()
{
    if(PLCTaskModel->rowCount() != 0 || PLCInstanceModel->rowCount() != 0)
        return;
    PLCTaskModel->insertRow(0,QModelIndex());
//    PLCTaskModel->setData(PLCTaskModel->index(0,0,QModelIndex()),tr("Task0"));
    PLCTaskModel->setData(PLCTaskModel->index(0,1,QModelIndex()),tr("cyclic"));
    PLCTaskModel->setData(PLCTaskModel->index(0,3,QModelIndex()),tr("T#8ms"));
    PLCTaskModel->setData(PLCTaskModel->index(0,4,QModelIndex()),80);

    PLCInstanceModel->insertRow(0,QModelIndex());
    PLCInstanceModel->setData(PLCInstanceModel->index(0,0,QModelIndex()),tr("Instance0"));
    PLCInstanceModel->setData(PLCInstanceModel->index(0,1,QModelIndex()),tr("Main"));
    PLCInstanceModel->setData(PLCInstanceModel->index(0,2,QModelIndex()),tr("task0"));
}


void PLCResourceConf::addVariableSlot()
{
    int row = tasksVarModel->rowCount();
    tasksVarModel->insertRow(row,QModelIndex());
    tasksVarModel->setData(tasksVarModel->index(row,0,QModelIndex()),tr("LocalVar0"));
    tasksVarModel->setData(tasksVarModel->index(row,1,QModelIndex()),tr("DINT"));
}

void PLCResourceConf::removeVariableSlot()
{
    int row = tasksVarView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
        return;
    }
    tasksVarModel->removeRow(row);
}

void PLCResourceConf::addTaskSlot()
{
    int row = PLCTaskModel->rowCount();
    PLCTaskModel->insertRow(row,QModelIndex());
//    PLCTaskModel->setData(PLCTaskModel->index(row,0,QModelIndex()),tr("Task0"));
    PLCTaskModel->setData(PLCTaskModel->index(row,1,QModelIndex()),tr("Cyclic"));
    PLCTaskModel->setData(PLCTaskModel->index(row,3,QModelIndex()),tr("T#8ms"));
    PLCTaskModel->setData(PLCTaskModel->index(row,4,QModelIndex()),80);
}

void PLCResourceConf::removeTaskSlot()
{
    int row = PLCTaskView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
        return;
    }
    PLCTaskModel->removeRow(row);
}


void PLCResourceConf::addInstanceSlot()
{
    int row = PLCInstanceModel->rowCount();
    PLCInstanceModel->insertRow(row,QModelIndex());
}

void PLCResourceConf::removeInstanceSlot()
{
    int row = PLCInstanceView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
        return;
    }
    PLCInstanceModel->removeRow(row);
}


void PLCResourceConf::updateViews()
{
    qDebug() << "========================== In updateViews Slot=======================";
    int col = PLCTaskView->currentIndex().column();
    qDebug() << "column = " << col;
    // 修改任务表中任务名需要更新实例中的任务名
    if(col == 0){
        int row = PLCTaskView->currentIndex().row();
        QString newName = PLCTaskModel->data(PLCTaskModel->index(row,0,QModelIndex())).toString();
        qDebug() << "name = " << newName;
        if(newName.isEmpty()) return;
        if(row < taskNameList.size()){
            for(int i = 0; i < taskNameList.length(); ++i){
                if(i != row && newName == taskNameList[i]){
                    PLCTaskModel->setData(PLCTaskModel->index(row,0,QModelIndex()),taskNameList[row]);
                    QMessageBox::warning(this,tr("任务名错误"),tr("已存在该任务名"));
                    return;
                }
            }
            QString oldName = taskNameList[row];
            if(oldName != newName){
                taskNameList[row] = newName;
                instanceTaskDelegate->delegateReplaceItem(oldName,newName);
            }
        }else
        {
            for(int i = 0; i < taskNameList.length(); ++i){
                if(newName == taskNameList[i]){
                    PLCTaskModel->setData(PLCTaskModel->index(row,0,QModelIndex()),tr(""));
                    QMessageBox::warning(this,tr("任务名错误"),tr("已存在该任务名"));
                    return;
                }
            }
            taskNameList.append(newName);
            instanceTaskDelegate->delegateInsertItem(newName);
        }
    }else if(col == 1)
    {

    }
}

/**
 * @brief PLCResourceConf::updateTaskModelSingleDelegate 当全局变量发生变化时需要更新任务代理
 */
void PLCResourceConf::updateTaskModelSingleDelegate()
{
    taskSingleDelegate->delegateClearItems();
    QStringList variableList = controller->getEditedResourceVariables(tagName);
    for(int i = 0; i < variableList.length(); ++i)
        taskSingleDelegate->delegateInsertItem(variableList[i]);
}

/**
 * @brief PLCResourceConf::updateInstanceTypeDelegate 当用户添加新的Program时需要更新实例类型代理
 */
void PLCResourceConf::updateInstanceTypeDelegate()
{
    instanceTypeDelegate->delegateClearItems();
    QStringList progList = controller->getBlockResource();
    for(int i = 0; i < progList.length(); ++i)
        instanceTypeDelegate->delegateInsertItem(progList[i]);
}

/**
 * @brief PLCResourceConf::updateResourcesModules  更新资源模型
 * @param taskInfo                          获取的任务信息
 * @param instanceInfo                      获取的实例信息
 */
void PLCResourceConf::updateResourcesModules(QList<QHash<QString,QString>>& tasksInfo,
        QList<QHash<QString,QString>>& instancesInfo, QStringList& variableList, QStringList& progList)
{
    for(int i = 0; i < tasksInfo.length(); ++i)
    {
        int row = PLCTaskModel->rowCount();
        PLCTaskModel->insertRow(row,QModelIndex());
        PLCTaskModel->setData(PLCTaskModel->index(row,0,QModelIndex()),tasksInfo[i]["Name"]);
        PLCTaskModel->setData(PLCTaskModel->index(row,1,QModelIndex()),tasksInfo[i]["Triggering"]);
        PLCTaskModel->setData(PLCTaskModel->index(row,2,QModelIndex()),tasksInfo[i]["Single"]);
        PLCTaskModel->setData(PLCTaskModel->index(row,3,QModelIndex()),tasksInfo[i]["Interval"]);
        PLCTaskModel->setData(PLCTaskModel->index(row,4,QModelIndex()),tasksInfo[i]["Priority"]);

        QString taskname = tasksInfo[i]["Name"];
        qDebug() << "========================== updateResourcesModules ============================ ";
        qDebug() << taskname;
        taskNameList.append(taskname);
        instanceTaskDelegate->delegateInsertItem(taskname);
    }

    for(int i = 0; i < instancesInfo.length(); ++i){
        int row = PLCInstanceModel->rowCount();
        PLCInstanceModel->insertRow(row,QModelIndex());
        PLCInstanceModel->setData(PLCInstanceModel->index(row,0,QModelIndex()),instancesInfo[i]["Name"]);
        PLCInstanceModel->setData(PLCInstanceModel->index(0,1,QModelIndex()),instancesInfo[i]["Type"]);
        PLCInstanceModel->setData(PLCInstanceModel->index(0,2,QModelIndex()),instancesInfo[i]["Task"]);
    }

    for(int i = 0; i < variableList.size(); ++i)
        taskSingleDelegate->delegateInsertItem(variableList[i]);
    for(int i = 0; i < progList.size(); ++i)
        instanceTypeDelegate->delegateInsertItem(progList[i]);
}

/**
 * @brief PLCResourceConf::updateResourcesProjectNode  将资源配置窗口中数据更新到Project中
 */
void PLCResourceConf::updateResourcesProjectNode()
{
    QList<QHash<QString,QString>> curTasksInfo, curInstancesInfo;
    for(int row = 0; row < PLCTaskModel->rowCount(); ++row){
        QHash<QString,QString> oneTaskInfo;
        oneTaskInfo["Name"] = PLCTaskModel->data(PLCTaskModel->index(row,0,QModelIndex())).toString();
        oneTaskInfo["Triggering"] = PLCTaskModel->data(PLCTaskModel->index(row,1,QModelIndex())).toString();
        oneTaskInfo["Single"] = PLCTaskModel->data(PLCTaskModel->index(row,2,QModelIndex())).toString();
        oneTaskInfo["Interval"] = PLCTaskModel->data(PLCTaskModel->index(row,3,QModelIndex())).toString();
        oneTaskInfo["Priority"] = PLCTaskModel->data(PLCTaskModel->index(row,4,QModelIndex())).toString();
        curTasksInfo.append(oneTaskInfo);
    }
    for(int row = 0; row < PLCInstanceModel->rowCount(); ++row){
        QHash<QString,QString> oneInstanceInfo;
        oneInstanceInfo["Name"] = PLCInstanceModel->data(PLCInstanceModel->index(row,0,QModelIndex())).toString();
        oneInstanceInfo["Type"] = PLCInstanceModel->data(PLCInstanceModel->index(row,1,QModelIndex())).toString();
        oneInstanceInfo["Task"] = PLCInstanceModel->data(PLCInstanceModel->index(row,2,QModelIndex())).toString();
        curInstancesInfo.append(oneInstanceInfo);
    }
    controller->setEditedResourceInfos(tagName,curTasksInfo,curInstancesInfo);
}

/**
 * @brief PLCResourceConf::updateVariableDelegateTypes
 */
void PLCResourceConf::updateVariableDelegateTypes()
{
    QStringList baseDataTypes;
    controller->getBaseTypes(baseDataTypes);
    QStringList userDataTypes;
    controller->getDataTypes(userDataTypes);
    QStringList libNameList;
    QList<QStringList> libTypeList;
    controller->getConfNodeDataTypes(libNameList,libTypeList);
    varTypeDelegate->addOneMenuList(tr("基本数据类型"),baseDataTypes);
    varTypeDelegate->addOneMenuList(tr("用户数据类型"),userDataTypes);
    for(int i = 0; i < libNameList.size(); ++i)
        varTypeDelegate->addOneMenuList(libNameList[i],libTypeList[i]);
    QStringList fbTypes;
    controller->getFunctionBlockTypes(tagName,fbTypes);
    varTypeDelegate->addOneMenuList(tr("功能块类型"),fbTypes);
}


/**
 * @brief PLCResourceConf::updateVariables2VariableModel 将xml文件中的资源变量信息更新到variable模型中
 */
void PLCResourceConf::updateVariables2VariableModel()
{
    QList<QStringList> varsInfo;
    controller->getEditedResourceElemnetVariables(getConfigName(),getResourceName(),varsInfo);
    qDebug() << "======================= updateVariables2VariableModel =======================";
    qDebug() << varsInfo;
    for(int i = 0; i < varsInfo.length(); ++i){
        int row = tasksVarModel->rowCount();
        tasksVarModel->insertRow(row,QModelIndex());
        for(int j = 0; j < 5; ++j)
            tasksVarModel->setData(tasksVarModel->index(row,j,QModelIndex()),varsInfo[i][j]);
    }
}

/**
 * @brief PLCResourceConf::onResourceVariableTableChanged 资源变量表变化槽函数
 */
void PLCResourceConf::onResourceVariableTableChanged()
{
    qDebug() << "on ResourceVariableTableChanged ";
    QList<QStringList> varsList;
    for(int i = 0; i < tasksVarModel->rowCount(); ++i)
    {
        QStringList oneVar;
        for(int j = 0; j < tasksVarModel->columnCount();++j)
            oneVar.append(tasksVarModel->data(tasksVarModel->index(i,j,QModelIndex())).toString());
        varsList.append(oneVar);
    }
    qDebug() << varsList;
    controller->setEditedResourceElementVariables(this->getConfigName(),this->getResourceName(),varsList);
    updateTaskModelSingleDelegate();
}

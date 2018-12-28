#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QStatusBar>
#include <QDebug>
#include <QApplication>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("RobotStudio"));
    resize(1400,700);
    createActions();
    createMenuBar();
    createToolBar();
    statusBar();
    createDockWidget();
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea,leftDockWidget,Qt::Horizontal);
    addDockWidget(Qt::BottomDockWidgetArea,bottomDockWidget,Qt::Horizontal);
    addDockWidget(Qt::RightDockWidgetArea,rightDockWidget,Qt::Horizontal);

    mainWidget = new QStackedWidget(this);
    textTabWidget = new QTabWidget(mainWidget);
    textTabWidget->setTabsClosable(true);
    PLCTabWidget = new QTabWidget(mainWidget);
    PLCTabWidget->setTabsClosable(true);
    mainWidget->addWidget(textTabWidget);
    mainWidget->addWidget(PLCTabWidget);
    connect(this,SIGNAL(currentTreeIndex(int)),mainWidget,SLOT(setCurrentIndex(int)));
    connect(this,SIGNAL(currentTabIndex(int)),textTabWidget,SLOT(setCurrentIndex(int)));
    connect(textTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTextTabSlot(int)));
    // Todo add PLCTabWidget signal slots
    connect(this,SIGNAL(currentPLCTabIndex(int)),PLCTabWidget,SLOT(setCurrentIndex(int)));
    connect(PLCTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closePLCTabSlot(int)));

    setCentralWidget(mainWidget);
    rbtConfig = nullptr;
    rsiConfig = nullptr;
    mainTextEditor = nullptr;
    stackIndex = -1;
    PLCprjCtl = nullptr;

    consoleOutput = new QTextEdit;
    bottomDockWidget->setWidget(consoleOutput);
    pPLCCompiler = new QProcess(this);
    connect(pPLCCompiler,SIGNAL(error(QProcess::ProcessError)),SLOT(compilerError(QProcess::ProcessError)));
    connect(pPLCCompiler,SIGNAL(readyReadStandardError()),this,SLOT(printCompilerErrors()));

//    PLCprjCtl = new PLCProjectCtrl(this);
//    qDebug() << "Python module load is OK";
//    connect(PLCprjCtl,SIGNAL(updateProjectTree(QStringList,QList<int>)),this,SLOT(onLoadPLCProjects(QStringList,QList<int>)));
//    createPLCMenus();
}

MainWindow::~MainWindow()
{
    delete PLCprjCtl;
}


/**
 * @brief MainWindow::contextMenuEvent  重载函数，使用事件的方式可能需要用到，事件可以被屏蔽
 * @param event
 */
/*
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QTreeWidgetItem* item = solutionTree->rootTree->currentItem();
    QTreeWidgetItem* parent = item->parent();
    QTreeWidgetItem* rootItem = solutionTree->rootTree->topLevelItem(0);
    QString itemName = item->text(0);
    QString itemPath = itemName;
    while(parent != rootItem){
        itemPath = parent->text(0) + "/" + itemPath;
        item = parent;
        parent = item->parent();

    }
    int index = parent->indexOfChild(item);
    if(index != 0)
    {
        itemPath = projectPath + "/" + itemPath;
        QFileInfo fileInfo(itemPath);
        if(fileInfo.isDir() && itemName == "PLC"){
            if(PLCMenu->isEmpty()){
                PLCMenu->addAction(addTypeAction);
                PLCMenu->addAction(addResourceAction);
                PLCMenu->addAction(addFunAction);
                PLCMenu->addAction(addFBAction);
                PLCMenu->addAction(addProgAction);
            }
            PLCMenu->exec(QCursor::pos());
        }
    }
}
*/

/**
 * @brief MainWindow::createActions 创建工程菜单栏工具栏的动作
 */
void MainWindow::createActions(){
    plcUnitAction = new QAction(QIcon(":/icon/if_new"),tr("&New PLC Unit"),this);
    plcUnitAction->setStatusTip(tr("Create a new plc unit"));
    connect(plcUnitAction,SIGNAL(triggered(bool)),this,SLOT(createNewPLCUnit()));

    openAction = new QAction(QIcon(":/icon/if_folder-open"),tr("&Open Project"),this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing project"));
    connect(openAction,SIGNAL(triggered(bool)),this,SLOT(openProject()));

    newAction = new QAction(QIcon(":/icon/if_new"),tr("&New Project"),this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new project"));
    connect(newAction,SIGNAL(triggered(bool)),this,SLOT(createNewProject()));

    closeAction = new QAction(tr("&Close Project"),this);
    closeAction->setShortcuts(QKeySequence::Close);
    closeAction->setStatusTip(tr("Close the project"));
    connect(closeAction,SIGNAL(triggered(bool)),this,SLOT(closeProject()));

    saveAction = new QAction(QIcon(":/icon/if_save"),tr("&Save"),this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save"));
    connect(saveAction,SIGNAL(triggered(bool)),this,SLOT(saveProject()));

    saveAsAction = new QAction(tr("Save &As"),this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save As"));

    quitAction = new QAction(QIcon(":/icon/if_quit"),tr("&Quit"),this);
    quitAction->setShortcuts(QKeySequence::Close);
    quitAction->setStatusTip((tr("Exit")));
    connect(quitAction,SIGNAL(triggered(bool)),this,SLOT(close()));

    undoAction = new QAction(QIcon(":/icon/if_undo"),tr("&Undo"),this);
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo"));

    redoAction = new QAction(QIcon(":/icon/if_redo"),tr("&Redo"),this);
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setStatusTip(tr("Redo"));

    cutAction = new QAction(QIcon(":/icon/if_cut"),tr("&Cut"),this);
    cutAction->setShortcuts(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut"));

    copyAction = new QAction(QIcon(":/icon/if_copy"),tr("&Copy"),this);
    copyAction->setShortcuts(QKeySequence::Cut);
    copyAction->setStatusTip(tr("Copy"));

    pasteAction = new QAction(QIcon(":/icon/if_paste"),tr("&Paste"),this);
    pasteAction->setShortcuts(QKeySequence::Cut);
    pasteAction->setStatusTip(tr("Paste"));

    generatePLCcodeAction = new QAction(QIcon(":/icon/build"),tr("&Build"),this);
    generatePLCcodeAction->setStatusTip(tr("产生PLC目标文件"));
    connect(generatePLCcodeAction,SIGNAL(triggered(bool)),this,SLOT(generatePLCCode()));

    downloadAction = new QAction(QIcon(":/icon/download"),tr("&Download"),this);
    downloadAction->setStatusTip(tr("下载到控制器"));

    connectAction = new QAction(QIcon(":/icon/connect"),tr("&Connect"),this);
    connectAction->setStatusTip(tr("连接控制器"));

    rebootAction = new QAction(QIcon(":/icon/reboot"),tr("&Reboot"),this);
    rebootAction->setStatusTip(tr("重启控制器"));

    refreshAction = new QAction(QIcon(":/icon/refresh"),tr("&refresh"),this);
    refreshAction->setStatusTip(tr("重新加载工程"));

}

/**
 * @brief MainWindow::createMenuBar 创建工程菜单栏
 */
void MainWindow::createMenuBar(){
    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(newAction);
    file->addAction(openAction);
    file->addAction(closeAction);
    file->addSeparator();
    file->addAction(saveAction);
    file->addAction(saveAsAction);
    file->addSeparator();
    file->addAction(quitAction);

    QMenu *edit = menuBar()->addMenu(tr("&Edit"));
    edit->addAction(undoAction);
    edit->addAction(redoAction);
    edit->addAction(cutAction);
    edit->addAction(copyAction);
    edit->addAction(pasteAction);

    QMenu *unit = menuBar()->addMenu(tr("&Unit"));
    unit->addAction(plcUnitAction);
}

/**
 * @brief MainWindow::createToolBar 创建工具条
 */
void MainWindow::createToolBar(){
    QToolBar *toolBar = addToolBar(tr("&File"));
    toolBar->addAction(openAction);
    toolBar->addAction(newAction);
    toolBar->addAction(saveAction);

    QToolBar *editBar = addToolBar(tr("&Edit"));
    editBar->addAction(undoAction);
    editBar->addAction(redoAction);
    editBar->addAction(generatePLCcodeAction);
    editBar->addAction(downloadAction);
    editBar->addAction(connectAction);
    editBar->addAction(refreshAction);
    editBar->addAction(rebootAction);
}

void MainWindow::createDockWidget(){
    leftDockWidget = new QDockWidget(tr("Solution Explorer"),this);
    bottomDockWidget = new QDockWidget(tr("Console"),this);
    rightDockWidget = new QDockWidget(tr("Library"),this);
    rightDockWidget->hide();
}

void MainWindow::createNewPLCUnit(){

}

/**
 * @brief MainWindow::openProject
 *        打开已有工程
 */
void MainWindow::openProject(){
    qDebug() << "============================ openProject =======================================";
    QString path =QFileDialog::getOpenFileName(this,tr("Open File"),"..",tr("Robot files(*.rbt)"));
    if(!path.isEmpty()){
        int index = path.lastIndexOf('/');
        projectPath = path.left(index);
        qDebug() << "********** " << projectPath;
        projectName = path.right(path.length()-index-1);
        projectName = projectName.left(projectName.lastIndexOf('.'));
        qDebug() << "========================== create Solution Tree ================================";
        solutionTree = new SolutionTree(projectPath,projectName,leftDockWidget);
        leftDockWidget->setWidget(solutionTree);
        connect(solutionTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));
        connect(solutionTree,SIGNAL(createNewPLCObj(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));
        connect(solutionTree,SIGNAL(PLCMenuTriggered(QAction*,QTreeWidgetItem*)),this,SLOT(onPLCMenuTrigged(QAction*,QTreeWidgetItem*)));
        /*
        fswatcher.addPath(projectPath);
        connect(&fswatcher,SIGNAL(directoryChanged(QString)),this,SLOT(updateSolutionTree(QString)));
        */
        qDebug() << "========================== solution Tree create success================================";
        qDebug() << projectPath + "/plc.xml";
        QFileInfo plcxmlFile(projectPath+"/plc.xml");
        if(!plcxmlFile.isFile()){
            QMessageBox::warning(this,tr("未找到plc.xml文件"),tr("未找到plc.xml文件，请新建工程"));
            return;
        }
        qDebug() << "========================== create PLCProjectController =================================";
        PLCprjCtl = new PLCProjectCtrl(this);
        qDebug() << "Python module load is OK";
        connect(PLCprjCtl,SIGNAL(updateProjectTree(QStringList,QList<int>)),this,SLOT(onLoadPLCProjects(QStringList,QList<int>)));

        qDebug() << "========================= craete PLCProjectController success ===========================";
        PLCprjCtl->LoadPLCProject(projectPath);
        PLCprjCtl->setProjectBuildPath(projectPath);
        PLCprjCtl->getProjectCurrentConfigName(); // for test
        QStringList strlist = PLCprjCtl->getBlockResource();
        qDebug() << "In mainwindow " << strlist;

        plcLibraryTree = new LibraryTreeWidget(PLCprjCtl,this);
        rightDockWidget->setWidget(plcLibraryTree);
        /*
        QStringList names;
        QList<QStringList> contents;
        PLCprjCtl->getLibraryNodes(names,contents);
        qDebug() << "==========***********************************************=================";
        qDebug() << names;
        qDebug() << contents;
        */
        // 创建ftp传输对话框
        ftpDlg = new FTPDialog(projectPath,this);
        connect(downloadAction,SIGNAL(triggered(bool)),ftpDlg,SLOT(showFTPDialog()));
    }
}

/**
 * @brief MainWindow::createNewProject
 *        新建工程
 */
void MainWindow::createNewProject()
{
    projectPath = QFileDialog::getSaveFileName(this,tr("新建一个工程"),"..");
    if(!projectPath.isEmpty()){
        int index = projectPath.lastIndexOf('/');
        projectName = projectPath.right(projectPath.length()-index-1);
        QDir projectDir(projectPath.left(index));
        projectDir.mkdir(projectName);
        QDir tempDir;
        QString currentPath = tempDir.currentPath();
        tempDir.setCurrent(projectPath);
        QFile* file = new QFile;
        file->setFileName(projectName+".rbt");
        if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate))
            QMessageBox::information(this,tr("文件创建失败"),tr("错误的工程名"));
        QDir(projectPath).mkdir("PLC");
        QDir(projectPath).mkdir("RC");
        tempDir.setCurrent(currentPath);
        solutionTree = new SolutionTree(projectPath,projectName,leftDockWidget);
        leftDockWidget->setWidget(solutionTree);
        connect(solutionTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));
        connect(solutionTree,SIGNAL(createNewPLCObj(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));  // 创建新PLC节点时刷新主窗口
        connect(solutionTree,SIGNAL(PLCMenuTriggered(QAction*,QTreeWidgetItem*)),this,SLOT(onPLCMenuTrigged(QAction*,QTreeWidgetItem*)));
        /*
        fswatcher.addPath(projectPath);
        connect(&fswatcher,SIGNAL(directoryChanged(QString)),this,SLOT(updateSolutionTree(QString)));
        */
        PLCprjCtl = new PLCProjectCtrl(this);
        qDebug() << "Python module load is OK";
        connect(PLCprjCtl,SIGNAL(updateProjectTree(QStringList,QList<int>)),this,SLOT(onLoadPLCProjects(QStringList,QList<int>)));
        PLCprjCtl->NewPLCProject(projectPath);
        plcLibraryTree = new LibraryTreeWidget(PLCprjCtl,this);
        rightDockWidget->setWidget(plcLibraryTree);
        // 创建ftp传输对话框
        ftpDlg = new FTPDialog(projectPath,this);
        connect(downloadAction,SIGNAL(triggered(bool)),ftpDlg,SLOT(showFTPDialog()));
    }

}

void MainWindow::saveProject()
{
    for(int i = 0; i < resourceList.size(); ++i){
        qDebug() << "====================== In Save Project =========================";
        PLCResourceConf* resWidget = resourceList[i];
        qDebug() << "resource name = " << resWidget->getResourceName();
        resWidget->updateResourcesProjectNode();
    }
    PLCprjCtl->SaveProject();
}

void MainWindow::closeProject()
{
    for(int i = 0; i < tabPLCObjList.length(); ++i)
        PLCTabWidget->removeTab(i);
    tabPLCObjList.clear();
    for(int i = 0; i < resourceList.length(); ++i)
    {
        PLCResourceConf* resWidget = resourceList[i];
        resWidget->setParent(nullptr);
        delete resWidget;
    }
    resourceList.clear();
    for(int i = 0; i < functionList.length(); ++i)
    {
        PLCPouConf* funWidget = functionList[i];
        funWidget->setParent(nullptr);
        delete funWidget;
    }
    functionList.clear();
    for(int i = 0; i < functionBlockList.length(); ++i)
    {
        PLCPouConf* fbWidget = functionBlockList[i];
        fbWidget->setParent(nullptr);
        delete fbWidget;
    }
    functionBlockList.clear();
    for(int i = 0; i < programList.length(); ++i)
    {
        PLCPouConf* progWidget = programList[i];
        progWidget->setParent(nullptr);
        delete progWidget;
    }
    programList.clear();
    delete solutionTree;
    delete PLCprjCtl;
    projectPath.clear();
    projectName.clear();
    stackIndex = -1;
    tabfileList.clear();
    tabfilechanged.clear();
}

void MainWindow::openFile(){
    QString path =QFileDialog::getOpenFileName(this,tr("Open File"),"..",tr("Robot files(*.rbt)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::warning(this,tr("Read file"),tr("can't open this file:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);
        mainTextEditor->setText(in.readAll());
        file.close();
    }
}

/**
 * @brief MainWindow::generatePLCCode 产生PLC目标代码
 */
void MainWindow::generatePLCCode()
{
    saveProject();
    PLCprjCtl->generatePLCSTCode();   //在保存文件时生成
    compileSuccess = true;
    QDir tempPath;
    QString currentPath = QApplication::applicationDirPath();  //tempPath.currentPath();
    tempPath.setCurrent(projectPath);
    consoleOutput->append(currentPath);
    QString compilerPath = currentPath + "/./iec_compiler.exe";
    pPLCCompiler->start(compilerPath,QStringList() << "generated_plc.st");
    if(pPLCCompiler->waitForFinished() && compileSuccess){
        QString targetGenPath = currentPath + "/./translator.exe";
        pPLCCompiler->start(targetGenPath,QStringList() << "code.txt");
        pPLCCompiler->waitForFinished();
        consoleOutput->setText(tr("generate exec.obj success!"));
    }
    tempPath.setCurrent(currentPath);
}


/**
 * @brief MainWindow::compilerError  外部进程出现错误
 * @param error
 */
void MainWindow::compilerError(QProcess::ProcessError error)
{
 switch(error)
 {
  case QProcess::FailedToStart:
    QMessageBox::information(0,"FailedToStart","FailedToStart");
    break;
  case QProcess::Crashed:
    QMessageBox::information(0,"Crashed","Crashed");
    break;
  case QProcess::Timedout:
    QMessageBox::information(0,"FailedToStart","FailedToStart");
    break;
  case QProcess::WriteError:
    QMessageBox::information(0,"Timedout","Timedout");
    break;
  case QProcess::ReadError:
    QMessageBox::information(0,"ReadError","ReadError");
    break;
  case QProcess::UnknownError:
    QMessageBox::information(0,"UnknownError","UnknownError");
    break;
  default:
    QMessageBox::information(0,"default","default");
    break;
 }
}

/**
 * @brief MainWindow::printCompilerErrors  更新控制台状态显示
 */
void MainWindow::printCompilerErrors()
{
    compileSuccess = false;
    QString output;
    output += pPLCCompiler->readAllStandardError();
    consoleOutput->setText(output);
}

/**
 * @brief MainWindow::showRobotConfigWindow
 * @param item
 */
void MainWindow::showRobotConfigWindow(QTreeWidgetItem* item,int /*column*/)
{
    QTreeWidgetItem* parent = item->parent();
    if(NULL == parent)
        return;
//    QTreeWidgetItem* rootItem = solutionTree->rootTree->topLevelItem(0);
    QTreeWidgetItem* rootItem = solutionTree->topLevelItem(0);
    QTreeWidgetItem* copyItem = item;
    QString itemName = item->text(0);
//    QString parentName = parent->text(0);
    int itemType = item->type();

    QString tempdir = item->text(0);
    while(rootItem != parent){
        tempdir = parent->text(0) + "/"+ tempdir;
        item = parent;
        parent = item->parent();
    }
    rightDockWidget->hide();
    if(itemType == -3){
        if(unit == nullptr)
            unit = new Unit(this);
        if(mainWidget->indexOf(unit) == -1)
            mainWidget->addWidget(unit);
         int index = mainWidget->indexOf(unit);
         if(index != stackIndex){
             stackIndex = index;
             emit currentTreeIndex(stackIndex);
         }
    }else if(itemType == -2){
        if(rsiConfig == nullptr)
            rsiConfig = new RSIconfig(projectPath,this);
        if(mainWidget->indexOf(rsiConfig) == -1)
            mainWidget->addWidget(rsiConfig);
        int index = mainWidget->indexOf(rsiConfig);
        if(index != stackIndex){
            stackIndex = index;
            emit currentTreeIndex(stackIndex);
        }
    }
    else if(itemType == -1){
        if(rbtConfig == nullptr)
            rbtConfig = new RobotConfig(projectPath,this);
        if(mainWidget->indexOf(rbtConfig) == -1)
            mainWidget->addWidget(rbtConfig);
        int index = mainWidget->indexOf(rbtConfig);
        // 当选择目录树需要切换堆叠控件时发送信号
        if(index != stackIndex){
            stackIndex = index;
            emit currentTreeIndex(stackIndex);
        }
    }else if(itemType == 0)
        QMessageBox::information(this,tr("Dircetory"),itemName);
    else if(itemType == 1)
    {
        tempdir = projectPath + "/" + tempdir;
        int index = mainWidget->indexOf(textTabWidget);
         // 当选择目录树需要切换堆叠控件时发送信号
        if(index != stackIndex){
            stackIndex = index;
            emit currentTreeIndex(stackIndex);
        }
        if(tabfileList.indexOf(tempdir)== -1){
            tabfileList << tempdir;
            tabfilechanged << false;
            QTextEdit* editor = new QTextEdit(textTabWidget);
            editor->setUndoRedoEnabled(true);
            QFile file(tempdir);
            if(file.open(QIODevice::ReadWrite))
            {
                QTextStream stream(&file);
                while(!stream.atEnd())
                    editor->append(stream.readLine());
            }
            textTabWidget->addTab(editor,itemName);
            connect(editor,SIGNAL(textChanged()),this,SLOT(updateTextTabStatus()));
        }
        index = tabfileList.indexOf(tempdir);
        emit currentTabIndex(index);
    }else if(itemType > 1)
    {   rightDockWidget->show();
        int index = mainWidget->indexOf(PLCTabWidget);
         // 当选择目录树需要切换堆叠控件时发送信号
        if(index != stackIndex){
            stackIndex = index;
            emit currentTreeIndex(stackIndex);
        }
//        QString tmpName = itemName + parentName;
        QString tagName;
        if(itemType == 6){
            QList<PLCResourceConf*>::iterator it = resourceList.begin();
            for(; it != resourceList.end(); ++it)
                if((*it)->getResourceName() == itemName)
                    break;
            PLCResourceConf* resWidget = nullptr;
            tagName = copyItem->data(0,Qt::UserRole).toString();
            qDebug() << "从工程树获取的资源名："  << tagName;
            if(it == resourceList.end()){
                resWidget = new PLCResourceConf(tagName,PLCprjCtl,PLCTabWidget);
                resourceList.append(resWidget);
                // 根据xml文件中的内容更新该资源的配置
                /*
                PLCprjCtl->getEditedResourceInfos(tagName);
                QStringList variablesList = PLCprjCtl->getEditedResourceVariables(tagName);
                QStringList progList = PLCprjCtl->getBlockResource();
                resWidget->updateResourcesModules(PLCprjCtl->tasksInfo,PLCprjCtl->instancesInfo,variablesList,progList);
                */
            }
            else
                resWidget = *it;
            if(tabPLCObjList.indexOf(tagName) == -1){
                tabPLCObjList << tagName;
                PLCTabWidget->addTab(resWidget,itemName);
            }
        }else if(itemType == 3)
        {
            qDebug() << "添加函数窗口";
            QList<PLCPouConf*>::iterator it = functionList.begin();
            for(; it != functionList.end(); ++it)
                if((*it)->getPouName() == itemName)
                    break;
            PLCPouConf* funWidget = nullptr;
            tagName = copyItem->data(0,Qt::UserRole).toString();
            if(it == functionList.end()){
                qDebug() << "创建函数窗口";
                qDebug() << "从工程树获取的函数名："  << tagName;
                funWidget = new PLCPouConf(tagName,"FUN",PLCprjCtl,PLCTabWidget);
                functionList.append(funWidget);
            }
            else
                funWidget = *it;
            if(tabPLCObjList.indexOf(tagName) == -1){
                tabPLCObjList << tagName;
                PLCTabWidget->addTab(funWidget,itemName);
            }
        }else if(itemType == 4)
        {
            qDebug() << "添加功能块窗口";
            QList<PLCPouConf*>::iterator it = functionBlockList.begin();
            for(; it != functionBlockList.end(); ++it)
                if((*it)->getPouName() == itemName)
                    break;
            PLCPouConf* functionblockWidget = nullptr;
            tagName = copyItem->data(0,Qt::UserRole).toString();
            if(it == functionBlockList.end()){
                qDebug() << "创建功能块窗口";
                qDebug() << "从工程树获取的功能块名："  << tagName;
                functionblockWidget = new PLCPouConf(tagName,"FB",PLCprjCtl,PLCTabWidget);
                functionBlockList.append(functionblockWidget);
            }else
                functionblockWidget = *it;
            if(tabPLCObjList.indexOf(tagName) == -1){
                tabPLCObjList << tagName;
                PLCTabWidget->addTab(functionblockWidget,itemName);
            }
        }
        else if(itemType == 5)
        {
            qDebug() << "添加程序窗口";
            QList<PLCPouConf*>::iterator it = programList.begin();
            for(; it != programList.end(); ++it)
                if((*it)->getPouName() == itemName)
                    break;
            PLCPouConf* programWidget = nullptr;
            tagName = copyItem->data(0,Qt::UserRole).toString();
            if(it == programList.end()){
                qDebug() << "创建程序窗口";
                qDebug() << "从工程树获取的程序名："  << tagName;
                programWidget = new PLCPouConf(tagName,"PROG",PLCprjCtl,PLCTabWidget);
                programList.append(programWidget);
            }else
                programWidget = *it;
            if(tabPLCObjList.indexOf(tagName) == -1){
                tabPLCObjList << tagName;
                PLCTabWidget->addTab(programWidget,itemName);
            }
        }
        index = tabPLCObjList.indexOf(tagName);
//        qDebug() << "newindex = " << index;
        emit currentPLCTabIndex(index);
//        qDebug() << "=========================== mainwindow 3========================" << tmpName;
    }
}

/**
 * @brief MainWindow::updateSolutionTree  当目录发生变化时更新工程目录树
 */
void MainWindow::updateSolutionTree(QString)
{
    disconnect(solutionTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));
    delete solutionTree;
    solutionTree = new SolutionTree(projectPath,projectName,leftDockWidget);
    leftDockWidget->setWidget(solutionTree);
    connect(solutionTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));
    connect(solutionTree,SIGNAL(createNewPLCObj(QTreeWidgetItem*,int)),this,SLOT(showRobotConfigWindow(QTreeWidgetItem*,int)));
    connect(solutionTree,SIGNAL(PLCMenuTriggered(QAction*,QTreeWidgetItem*)),this,SLOT(onPLCMenuTrigged(QAction*,QTreeWidgetItem*)));
}

/**
 * @brief MainWindow::closeTextTabSlot 关闭标签槽
 * @param index
 */
void MainWindow::closeTextTabSlot(int index)
{
//    QStringList::iterator it = tabfileList.begin()+index;
    QWidget* tmpWidget = textTabWidget->widget(index);
    if(tabfilechanged.at(index)){
        switch(QMessageBox::warning(this,tr("warning"),tr("close without saving?"),
              QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel,QMessageBox::Save))
        {
        case QMessageBox::Save:
        {
            QFile file(tabfileList.at(index));
            if(file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
                QTextStream stream(&file);
                QTextEdit* editor = dynamic_cast<QTextEdit*>(tmpWidget);
                stream << editor->toPlainText();
            }
            break;
        }
        case QMessageBox::Cancel:
            return;
        case QMessageBox::Discard:
        default:
            break;
        }
    }
    tabfileList.removeAt(index);
    tabfilechanged.removeAt(index);
    textTabWidget->removeTab(index);
    tmpWidget->setParent(NULL);
    delete tmpWidget;
}


void MainWindow::closePLCTabSlot(int index)
{
    PLCTabWidget->removeTab(index);
    tabPLCObjList.removeAt(index);
}

/**
 * @brief MainWindow::updateTextTabStatus  更新标签页眉
 */
void MainWindow::updateTextTabStatus()
{
    int index = textTabWidget->currentIndex();
    QString currentLabel = textTabWidget->tabText(index);
    if(tabfilechanged.at(index) == false) textTabWidget->setTabText(index,currentLabel+"*");
    tabfilechanged[index] = true;
}

/**
 * @brief MainWindow::onPLCMenuTrigged 右键PLC目录显示菜单的槽集合
 * @param action
 */
void MainWindow::onPLCMenuTrigged(QAction* action,QTreeWidgetItem* item)
{
    bool ok;
    if(action == solutionTree->addTypeAction){
        QString text = QInputDialog::getText(this,tr("添加类型"),tr("类型名："),QLineEdit::Normal,QString(""),&ok);
        if(ok) solutionTree->insertPLCObj(item,text,2);
    }else if(action == solutionTree->addResourceAction){
        QString text = QInputDialog::getText(this,tr("添加资源"),tr("资源名："),QLineEdit::Normal,QString(""),&ok);
        QList<PLCResourceConf*>::iterator it = resourceList.begin();
        for(; it != resourceList.end(); ++it)
            if((*it)->getResourceName() == text){
                QMessageBox::warning(this,tr("添加资源错误"),tr("请输入正确的资源名"));
                return;
            }
        if(ok) {
            // 以 R::confname::resourcename 的形式保存
            QString configName = PLCprjCtl->getProjectCurrentConfigName();
            QString resourceName = text;
            QString prefix = "R::"+configName+"::";
            solutionTree->insertPLCObj(item,resourceName,6,prefix);
            PLCprjCtl->projectAddConfigurationResource(configName,resourceName);
        }
    }else if(action == solutionTree->addFunAction){
        QString text = QInputDialog::getText(this,tr("添加函数"),tr("函数名："),QLineEdit::Normal,QString(""),&ok);
        QList<PLCPouConf*>::iterator it = functionList.begin();
        for(; it != functionList.end(); ++it)
            if((*it)->getPouName() == text){
                QMessageBox::warning(this,tr("添加函数错误"),tr("请输入正确的函数名"));
                return;
            }
        if(ok)
        {
            solutionTree->insertPLCObj(item,text,3,"P::");
            PLCprjCtl->projectAddPou(text,"function");
        }
    }else if(action == solutionTree->addFBAction){
        QString text = QInputDialog::getText(this,tr("添加功能块"),tr("功能块名："),QLineEdit::Normal,QString(""),&ok);
        QList<PLCPouConf*>::iterator it = functionBlockList.begin();
        for(; it != functionBlockList.end();++it)
            if((*it)->getPouName() == text){
                QMessageBox::warning(this,tr("添加功能块错误"),tr("请输入正确的功能块名"));
                return;
            }
        if(ok)
        {
            solutionTree->insertPLCObj(item,text,4,"P::");
            PLCprjCtl->projectAddPou(text,"functionBlock");
        }
    }else if(action == solutionTree->addProgAction){
        QString text = QInputDialog::getText(this,tr("添加程序"),tr("程序名："),QLineEdit::Normal,QString(""),&ok);
        QList<PLCPouConf*>::iterator it = programList.begin();
        for(; it != programList.end(); ++it){
            if((*it)->getPouName() == text){
                QMessageBox::warning(this,tr("添加程序错误"),tr("请输入正确的程序名"));
                return;
            }
        }
        if(ok)
        {
            solutionTree->insertPLCObj(item,text,5,"P::");
            PLCprjCtl->projectAddPou(text,"program");
            for(int i = 0; i < resourceList.length();++i)
                resourceList[i]->updateInstanceTypeDelegate();
        }
    }else if(action == solutionTree->removeAction){
        int itemType = item->type();
        QString itemName = item->text(0);
        QString fullName = item->data(0,Qt::UserRole).toString();
        int index = tabPLCObjList.indexOf(fullName);
        tabPLCObjList.removeAt(index);
        switch(itemType)
        {   // 函数
            case 3:
            {
                index = 0;
                for(;index < functionList.size(); ++index)
                    if(functionList[index]->getPouName() == itemName)
                        break;
                PLCPouConf* functionWidget = functionList[index];
                functionList.removeAt(index);
                PLCprjCtl->projectRemovePou(itemName);
                functionWidget->setParent(nullptr);
                delete functionWidget;
                break;
            }
            // 功能块
            case 4:
            {
                index = 0;
                for(; index < functionBlockList.size(); ++index)
                    if(functionBlockList[index]->getPouName() == itemName)
                        break;
                PLCPouConf* functionBlockWidget = functionBlockList[index];
                functionBlockList.removeAt(index);
                PLCprjCtl->projectRemovePou(itemName);
                functionBlockWidget->setParent(nullptr);
                delete functionBlockWidget;
                break;
            }
            // 程序
            case 5:
            {
                index = 0;
                for(; index < programList.size(); ++index)
                    if(programList[index]->getPouName() == itemName)
                        break;
                PLCPouConf* programWidget = programList[index];
                programList.removeAt(index);
                PLCprjCtl->projectRemovePou(itemName);
                for(int i = 0; i < resourceList.length();++i)
                    resourceList[i]->updateInstanceTypeDelegate();
                programWidget->setParent(nullptr);
                delete programWidget;
                break;
            }
            // 资源
            case 6:
            {
                index = 0;
                for(;index < resourceList.size();++index)
                    if(resourceList[index]->getResourceName() == itemName)
                        break;
                PLCResourceConf* resWidget = resourceList[index];
                resourceList.removeAt(index);
                PLCprjCtl->projectRemoveConfigurationResource(resWidget->getConfigName(),resWidget->getResourceName());
                resWidget->setParent(nullptr);
                delete resWidget;
                break;
            }
            default:
                break;
        }
        solutionTree->deletePLCObj(item);
    }else if(action == solutionTree->renameAction){
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        solutionTree->renameItem(item);
        // 暂时只能修改solutiontree中item名，不能修改xml文件中结点名称
    }
}

/**
 * @brief MainWindow::onLoadPLCProjects  加载文件槽函数
 * @param treeItemNameList          树形条目名称
 * @param treeItemTypeList          类型
 */
void MainWindow::onLoadPLCProjects(QStringList treeItemNameList, QList<int> treeItemTypeList)
{
    QTreeWidgetItem* PLCItem = solutionTree->getPLCItem();
    if(PLCItem == nullptr) return;
    for(int i = 0; i < treeItemNameList.length(); ++i)
    {
        QString itemName = treeItemNameList[i];
        switch(treeItemTypeList[i])
        {
        case ITEM_DATATYPES:
            solutionTree->insertPLCObj(PLCItem,itemName,2);
            break;
        case ITEM_FUNCTION:
            solutionTree->insertPLCObj(PLCItem,itemName,3,"P::");
            break;
        case ITEM_FUNCITONBLOCK:
            solutionTree->insertPLCObj(PLCItem,itemName,4,"P::");
            break;
        case ITEM_PROGRAM:
            solutionTree->insertPLCObj(PLCItem,itemName,5,"P::");
            break;
        case ITEM_RESOURCES:
            {
            // 以 R::confname::resourcename 的形式保存
            QString configname = PLCprjCtl->getProjectCurrentConfigName();
            QString prefix = "R::"+configname+"::";
            solutionTree->insertPLCObj(PLCItem,itemName,6,prefix);
            break;
            }
        }
    }
}

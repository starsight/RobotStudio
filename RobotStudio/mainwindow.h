#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QDockWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QDirModel>
#include <QFileSystemWatcher>
#include <QStackedWidget>
#include <QTabWidget>
#include "solutiontree.h"
#include "robotconfig.h"
#include "plcresourceconf.h"
#include "plcpouconf.h"
#include "ftptransfer.h"
#include "rsiconfig.h"

#include "plcprojectctrl.h"
#include "plclibrarytreewidget.h"

#include <QProcess>   // 调用PLC编译器 及 字节码生成器

#include "unit.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
//    void contextMenuEvent(QContextMenuEvent *event);
private:
    void createActions();
    void createMenuBar();
    void createToolBar();
    void createDockWidget();
    void createTreeWidget();
    void openFile();

    void createPLCMenus();
signals:
    void currentTreeIndex(int);
    void currentTabIndex(int);
    void currentPLCTabIndex(int);
private slots:
    void openProject();
    void createNewProject();
    void saveProject();
    void closeProject();
    void showRobotConfigWindow(QTreeWidgetItem*,int);
    void updateSolutionTree(QString);
    void closeTextTabSlot(int);
    void updateTextTabStatus();

    void closePLCTabSlot(int index);
    void onPLCMenuTrigged(QAction*,QTreeWidgetItem*);
    void onLoadPLCProjects(QStringList treeItemNameList, QList<int> treeItemTypeList);

    void generatePLCCode();
    void compilerError(QProcess::ProcessError error);
    void printCompilerErrors();
//    void showAdditionMenu(QTreeWidgetItem*,int);

    void createNewPLCUnit();
private:
    QAction* openAction;
    QAction* newAction;
    QAction* closeAction;

    QAction* quitAction;
    QAction* saveAction;
    QAction* saveAsAction;

    QAction* undoAction;
    QAction* redoAction;
    QAction* cutAction;
    QAction* copyAction;
    QAction* pasteAction;

    QAction* generatePLCcodeAction;
    QAction* downloadAction;
    QAction* helpAction;

    QAction* connectAction;
    QAction* rebootAction;
    QAction* refreshAction;

    QAction* plcUnitAction;

    QGridLayout* mainLayout;
//    QWidget* mainWidget;
    QStackedWidget* mainWidget;
    QTabWidget* textTabWidget;
    QTabWidget* PLCTabWidget;
    QDockWidget* leftDockWidget;
    QDockWidget* bottomDockWidget;
    QDockWidget* rightDockWidget;

    SolutionTree* solutionTree;
    RobotConfig* rbtConfig;
    RSIconfig* rsiConfig;

    Unit* unit;

    QTextEdit* mainTextEditor;
    LibraryTreeWidget* plcLibraryTree;

    FTPDialog* ftpDlg;

    QFileSystemWatcher fswatcher;

private:
    QString projectPath;
    QString projectName;
    int stackIndex;
    QStringList tabfileList;
    QList<bool> tabfilechanged;

    QStringList tabPLCObjList;               // 存放所有PLC窗口的全名
    QList<PLCResourceConf*> resourceList;    // 存放所有资源窗口指针
    QList<PLCPouConf*> functionList;        // 存放所有函数窗口指针
    QList<PLCPouConf*> functionBlockList;   // 存放所有功能块窗口指针
    QList<PLCPouConf*> programList;         // 存放所有程序窗口指针

private:
    PLCProjectCtrl* PLCprjCtl;
    QProcess* pPLCCompiler;
    bool compileSuccess;
    QTextEdit* consoleOutput;
};

#endif // MAINWINDOW_H

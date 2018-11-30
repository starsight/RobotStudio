#ifndef PLCRESOURCECONF_H
#define PLCRESOURCECONF_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "plcprojectctrl.h"
#include "combodelegate.h"
#include "taskintervaldelegate.h"
#include "plcpouconf.h"

class PLCResourceConf : public QWidget
{
    Q_OBJECT
public:
    PLCResourceConf(QString name,PLCProjectCtrl* ctrl,QWidget *parent = nullptr);
    void setupTasksVarModel();
    void setupTasksVarView();
    void setupPLCTaskModel();
    void setupPLCTaskView();
    void setupPLCInstanceModel();
    void setupPLCInstanceView();
    QString getResourceName();
    QString getConfigName();
    void addDefaultTaskAndInstance();
    void updateTaskModelSingleDelegate();
    void updateInstanceTypeDelegate();
    void updateResourcesModules(QList<QHash<QString,QString>>&, QList<QHash<QString,QString>>&,QStringList&,QStringList&);
    void updateResourcesProjectNode();
    void updateVariableDelegateTypes();
    void updateVariables2VariableModel();
signals:

public slots:
private slots:
    void addVariableSlot();
    void removeVariableSlot();
    void addTaskSlot();
    void removeTaskSlot();
    void addInstanceSlot();
    void removeInstanceSlot();

    void updateViews();
    void onResourceVariableTableChanged();
private:
    QLabel* taskLabel, *instanceLabel, *tasksVarLabel;
    QStandardItemModel* tasksVarModel;
    QStandardItemModel* PLCTaskModel;
    QStandardItemModel* PLCInstanceModel;
    QTableView* tasksVarView;
    QTableView* PLCTaskView;
    QTableView* PLCInstanceView;
    QPushButton* addTaskBtn, *addInstanceBtn, *rmvTaskBtn, *rmvInstanceBtn, *addVariableBtn, *rmvVariableBtn;

    QVBoxLayout* mainLayout, *PLCTaskLayout, *PLCInstanceLayout, *tasksVarLayout;
    QHBoxLayout* taskWidgetLayout, *instanceWidgetLayout, *varWidgetLayout;

    ComboDelegate* varOptionDelegate;
    VariableTypeDelegate* varTypeDelegate;

    ComboDelegate* taskTriggerDelegate, *taskSingleDelegate, *taskPriorityDelegate;
    ComboDelegate* instanceTypeDelegate, *instanceTaskDelegate;
    TaskIntervalDelegate* taskIntervalDelegate;
private:
    QString tagName;
    PLCProjectCtrl* controller;
    QList<QString> taskNameList;
};

#endif // PLCRESOURCECONF_H

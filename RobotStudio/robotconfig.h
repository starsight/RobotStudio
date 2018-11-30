#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QTableView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QTableWidget>
//#include <unordered_set>
#include "xmluntity.h"
#include "addiodlg.h"

class RobotConfig : public QWidget
{
    Q_OBJECT
    enum ROBOT_TYPE{
        NORMAL_TYPE = 0,
        OTHER
    };
public:
    explicit RobotConfig(QString path,QWidget *parent = nullptr);

signals:

public slots:
private:
    // 主配置区控件
    QLabel* rbtPlatformLabel, *bitOrderLabel, *bitCountLabel;
    QComboBox* rbtPlatformComboBox, *bitOrderComboBox, *bitCountComboBox;
    QGroupBox* rbtGroupBox;
    QGridLayout* mainLayout;
    QHBoxLayout* topLayout, *rbtSectionTopLayout, *rbtSectionLayout;
    QVBoxLayout* rbtSectionLeftLayout;
    QTabWidget* configTabWidget;
    QWidget* rbtConfWidget, *devConfWidget;

private:
    // 机器人参数配置区控件
    QLabel* rbtNameLabel, *rbtTypeLabel,*rbtGraphLabel;
    QLineEdit* rbtNameLineEdit;
    QComboBox* rbtTypeCombobox;
    QPushButton* addAxisBtn, *removeAxisBtn, *generateConfFileBtn;
    QTableView* rbtDHParTableView, *rbtLimParTableView;
    QStandardItemModel* rbtDHModel, *rbtLimModel;
    QImage* img;

private:
    // IO及伺服配置区控件
    QLabel* devIntervalLabel, *devBusTypeLabel, *digitLabel, *analogLabel;
    QLineEdit* devIntervalLineEdit;
    QComboBox* devBusTypeComboBox;
    QGroupBox* IOGroupBox, *servoGroupBox;
    QPushButton* addIOModuleBtn, *removeDigitIOBtn, *removeAnalogIOBnt;
    QTableView* digitIOTableView, *analogIOTableView, *servoTableView;
    QStandardItemModel* digitIOModel, *analogIOModel, *servoModel;
    AddIODlg* addDialog;
    QHBoxLayout* devTopLayout, *IOLayout,*servoLayout;
    QVBoxLayout* devMainLayout, *IOLeftLayout, *IOMidLayout,*IORightLayout;
//    QGridLayout* IOLayout, *IORightLayout;
private:
    void setupRobotConfigModel();
    void setupRobotConfigView();
    void setupDeviceConfigModel();
    void setupDeviceConfigView();
    bool getAllWidgetsData();
private slots:
    void addAxisToModel();
    void removeAxisFromModel();
    void generateXmlFile();
    void generateIOXmlNode();
    void generateServoXmlNode();
    void generateRobotXmlNode();
    void switchRobotModelGraph(int);
//    void getRobotName(QString);

    void addIOToModle(QString moduleName, QString iotype, QString iodir,int portcnt);
    void removeDigitIOFromModel();
    void removeAnalogIOFromModel();
private:
//    std::unordered_set<QString> axisNameSet;
    QString projectPath;
    QStringList axisNameList;
    QStringList limHeadList;
    QStringList servoHeadList;
    XmlUntity* xml;
    // 配置文件中所需属性数据
    QString platformName, bitCountStr, bitOrderStr, busIntervalStr,busTypeStr;
    int IOModuleCount;
    QString rbtName;
    ROBOT_TYPE rbtType;
    int axisCount;

private:
    int digitIOCount,analogIOCount,digitIOInGrpCnt,digitIOOutGrpCnt,analogIOInGrpCnt,analogIOOutGrpCnt,servoAxisCnt;
};

#endif // ROBOTCONFIG_H

#ifndef RSICONFIG_H
#define RSICONFIG_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QTabWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QSpinBox>
#include <QDialog>
#include "xmluntity.h"
#include "combodelegate.h"

class RSIVariableWidget;
class RSIFunctionBlockWidget;
class RSICommunicationWidget;

class RSIconfig : public QWidget
{
    Q_OBJECT
public:
    explicit RSIconfig(const QString& path, QWidget *parent = nullptr);
private:
    void addXMLTextToElement(QDomElement& element, const QString& name, const QString& value);

signals:

public slots:
private slots:
//    void switchSensorTypeSlot(int);
    void onGenerateXMLFile();
private:
    QLabel* RSIComTypeLabel, *RSIIPLabel, *RSIPortLabel, *RSIAllowDelayLabel;
    QLineEdit* RSIIPLineEdit, *RSIPortLineEdit, *RSIAllowDelayLineEdit;
    QComboBox* RSIComTypeComboBox;
    QPushButton* generateXMLBtn;
    QLabel* RSIEditorLabel;

    QTabWidget* RSIconfigTabWidget;
    QTextEdit* RSIProgramTextEdit;
    QHBoxLayout* topLayout, *bottomLayout;
    QVBoxLayout* mainLayout;
    QGridLayout* bottomRightLayout;

    RSIVariableWidget* variableWidget;
    RSIFunctionBlockWidget* fbWidget;
    RSICommunicationWidget* communicationWidget;
private:
    QString projectPath;
    XmlUntity* xml;
};

// 自定义变量模块
class RSIVariableWidget:public QWidget{
    Q_OBJECT
public:
    explicit RSIVariableWidget(QWidget* parent = nullptr);
    void generateVariablesNode(XmlUntity* xml, QDomElement& varRoot);
private:
    void setupVariabelModelView();
private slots:
    void addVairableSlot();
    void removeVariableSlot();
    void updateUserDefinedVariables();
private:
    QLabel* variableNameLabel, *variableTypeLabel, *variableInitValLabel;
    QLineEdit* variableNameLineEdit, *variableInitValLineEdit;
    QComboBox* variableTypeComboBox;
    QTableView* varTableView;
    QStandardItemModel* varTableModel;
    QPushButton* addVariableBtn,  *removeVariabelBtn;

    QGridLayout* rightbottomLayout;
    QHBoxLayout* mainLayout;
    QVBoxLayout* rightLayout;

private:
    QStringList varNameList;
};


class RSIFbBase;
class RSIFbPid;
class RSIFbPidN;
class RSIFbSetOffset;
class RSIFbDelay;
class RSIFbPosCorr;
class RSIFbAxisCorr;

// 功能块模块
class RSIFunctionBlockWidget:public QWidget
{
    Q_OBJECT
public:
    explicit RSIFunctionBlockWidget(QWidget* parent = nullptr);
    void generateFunctionBlockXMLNode(XmlUntity* xml, QDomElement& fbRoot);
private:
    void setupFBModelView();
private slots:
    void addFBSlot();
    void removeFBSlot();
private:
    QLabel* fbTypeLabel, *fbNameLabel;
    QComboBox* fbTypeComboBox;
    QLineEdit* fbNameLineEdit;
    QStackedWidget* fbParametersWidget;
    QPushButton* addFBBtn, *removeFBBtn;
    QStandardItemModel* fbTableModel;
    QTableView* fbTableView;
    RSIFbPid* fbPID;
    RSIFbPidN* fbPIDN;
    RSIFbSetOffset* fbSETOFFSET;
    RSIFbDelay* fbDELAY;
    RSIFbPosCorr* fbPOSCORR;
    RSIFbAxisCorr* fbAXISCORR;

    QVBoxLayout* rightLayout;
    QHBoxLayout* rightTopLayout, *rightBottomLayout, *mainLayout;
private:
    QStringList fbNameList;
    QHash<QString,QHash<QString,QString>> fbInfos;
    QList<RSIFbBase*> fbPtrs;
};

// 功能块基类
class RSIFbBase:public QWidget
{
public:
    explicit RSIFbBase(QWidget* parent = nullptr):QWidget(parent){}
    virtual void getFbInfo(QHash<QString,QString>&) = 0;
};

// PID功能块
class RSIFbPid:public RSIFbBase
{
//    Q_OBJECT
public:
    explicit RSIFbPid(QWidget* parent = nullptr);
    void getFbInfo(QHash<QString,QString>&);
private:
    QLabel* KpLabel, *KdLabel, *KiLabel;
    QLineEdit* KpLineEdit, *KdLineEdit, *KiLineEdit;
    QGridLayout* mainLayout;
};

// PIDN功能块
class RSIFbPidN:public RSIFbBase
{
//    Q_OBJECT
public:
    explicit RSIFbPidN(QWidget* parent = nullptr);
    void getFbInfo(QHash<QString,QString>&);
private:
    QLabel* KpLabel, *KdLabel, *KiLabel,*NLabel;
    QLineEdit* KpLineEdit, *KdLineEdit, *KiLineEdit, *NLineEdit;
    QGridLayout* mainLayout;
};

// SETOFFSET 功能块
class RSIFbSetOffset:public RSIFbBase
{
//    Q_OBJECT
public:
    explicit RSIFbSetOffset(QWidget* parent = nullptr);
    void getFbInfo(QHash<QString,QString>&);
private:
    QLabel* offsetCntLabel, *offsetSampleCntLabel;
    QLineEdit* offsetCntLineEdit, *offsetSampleCntLineEdit;
    QGridLayout* mainLayout;
};

// DELAY 功能块
class RSIFbDelay:public RSIFbBase
{
//    Q_OBJECT
public:
    explicit RSIFbDelay(QWidget* parent = nullptr);
    void getFbInfo(QHash<QString,QString>&);
private:
    QLabel* delayLabel;
    QLineEdit* delayLineEdit;
    QGridLayout* mainLayout;
};

// POSCORR 功能块
class RSIFbPosCorr:public RSIFbBase
{
public:
    explicit RSIFbPosCorr(QWidget* parent = nullptr);
    void getFbInfo(QHash<QString,QString>&);
private:
    QLabel* lowerLimXLabel, *lowerLimYLabel, *lowerLimZLabel, *upperLimXLabel, *upperLimYLabel, *upperLimZLabel,
            *corrTypeLabel, *maxRotAngleLabel, *refCorrSysLabel, *overTransLimLabel, *overRotAngleLabel;
    QLineEdit* lowerLimXLineEdit, *lowerLimYLineEdit, *lowerLimZLineEdit, *upperLimXLineEdit, *upperLimYLineEdit,
    *upperLimZLineEdit, *corrTypeLineEdit, *maxRotAngleLineEdit, *refCorrSysLineEdit, *overTransLimLineEdit, *overRotAngleLineEdit;
    QGridLayout* mainLayout;
};

// AXISCORR 功能块
class RSIFbAxisCorr:public RSIFbBase
{
public:
    explicit RSIFbAxisCorr(QWidget* parent = nullptr);
    void getFbInfo(QHash<QString,QString>&);
private:
    QLabel* lowerLimA1Label, *lowerLimA2Label, *lowerLimA3Label, *lowerLimA4Label, *lowerLimA5Label, *lowerLimA6Label,
    *upperLimA1Label, *upperLimA2Label, *upperLimA3Label, *upperLimA4Label, *upperLimA5Label, *upperLimA6Label,
    *overLimA1Label, *overLimA2Label, *overLimA3Label, *overLimA4Label, *overLimA5Label, *overLimA6Label,*corrTypeLabel;
    QLineEdit* lowerLimA1LineEdit, *lowerLimA2LineEdit, *lowerLimA3LineEdit, *lowerLimA4LineEdit, *lowerLimA5LineEdit,
    *lowerLimA6LineEdit, *upperLimA1LineEdit, *upperLimA2LineEdit, *upperLimA3LineEdit, *upperLimA4LineEdit,*upperLimA5LineEdit,
    *upperLimA6LineEdit, *overLimA1LineEdit, *overLimA2LineEdit, *overLimA3LineEdit, *overLimA4LineEdit, *overLimA5LineEdit,
    *overLimA6LineEdit,*corrTypeLineEdit;
    QGridLayout* mainLayout;
};

// 通信参数配置模块
class RSINormalSensorWidget;
class RSITerminalWidget;
class RSICommunicationWidget:public QWidget
{
    Q_OBJECT
public:
    explicit RSICommunicationWidget(QWidget* parent = nullptr);
    void generateCommunicationXMLNode(XmlUntity* xml, QDomElement& communicateElement);
private:
    QLabel* RSISensorTypeLabel, *RSISensorTrigTypeLabel, *RSICommunicationXMLRootNodeLabel;
    QComboBox* RSISensorTypeComboBox, *RSISensorTrigTypeComboBox;
    QLineEdit* RSICommunicationXMLRootNodeLineEdit;
    QStackedWidget* RSISensorStackWidget;
    RSINormalSensorWidget* sensorWidget;
    RSITerminalWidget* terminalWidget;
    QHBoxLayout* topLayout;
    QVBoxLayout* mainLayout;
};

// 普通传感器
class RSINormalSensorWidget:public QWidget
{
    Q_OBJECT
public:
    explicit RSINormalSensorWidget(QWidget* parent = nullptr);
    void generateNormalSensorXMLNode(XmlUntity* xml, QDomElement& communicateElement);
private:
    void leftLayoutInit();
    void rightUpperLayoutInit();
    void rightDownLayoutInit();
    void setSensorCfgModelView();
    void setSensorSendModelView();
    void setSensorRecvModelView();
public slots:
    void sensorTrigSwitched(int);
private slots:
    void onAddSensorConfigSection();
    void onRemoveSensorConfigSection();
    void onAddSensorConfigContent();
    void onRemoveSensorConfigContent();
    void onAddSensorSendSection();
    void onRemoveSensorSendSection();
    void onAddSensorSendContent();
    void onRevmoveSensorSendContent();
    void onAddSensorRecvConfig();
    void onRemoveSensorRecvConfig();
private:
    QLabel* sensorSectionTypeLabel;
    QComboBox* sensorSectionTypeComboBox;
    QPushButton* addSensorSectionBtn, *removeSensorSectionBtn, *addCfgContentBtn, *removeCfgContentBtn;
    QStandardItemModel* sensorCfgTableModel;
    QTableView* sensorCfgTableView;
    QVBoxLayout* leftLayout;
    QHBoxLayout* leftTopLayout, *leftBottomLayout;

    QLabel* sendSectionTypeLabel;
    QComboBox* sendSectionTypeComboBox;
    QPushButton* addSendSectionBtn, *removeSendSectionBtn, *addSendContentBtn, *removeSendContentBtn;
    QStandardItemModel* sensorSendTableModel;
    QTableView* sensorSendTableView;
    QVBoxLayout* rightUpperLayout;
    QHBoxLayout* rightUpperTopLayout, *rightUpperBottomLayout;

    QLabel* sensorRecvOffBytesLabel;
    QLineEdit* sensorRecvOffBytesLineEdit;
    QStandardItemModel* sensorRecvTableModel;
    QTableView* sensorRecvTableView;
    QPushButton* addRecvCfgBtn, *removeRecvCfgBtn;
    ComboDelegate* varTypeDelegate, *varNameDelegate;
    QHBoxLayout* rightDownBottomLayout;
    QVBoxLayout* rightDownLayout;

    QVBoxLayout* rightLayout;
    QHBoxLayout* mainLayout;

};

/*====================== 终端设备添加配置对话框 =================================*/
class RSISendRecvNodeDlg:public QDialog
{
  Q_OBJECT
public:
    explicit RSISendRecvNodeDlg(const QStringList& varList, QWidget* parent = nullptr);
    void updateVariableList(const QStringList& varList);
signals:
    void createNewCommunicateNode(QStringList);
public slots:
    void onUpdateVariables();
private slots:
    void onOkBtnClicked();
    void onCancelBtnClicked();
    void updateVariableType(QString varName);
private:
    QLabel* xmlNodeNameLabel, *xmlNodeAttributeLabel, *variableLabel, *variableTypeLabel;
    QLineEdit* xmlNodeNameLineEdit, *xmlNodeAttributeLineEdit, *variableTypeLineEdit;
    QComboBox* variableComboBox; //*variableTypeComboBox;
    QPushButton* OkBtn,*cancelBtn;
    QGridLayout* topLayout;
    QHBoxLayout* bottomLayout;
    QVBoxLayout* mainLayout;
};

// 终端设备
class RSITerminalWidget:public QWidget
{
    Q_OBJECT
public:
    explicit RSITerminalWidget(QWidget* parent = nullptr);
    void generateTerminalXMLNode(XmlUntity* xml, QDomElement& communicateElement);
private:
    void setSendRecvModelView();
private slots:
    void updateSendModel(QStringList);
    void updateRecvModel(QStringList);
    void onRemoveSendConfig();
    void onRemoveRecvConfig();
private:
    QLabel* sendTitleLabel, *recvTitleLabel;
    QPushButton* addSendConfigBtn, *removeSendConfigBtn, *addRecvConfigBtn, *removeRecvConfigBtn;
    QTableView* sendConfigTableView, *recvConfigTableView;
    QStandardItemModel* sendConfigTableModel, *recvConfigTableModel;
    RSISendRecvNodeDlg* sendDlg, *recvDlg;

    QVBoxLayout* leftLayout, *rightLayout;
    QHBoxLayout* leftTopLayout, *rightTopLayout, *mainLayout;
};

#endif // RSICONFIG_H

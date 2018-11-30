#include "robotconfig.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>

static const int AnalogStartIndex = 16;     // 模拟IO起始组下标
static const int PortCntOneGroup = 8;       // 一个IO组中的IO通道数

RobotConfig::RobotConfig(QString path, QWidget *parent) : QWidget(parent), projectPath(path)
{
    rbtPlatformLabel = new QLabel(tr("机器人运行平台："));
    rbtPlatformComboBox = new QComboBox;
    rbtPlatformComboBox->addItem(tr("请选择"));
    rbtPlatformComboBox->addItem(tr("IPC"));
    rbtPlatformComboBox->addItem(tr("AMR"));
    rbtPlatformLabel->setAlignment(Qt::AlignCenter);

    bitCountLabel = new QLabel(tr("平台位数："));
    bitCountComboBox = new QComboBox;
    bitCountComboBox->addItem(tr("请选择"));
    bitCountComboBox->addItem(tr("32位"));
    bitCountComboBox->addItem(tr("64位"));
    bitCountLabel->setAlignment(Qt::AlignCenter);

    bitOrderLabel = new QLabel(tr("字节序："));
    bitOrderComboBox = new QComboBox;
    bitOrderComboBox->addItem(tr("请选择"));
    bitOrderComboBox->addItem(tr("小端"));
    bitOrderComboBox->addItem(tr("大端"));
    bitOrderLabel->setAlignment(Qt::AlignCenter);

    generateConfFileBtn = new QPushButton(tr("生成配置文件"));
    connect(generateConfFileBtn,SIGNAL(clicked(bool)),this,SLOT(generateXmlFile()));

    topLayout = new QHBoxLayout;
    topLayout->addWidget(rbtPlatformLabel);
    topLayout->addWidget(rbtPlatformComboBox);
    topLayout->addWidget(bitCountLabel);
    topLayout->addWidget(bitCountComboBox);
    topLayout->addWidget(bitOrderLabel);
    topLayout->addWidget(bitOrderComboBox);
    topLayout->addWidget(generateConfFileBtn);

//    rbtGroupBox = new QGroupBox(tr("机器人配置区"));
    configTabWidget = new QTabWidget;
    rbtNameLabel = new QLabel(tr("机器人名称："));
    rbtNameLineEdit = new QLineEdit;
    rbtTypeLabel = new QLabel(tr("机器人类型："));
    rbtTypeCombobox = new QComboBox;
    rbtTypeCombobox->addItem(tr("普通六轴机器人"));
    rbtTypeCombobox->addItem(tr("其他"));
    addAxisBtn = new QPushButton(tr("添加轴"));
    removeAxisBtn = new QPushButton(tr("移除轴"));
    connect(addAxisBtn,SIGNAL(clicked(bool)),this,SLOT(addAxisToModel()));
    connect(removeAxisBtn,SIGNAL(clicked(bool)),this,SLOT(removeAxisFromModel()));
//    connect(rbtNameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(getRobotName(QString)));
    connect(rbtTypeCombobox,SIGNAL(currentIndexChanged(int)),SLOT(switchRobotModelGraph(int)));
    rbtGraphLabel = new QLabel;
    rbtGraphLabel->setScaledContents(true);
    img = new QImage;
    if(img->load(":/robotmodel/NormalRobot.png")){
            rbtGraphLabel->setPixmap(QPixmap::fromImage(*img));
    }

    setupRobotConfigModel();
    setupRobotConfigView();

    rbtSectionTopLayout = new QHBoxLayout;
    rbtSectionTopLayout->addWidget(rbtNameLabel);
    rbtSectionTopLayout->addWidget(rbtNameLineEdit);
    rbtSectionTopLayout->addWidget(rbtTypeLabel);
    rbtSectionTopLayout->addWidget(rbtTypeCombobox);
    rbtSectionTopLayout->addStretch(1);
    rbtSectionTopLayout->addWidget(addAxisBtn);
    rbtSectionTopLayout->addWidget(removeAxisBtn);
    rbtSectionLeftLayout = new QVBoxLayout;
    rbtSectionLeftLayout->addLayout(rbtSectionTopLayout);
    rbtSectionLeftLayout->addWidget(rbtDHParTableView);
    rbtSectionLeftLayout->addWidget(rbtLimParTableView);
    rbtSectionLayout = new QHBoxLayout;
    rbtSectionLayout->addLayout(rbtSectionLeftLayout);
    rbtSectionLayout->addWidget(rbtGraphLabel);
//    rbtGroupBox->setLayout(rbtSectionLayout);
    rbtConfWidget = new QWidget;
    rbtConfWidget->setLayout(rbtSectionLayout);
    configTabWidget->addTab(rbtConfWidget,"机器人本体参数配置");


    // IO及伺服配置区界面
    devConfWidget = new QWidget;
    devIntervalLabel = new QLabel(tr("现场总线周期(ns)："));
    devIntervalLineEdit = new QLineEdit;
    devBusTypeLabel = new QLabel(tr("现场总线类型："));
    devBusTypeComboBox = new QComboBox;
    devBusTypeComboBox->addItem(tr("请选择"));
    devBusTypeComboBox->addItem(tr("EtherCAT"));
    devBusTypeComboBox->addItem(tr("Powerlink"));
    devTopLayout = new QHBoxLayout;
    devTopLayout->addWidget(devIntervalLabel);
    devTopLayout->addWidget(devIntervalLineEdit);
    devTopLayout->addStretch(1);
    devTopLayout->addWidget(devBusTypeLabel);
    devTopLayout->addWidget(devBusTypeComboBox);
    devTopLayout->addStretch(1);

    digitLabel = new QLabel("数字IO");
    digitLabel->setAlignment(Qt::AlignCenter);
    analogLabel = new QLabel("模拟IO");
    analogLabel->setAlignment(Qt::AlignCenter);
    addIOModuleBtn = new QPushButton(tr("添加IO模块"));
    removeDigitIOBtn = new QPushButton(tr("移除数字IO模块"));
    removeAnalogIOBnt = new QPushButton(tr("移除模拟IO模块"));
    setupDeviceConfigModel();
    setupDeviceConfigView();

    addDialog = new AddIODlg(devConfWidget);
    connect(addIOModuleBtn,SIGNAL(clicked(bool)),addDialog,SLOT(showAddIOdialog()));
    connect(addDialog,SIGNAL(addIOModule(QString,QString,QString,int)),this,SLOT(addIOToModle(QString,QString,QString,int)));
    connect(removeDigitIOBtn,SIGNAL(clicked(bool)),this,SLOT(removeDigitIOFromModel()));
    connect(removeAnalogIOBnt,SIGNAL(clicked(bool)),this,SLOT(removeAnalogIOFromModel()));

    IOLeftLayout = new QVBoxLayout;
    IOLeftLayout->addWidget(digitLabel);
    IOLeftLayout->addWidget(digitIOTableView);
    IOMidLayout = new QVBoxLayout;
    IOMidLayout->addWidget(analogLabel);
    IOMidLayout->addWidget(analogIOTableView);
    IORightLayout = new QVBoxLayout;
    IORightLayout->addWidget(addIOModuleBtn);
    IORightLayout->addWidget(removeDigitIOBtn);
    IORightLayout->addWidget(removeAnalogIOBnt);
    IOLayout = new QHBoxLayout;
    IOLayout->addLayout(IOLeftLayout);
    IOLayout->addLayout(IOMidLayout);
    IOLayout->addLayout(IORightLayout);
    IOGroupBox = new QGroupBox(tr("IO配置区"));
    IOGroupBox->setLayout(IOLayout);

    servoLayout = new QHBoxLayout;
    servoLayout->addWidget(servoTableView);
    servoGroupBox = new QGroupBox(tr("伺服轴配置区"));
    servoGroupBox->setLayout(servoLayout);

    devMainLayout = new QVBoxLayout;
    devMainLayout->addLayout(devTopLayout);
    devMainLayout->addWidget(IOGroupBox);
    devMainLayout->addWidget(servoGroupBox);
    devConfWidget->setLayout(devMainLayout);
    configTabWidget->addTab(devConfWidget,"IO及伺服参数配置");

    // 配置主界面布局
    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setMargin(5);
    mainLayout->addLayout(topLayout,0,0);
//    mainLayout->addWidget(rbtGroupBox,1,0);
    mainLayout->addWidget(configTabWidget,1,0);

    // 变量初始化
    xml = nullptr;
    rbtType = NORMAL_TYPE;
//    deviceCount = 0;
    digitIOCount = 0;
    analogIOCount = 0;
    digitIOInGrpCnt = 0;
    digitIOOutGrpCnt = 0;
    analogIOInGrpCnt = 0;
    analogIOOutGrpCnt = 0;
    servoAxisCnt = 0;
}

/**
 * @brief RobotConfig::setupRobotConfigModel
 *        设置机器人参数模型
 */
void RobotConfig::setupRobotConfigModel()
{
    rbtDHModel = new QStandardItemModel(0,5,this);
    rbtDHModel->setHeaderData(0,Qt::Horizontal,tr("theta"));
    rbtDHModel->setHeaderData(1,Qt::Horizontal,tr("d"));
    rbtDHModel->setHeaderData(2,Qt::Horizontal,tr("a"));
    rbtDHModel->setHeaderData(3,Qt::Horizontal,tr("alpha"));
    rbtDHModel->setHeaderData(4,Qt::Horizontal,tr("offset"));
//    rbtDHModel->setVerticalHeaderLabels(QStringList()<<tr("DH 参数"));

    rbtLimModel = new QStandardItemModel(0,8,this);
    rbtLimModel->setHeaderData(0,Qt::Horizontal,tr("最大角度"));
    rbtLimModel->setHeaderData(1,Qt::Horizontal,tr("最小角度"));
    rbtLimModel->setHeaderData(2,Qt::Horizontal,tr("最大速度"));
    rbtLimModel->setHeaderData(3,Qt::Horizontal,tr("最大加速度"));
    rbtLimModel->setHeaderData(4,Qt::Horizontal,tr("最大减速度"));
    rbtLimModel->setHeaderData(5,Qt::Horizontal,tr("最大加加速度"));
    rbtLimModel->setHeaderData(6,Qt::Horizontal,tr("零位置偏移"));
    rbtLimModel->setHeaderData(7,Qt::Horizontal,tr("方向"));

    limHeadList << "max_pos" << "min_pos" << "max_vel" << "max_acc" << "max_dec" << "max_jerk";
}

/**
 * @brief RobotConfig::setupRobotConfigView
 *        设置机器人模型视图
 */
void RobotConfig::setupRobotConfigView()
{
    rbtDHParTableView = new QTableView;
    rbtDHParTableView->setModel(rbtDHModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(rbtDHModel);
    rbtDHParTableView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),rbtDHParTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));

    rbtLimParTableView = new QTableView;
    rbtLimParTableView->setModel(rbtLimModel);
    QItemSelectionModel* selectionModel2 = new QItemSelectionModel(rbtLimModel);
    rbtLimParTableView->setSelectionModel(selectionModel2);
    connect(selectionModel2,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),rbtLimParTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
    /*
    QModelIndex topLeft;
    QModelIndex bottomRight;
    topLeft = rbtConfigModel->index(0, 0, QModelIndex());
    bottomRight = rbtConfigModel->index(0, 6, QModelIndex());
    QItemSelection selection(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
    */
}

/**
 * @brief RobotConfig::addAxisToModel
 *        添加轴按钮slot
 */
void RobotConfig::addAxisToModel()
{
    bool ok;
    QString text = QInputDialog::getText(this,tr("添加轴"),tr("轴名称："),QLineEdit::Normal,QString(""),&ok);
//    if(axisNameSet.find(text) != axisNameSet.end()){
    if(axisNameList.indexOf(text) != -1){
        QMessageBox::critical(this,tr("Error"),tr("请输入正确的轴名称！"));
        return;
    }
    axisNameList << text;
    int row = rbtDHModel->rowCount();
    if(ok && !text.isEmpty()){
        QStandardItem* item = new QStandardItem(text);
        qDebug() << "addAxisToModel" << item->text() << " " << item->index().column();
        rbtDHModel->setVerticalHeaderItem(row,new QStandardItem(text));
        rbtLimModel->setVerticalHeaderItem(row,new QStandardItem(text));
        // 向伺服标准模型中添加轴
        servoModel->setVerticalHeaderItem(row,new QStandardItem(text));
        servoModel->setData(servoModel->index(row,0,QModelIndex()),row+digitIOCount+analogIOCount);
        servoModel->setData(servoModel->index(row,1,QModelIndex()),row+1);
    }
    //    rbtDHModel->setData(rbtDHModel->index(0,0,QModelIndex()),100);
}

/**
 * @brief RobotConfig::removeAxisFromModel
 *        移除轴按钮处理slot
 */
void RobotConfig::removeAxisFromModel()
{
    int row = rbtDHParTableView->currentIndex().row();
    if(row < 0)
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
    QString name = rbtDHParTableView->model()->headerData(row,Qt::Vertical).toString();
    rbtDHModel->removeRows(row,1,QModelIndex());
    rbtLimModel->removeRow(row,QModelIndex());
    QStringList::iterator it = axisNameList.begin();
    for(;it != axisNameList.end(); ++it)
        if(*it == name){
            axisNameList.erase(it);
            break;
        }
}

/**
 * @brief RobotConfig::switchRobotModelGraph
 *        改变显示机器人模型图
 */
void RobotConfig::switchRobotModelGraph(int index)
{
    int ret = 0;
    switch(index){
    case 0:
        rbtType = NORMAL_TYPE;
        ret = img->load(":/robotmodel/NormalRobot.png");
        break;
    case 1:
        rbtType = OTHER;
        ret = img->load(":/robotmodel/Other.png");
        break;
    default:
        break;
    }
    if(ret)
        rbtGraphLabel->setPixmap(QPixmap::fromImage(*img));
}

//===============================================================================
/*      设备配置区函数                                                           */
//===============================================================================

/**
 * @brief RobotConfig::setupDeviceConfigModel  创建数字模拟IO及伺服模型
 */
void RobotConfig::setupDeviceConfigModel()
{
    digitIOModel = new QStandardItemModel(0,5,this);
    digitIOModel->setHeaderData(0,Qt::Horizontal,tr("设备ID"));
    digitIOModel->setHeaderData(1,Qt::Horizontal,tr("IO模块名称"));
    digitIOModel->setHeaderData(2,Qt::Horizontal,tr("输入输出类型"));
    digitIOModel->setHeaderData(3,Qt::Horizontal,tr("组ID"));
    digitIOModel->setHeaderData(4,Qt::Horizontal,tr("组初始值"));

    analogIOModel = new QStandardItemModel(0,6,this);
    analogIOModel->setHeaderData(0,Qt::Horizontal,tr("设备ID"));
    analogIOModel->setHeaderData(1,Qt::Horizontal,tr("IO模块名称"));
    analogIOModel->setHeaderData(2,Qt::Horizontal,tr("输入输出类型"));
    analogIOModel->setHeaderData(3,Qt::Horizontal,tr("组ID"));
    analogIOModel->setHeaderData(4,Qt::Horizontal,tr("通道ID"));
    analogIOModel->setHeaderData(5,Qt::Horizontal,tr("通道初始值"));

    servoModel = new QStandardItemModel(0,7,this);
    servoModel->setHeaderData(0,Qt::Horizontal,tr("设备ID"));
    servoModel->setHeaderData(1,Qt::Horizontal,tr("轴ID"));
    servoModel->setHeaderData(2,Qt::Horizontal,tr("轴类型"));
    servoModel->setHeaderData(3,Qt::Horizontal,tr("正向最大角度"));
    servoModel->setHeaderData(4,Qt::Horizontal,tr("负向最大角度"));
    servoModel->setHeaderData(5,Qt::Horizontal,tr("减速比"));
    servoModel->setHeaderData(6,Qt::Horizontal,tr("编码器精度"));
    servoHeadList << "pid" << "id" << "type" << "max_pos" << "min_pos" << "ratio" << "encode";
}

/**
 * @brief RobotConfig::setupDeviceConfigView  创建数字模拟IO及伺服表格视图
 */
void RobotConfig::setupDeviceConfigView()
{
    digitIOTableView = new QTableView;
    digitIOTableView->setModel(digitIOModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(digitIOModel);
    digitIOTableView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),digitIOTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));

    analogIOTableView = new QTableView;
    analogIOTableView->setModel(analogIOModel);
    QItemSelectionModel* selectionModel2 = new QItemSelectionModel(analogIOModel);
    analogIOTableView->setSelectionModel(selectionModel2);
    connect(selectionModel2,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),analogIOTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));

    servoTableView = new QTableView;
    servoTableView->setModel(servoModel);
    QItemSelectionModel* selectionModel3 = new QItemSelectionModel(servoModel);
    servoTableView->setSelectionModel(selectionModel3);
    connect(selectionModel3,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),servoTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

/**
 * @brief RobotConfig::addIOToModle 添加IO模块SLOT
 * @param moduleName                IO模块名称
 * @param iotype                    IO类型（digit | analog）
 * @param iodir                     IO方向（input | output）
 * @param portcnt                   IO模块通道数
 */
void RobotConfig::addIOToModle(QString moduleName, QString iotype, QString iodir,int portcnt)
{
    qDebug() << "addIOToModel " << moduleName << "  " << iotype << "  " << portcnt;
    int grpCount = portcnt % PortCntOneGroup == 0 ? portcnt/PortCntOneGroup:portcnt/PortCntOneGroup+1;
    if(iotype == "digit"){
        int row = digitIOModel->rowCount();
        int grpIndex;
        for(int i= 0; i < grpCount; ++i){
            digitIOModel->insertRows(row,1,QModelIndex());
            digitIOModel->setData(digitIOModel->index(row,0,QModelIndex()),digitIOCount);
            digitIOModel->setData(digitIOModel->index(row,1,QModelIndex()),moduleName);
            digitIOModel->setData(digitIOModel->index(row,2,QModelIndex()),iodir);
            if(iodir == "input") grpIndex = digitIOInGrpCnt++;
            else                 grpIndex = digitIOOutGrpCnt++;
            digitIOModel->setData(digitIOModel->index(row,3,QModelIndex()),grpIndex);
            row++;
        }
        for(int i = 0; i < analogIOModel->rowCount(); ++i)
            analogIOModel->setData(analogIOModel->index(i,0,QModelIndex()),analogIOModel->index(i,0,QModelIndex()).data().toInt()+1);
        for(int i = 0; i < servoModel->rowCount(); ++i)
            servoModel->setData(servoModel->index(i,0,QModelIndex()),servoModel->index(i,0,QModelIndex()).data().toInt()+1);
        digitIOCount++;
    }else if(iotype == "analog")
    {
        int row = analogIOModel->rowCount();
        int grpIndex;
        for(int i = 0; i < portcnt; ++i){
            analogIOModel->insertRows(row,1,QModelIndex());
            analogIOModel->setData(analogIOModel->index(row,0,QModelIndex()),digitIOCount+analogIOCount);
            analogIOModel->setData(analogIOModel->index(row,1,QModelIndex()),moduleName);
            analogIOModel->setData(analogIOModel->index(row,2,QModelIndex()),iodir);
            if(iodir == "input") { grpIndex = AnalogStartIndex + analogIOInGrpCnt; if((i+1)%PortCntOneGroup == 0) analogIOInGrpCnt++;}
            else                 { grpIndex = AnalogStartIndex + analogIOOutGrpCnt; if((i+1)%PortCntOneGroup == 0) analogIOOutGrpCnt++;}
            analogIOModel->setData(analogIOModel->index(row,3,QModelIndex()),grpIndex);
            analogIOModel->setData(analogIOModel->index(row,4,QModelIndex()),i%PortCntOneGroup);
            row++;
        }
        for(int i = 0; i < servoModel->rowCount(); ++i)
            servoModel->setData(servoModel->index(i,0,QModelIndex()),servoModel->index(i,0,QModelIndex()).data().toInt()+1);
        analogIOCount++;
    }
}

/**
 * @brief RobotConfig::removeDigitIOFromModel 移除数字IO模块
 *        更新其他数字IO设备ID和组ID， 更新模拟IO设备ID， 更新伺服设备ID
 */
void RobotConfig::removeDigitIOFromModel()
{
    int row = digitIOTableView->currentIndex().row();
    if(row < 0)
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
    int temprow = row;
    int deviceID = digitIOModel->index(temprow,0,QModelIndex()).data().toInt();
    temprow--;
    while(temprow >= 0){
        if(digitIOModel->index(temprow,0,QModelIndex()).data().toInt() != deviceID)
            break;
        row = temprow;
        temprow--;
    }
    temprow = digitIOTableView->currentIndex().row();
    int rowend = temprow;
    temprow++;
    while(temprow < digitIOModel->rowCount()){
        if(digitIOModel->index(temprow,0,QModelIndex()).data().toInt() != deviceID)
            break;
        rowend = temprow;
        temprow++;
    }
    int removeCnt = rowend-row+1;
    QString IOdir = digitIOModel->index(row,2,QModelIndex()).data().toString();
    for(int i = rowend+1; i < digitIOModel->rowCount(); ++i){
        digitIOModel->setData(digitIOModel->index(i,0,QModelIndex()),digitIOModel->index(i,0,QModelIndex()).data().toInt()-1);
        QString tempDir = digitIOModel->index(row,2,QModelIndex()).data().toString();
        if(tempDir == IOdir)
            digitIOModel->setData(digitIOModel->index(i,3,QModelIndex()),digitIOModel->index(i,3,QModelIndex()).data().toInt()-removeCnt);
    }
    for(int i = 0; i < analogIOModel->rowCount(); ++i)
        analogIOModel->setData(analogIOModel->index(i,0,QModelIndex()),analogIOModel->index(i,0,QModelIndex()).data().toInt()-1);
    for(int i = 0; i < servoModel->rowCount(); ++i)
        servoModel->setData(servoModel->index(i,0,QModelIndex()),analogIOModel->index(i,0,QModelIndex()).data().toInt()-1);

    digitIOModel->removeRows(row,removeCnt,QModelIndex());
    if(IOdir == "input")    digitIOInGrpCnt -= removeCnt;
    else                    digitIOOutGrpCnt -= removeCnt;
    digitIOCount--;

}

/**
 * @brief RobotConfig::removeAnalogIOFromModel 移除模拟IO模块
 *      更新模拟IO模块设备ID,组ID, 更新伺服设备ID
 */
void RobotConfig::removeAnalogIOFromModel()
{
    int row = analogIOTableView->currentIndex().row();
    if(row < 0)
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
    int deviceID = analogIOModel->index(row,0,QModelIndex()).data().toInt();
    int temprow = row-1;
    while(temprow >= 0){
        if(analogIOModel->index(temprow,0,QModelIndex()).data().toInt() != deviceID)
            break;
        row = temprow;
        temprow--;
    }
    int rowend = analogIOTableView->currentIndex().row();
    temprow = rowend+1;
    while(temprow < analogIOModel->rowCount()){
       if(analogIOModel->index(temprow,0,QModelIndex()).data().toInt() != deviceID)
           break;
       rowend = temprow;
       temprow++;
    }
    int removecnt = rowend-row+1;
    int grpremoved = removecnt % PortCntOneGroup == 0 ? removecnt/PortCntOneGroup:removecnt/PortCntOneGroup+1;
    QString IOdir = analogIOModel->index(row,2,QModelIndex()).data().toString();
    for(int i = rowend+1; i < analogIOModel->rowCount(); ++i){
        analogIOModel->setData(analogIOModel->index(i,0,QModelIndex()),analogIOModel->index(i,0,QModelIndex()).data().toInt()-1);
        if(analogIOModel->index(i,2,QModelIndex()).data().toString() == IOdir)
            analogIOModel->setData(analogIOModel->index(i,3,QModelIndex()),analogIOModel->index(i,3,QModelIndex()).data().toInt()-grpremoved);
    }
    for(int i = 0; i < servoModel->rowCount(); ++i)
        servoModel->setData(servoModel->index(i,0,QModelIndex()),analogIOModel->index(i,0,QModelIndex()).data().toInt()-1);
    analogIOModel->removeRows(row,removecnt,QModelIndex());
    if(IOdir == "input")  analogIOInGrpCnt -= grpremoved;
    else                  analogIOOutGrpCnt -= grpremoved;
    analogIOCount--;
}


//===============================================================================
/*      生成配置文件                                                            */
//===============================================================================
//void RobotConfig::getRobotName(QString)
//{
//    rbtName = rbtNameLineEdit->text();
//}

/**
 * @brief RobotConfig::getAllWidgetsData 读取所有非模型控件的数据，在生成配置文件时调用
 */
bool RobotConfig::getAllWidgetsData()
{
    platformName = rbtPlatformComboBox->currentText();
    int index = bitCountComboBox->currentIndex();
    if(index > 0)   bitCountStr = index == 1 ? "SYS_TYPE_32" : "SYS_TYPE_64";
    index = bitOrderComboBox->currentIndex();
    if(index > 0)   bitOrderStr = index == 1 ? "BYTE_ORDER_LIT" : "BYTE_ORDER_BIT";
//    qDebug() << "getAllWidgetsData " << platformName << " " << bitCountStr << " " << bitOrderStr;
    if(platformName.isEmpty() || bitCountStr.isEmpty() || bitOrderStr.isEmpty())
    {
        QMessageBox::warning(this,tr("Warning"),tr("请确认运行平台参数"));
        return false;
    }

    rbtName = rbtNameLineEdit->text();
    axisCount = rbtDHModel->rowCount(QModelIndex());

    busIntervalStr = devIntervalLineEdit->text();
    IOModuleCount = digitIOCount + analogIOCount;
    busTypeStr = devBusTypeComboBox->currentText();

//    qDebug() << busTypeStr << " " << rbtName;
    if(busTypeStr.isEmpty() || rbtName.isEmpty())
    {
        QMessageBox::warning(this,tr("Warning"),tr("请确认参数是否完整"));
        return false;
    }
    return true;
}
/**
 * @brief RobotConfig::generateXmlFile
 *        生成机器人配置文件节点
 */
void RobotConfig::generateXmlFile()
{
    if(!getAllWidgetsData()) return;
    QDir tempPath;
    QString currentPath = tempPath.currentPath();
    tempPath.setCurrent(projectPath);
    if(xml == nullptr)
        xml = new XmlUntity("config.xml","config");
    QDomElement rootElement = xml->doc.firstChildElement();
    QDomElement headerElement;
    if((headerElement = rootElement.firstChildElement(tr("header"))).isNull()){
        headerElement = xml->doc.createElement(tr("header"));
        rootElement.appendChild(headerElement);
    }
    headerElement.setAttribute("type",bitCountStr);
    headerElement.setAttribute("order",bitOrderStr);
    headerElement.setAttribute("target",platformName);

    generateIOXmlNode();
    generateServoXmlNode();
    generateRobotXmlNode();

    xml->writeXmlDoc();
    tempPath.setCurrent(currentPath);
}

/**
 * @brief RobotConfig::generateIOXmlNode 产生配置文件IO节点
 */
void RobotConfig::generateIOXmlNode()
{
    QDomElement rootElement = xml->doc.firstChildElement();
    QDomElement IONode;
    if((IONode = rootElement.firstChildElement(tr("ios"))).isNull()){
        IONode = xml->doc.createElement(tr("ios"));
        rootElement.appendChild(IONode);
    }
    while(IONode.hasChildNodes())
    {
        QDomNode tempNode = IONode.firstChild();
        IONode.removeChild(tempNode);
    }
    IONode.setAttribute("period",busIntervalStr);
    IONode.setAttribute("bustype",busTypeStr);
    IONode.setAttribute("count",IOModuleCount);
    rootElement.appendChild(IONode);

    // 添加数字IO
    int pid = -1;
    QDomElement digitElement;
    int grpCnt = 0;
    for(int i = 0; i < digitIOModel->rowCount(); ++i){
        int temppid = digitIOModel->index(i,0,QModelIndex()).data().toInt();
        if(temppid != pid)
        {
            if(!digitElement.isNull()){
                digitElement.setAttribute("count",grpCnt*PortCntOneGroup);
                IONode.appendChild(digitElement);
            }
            digitElement = xml->doc.createElement(tr("io"));
            QString moduleName = digitIOModel->index(i,1,QModelIndex()).data().toString();
            QString IOdir = digitIOModel->index(i,2,QModelIndex()).data().toString();
            digitElement.setAttribute("pid",temppid);
            digitElement.setAttribute("name",moduleName);
            digitElement.setAttribute("iotype",IOdir);
            digitElement.setAttribute("datatype","digit");
            pid = temppid;
            grpCnt = 0;
        }
        QDomElement grpElement = xml->doc.createElement(tr("group"));
        int grpID = digitIOModel->index(i,3,QModelIndex()).data().toInt();
        QString initVal = digitIOModel->index(i,4,QModelIndex()).data().toString();
        grpElement.setAttribute("id",grpID);
        grpElement.setAttribute("init",initVal);
        digitElement.appendChild(grpElement);
        grpCnt++;
    }
    if(!digitElement.isNull()){
        digitElement.setAttribute("count",grpCnt*PortCntOneGroup);
        IONode.appendChild(digitElement);
        grpCnt = 0;
    }

    // 添加模拟IO
    pid = -1;
    int grpid = -1;
    QDomElement analogElement;
    QDomElement anagrpElement;
    int portcnt = 0;
    for(int i = 0; i < analogIOModel->rowCount(); ++i){
        int temppid = analogIOModel->index(i,0,QModelIndex()).data().toInt();
        int tempgrpid = analogIOModel->index(i,3,QModelIndex()).data().toInt();
        if(temppid != pid){
            if(!analogElement.isNull()){
                analogElement.setAttribute("count",portcnt);
                IONode.appendChild(analogElement);
            }
            analogElement = xml->doc.createElement(tr("io"));
            QString moduleName = analogIOModel->index(i,1,QModelIndex()).data().toString();
            QString IOdir = analogIOModel->index(i,2,QModelIndex()).data().toString();
            analogElement.setAttribute("pid",temppid);
            analogElement.setAttribute("name",moduleName);
            analogElement.setAttribute("iotype",IOdir);
            analogElement.setAttribute("datatype","analog");
            pid = temppid;
            portcnt = 0;
        }
        if(tempgrpid != grpid){
            anagrpElement = xml->doc.createElement(tr("group"));
            anagrpElement.setAttribute("id",tempgrpid);
            analogElement.appendChild(anagrpElement);
            grpid = tempgrpid;
        }
        QDomElement initElement = xml->doc.createElement(tr("init"));
        QString analogValue = analogIOModel->index(i,5,QModelIndex()).data().toString();
        QDomText text = xml->doc.createTextNode(analogValue);
        initElement.appendChild(text);
        anagrpElement.appendChild(initElement);
        portcnt++;
    }
    if(!analogElement.isNull()){
        analogElement.setAttribute("count",portcnt);
        IONode.appendChild(analogElement);
    }
}

void RobotConfig::generateServoXmlNode()
{
    QDomElement rootElement = xml->doc.firstChildElement();
    QDomElement servoNode;
    if((servoNode = rootElement.firstChildElement(tr("servos"))).isNull()){
        servoNode = xml->doc.createElement(tr("servos"));
        rootElement.appendChild(servoNode);
    }
    servoNode.setAttribute("period",busIntervalStr);
    servoNode.setAttribute("bustype",busTypeStr);
    servoNode.setAttribute("count",axisCount);
    while(servoNode.hasChildNodes()){
        QDomNode tempNode = servoNode.firstChild();
        servoNode.removeChild(tempNode);
    }
    for(int i = 0; i < axisCount; ++i)
    {
        QDomElement servoElement = xml->doc.createElement(tr("servo"));
        QString name = servoModel->headerData(i,Qt::Vertical).toString();
        servoElement.setAttribute("name",name);
        for(int j = 0; j < servoModel->columnCount(); ++j){
            QString tempStr = servoModel->index(i,j,QModelIndex()).data().toString();
            servoElement.setAttribute(servoHeadList.at(j),tempStr);
        }
        servoNode.appendChild(servoElement);
    }
}


void RobotConfig::generateRobotXmlNode()
{
    QDomElement rootElement = xml->doc.firstChildElement();
    QDomElement rbtNode;
    if((rbtNode = rootElement.firstChildElement("robot")).isNull()){
        rbtNode = xml->doc.createElement(tr("robot"));
        rootElement.appendChild(rbtNode);
    }
    int axisCount = rbtDHModel->rowCount(QModelIndex());
    rbtNode.setAttribute("count",axisCount);
    rbtNode.setAttribute("type",QString::number(rbtType));
    rbtNode.setAttribute("name",rbtName);

    while(rbtNode.hasChildNodes()){
        QDomNode tmpNode = rbtNode.firstChild();
        qDebug() << tmpNode.toElement().tagName();
        rbtNode.removeChild(tmpNode);
    }
    for(int i = 0; i < axisCount; ++i){
        QDomElement axisNode = xml->doc.createElement(tr("AXISINFO"));
        QString axisName = rbtLimModel->headerData(i,Qt::Vertical).toString();
        QString axisOff = rbtLimModel->index(i,6,QModelIndex()).data().toString();
        QString axisDir = rbtLimModel->index(i,7,QModelIndex()).data().toString();
        int dir = (axisDir == "+" ? 1 : -1);
        axisNode.setAttribute("offset",axisOff);
        axisNode.setAttribute("dir",QString::number(dir));
        axisNode.setAttribute("name",axisName);
        rbtNode.appendChild(axisNode);
        QDomElement DHElement = xml->doc.createElement("DH");
        for(int j = 0; j < rbtDHModel->columnCount(); ++j){
            QString tmpStr = rbtDHModel->headerData(j,Qt::Horizontal).toString();
            QString value = rbtDHModel->index(i,j,QModelIndex()).data().toString();
            DHElement.setAttribute(tmpStr,value);
        }
        axisNode.appendChild(DHElement);
        QDomElement LimElement = xml->doc.createElement("LIM");
        for(int j = 0; j < limHeadList.size(); ++j){
            QString value = rbtLimModel->index(i,j,QModelIndex()).data().toString();
            LimElement.setAttribute(limHeadList[j],value);
        }
        axisNode.appendChild(LimElement);
    }
}

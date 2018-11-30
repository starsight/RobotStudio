#include "rsiconfig.h"
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

static QStringList RSITypes({"BOOL","UINT8","INT8","INT16",
"UINT16","INT32","UINT32","INT64","UINT64","DOUBLE"});

static QStringList RSIFBTypes({"PID","PIDN","SETOFFSET","DELAY","POSCORR","AXISCORR"});

static QHash<QString,QString> UserDefinedVar;

RSIconfig::RSIconfig(const QString& path, QWidget *parent) : QWidget(parent),projectPath(path)
{
    RSIComTypeLabel = new QLabel(tr("通信方式："),this);
    RSIComTypeComboBox = new QComboBox(this);
    RSIComTypeComboBox->addItem(tr("请选择"));
    RSIComTypeComboBox->addItem(tr("TCP"));
    RSIComTypeComboBox->addItem(tr("UDP"));

    RSIIPLabel = new QLabel(tr("IP地址："),this);
    RSIIPLineEdit = new QLineEdit(this);

    RSIPortLabel = new QLabel(tr("端口号："),this);
    RSIPortLineEdit = new QLineEdit(this);

    RSIAllowDelayLabel = new QLabel(tr("允许时延(ms)："),this);
    RSIAllowDelayLineEdit = new QLineEdit(this);

    generateXMLBtn = new QPushButton(tr("生成配置文件"),this);
    connect(generateXMLBtn,SIGNAL(clicked(bool)),this,SLOT(onGenerateXMLFile()));

    RSIconfigTabWidget = new QTabWidget(this);
    variableWidget = new RSIVariableWidget(RSIconfigTabWidget);
    fbWidget = new RSIFunctionBlockWidget(RSIconfigTabWidget);
    communicationWidget = new RSICommunicationWidget(RSIconfigTabWidget);

    RSIconfigTabWidget->addTab(variableWidget,tr("自定义变量配置"));
    RSIconfigTabWidget->addTab(fbWidget,tr("功能块变量配置"));
    RSIconfigTabWidget->addTab(communicationWidget,tr("通信参数配置"));

    RSIEditorLabel = new QLabel(tr("RSI程序编辑"));
    RSIProgramTextEdit = new QTextEdit(this);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(RSIComTypeLabel);
    topLayout->addWidget(RSIComTypeComboBox);
    topLayout->addStretch(1);
    topLayout->addWidget(RSIIPLabel);
    topLayout->addWidget(RSIIPLineEdit);
    topLayout->addStretch(1);
    topLayout->addWidget(RSIPortLabel);
    topLayout->addWidget(RSIPortLineEdit);
    topLayout->addStretch(1);
    topLayout->addWidget(RSIAllowDelayLabel);
    topLayout->addWidget(RSIAllowDelayLineEdit);
    topLayout->addWidget(generateXMLBtn);

    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(RSIconfigTabWidget);
    bottomRightLayout = new QGridLayout;
    bottomRightLayout->setSpacing(5);
    bottomRightLayout->addWidget(RSIEditorLabel,0,0,1,1);
    bottomRightLayout->addWidget(RSIProgramTextEdit,1,0,1,5);
    bottomLayout->addLayout(bottomRightLayout);
    bottomLayout->setStretchFactor(RSIconfigTabWidget,2);
    bottomLayout->setStretchFactor(bottomRightLayout,1);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(20);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    xml = nullptr;
}

void RSIconfig::onGenerateXMLFile()
{
    QDir tempPath;
    QString currentPath = tempPath.currentPath();
    tempPath.setCurrent(projectPath);
    if(xml == nullptr)
        xml = new XmlUntity("rsidemo.xml","Root");
    QDomElement rootElement = xml->doc.firstChildElement();
    QDomElement declareElement;
    if((declareElement = rootElement.firstChildElement(tr("DECLARE"))).isNull()){
        declareElement = xml->doc.createElement(tr("DECLARE"));
        rootElement.appendChild(declareElement);
    }
    while(declareElement.hasChildNodes())
    {
        QDomNode tempNode = declareElement.firstChild();
        declareElement.removeChild(tempNode);
    }
    QDomElement varRoot;
    variableWidget->generateVariablesNode(xml,varRoot);
    declareElement.appendChild(varRoot);
    QDomElement fbRoot;
    fbWidget->generateFunctionBlockXMLNode(xml,fbRoot);
    declareElement.appendChild(fbRoot);
    QDomElement communicateElement;
    if((communicateElement = rootElement.firstChildElement(tr("COMMUNICATION"))).isNull()){
        communicateElement = xml->doc.createElement(tr("COMMUNICATION"));
        rootElement.appendChild(communicateElement);
    }
    while(communicateElement.hasChildNodes())
    {
        QDomNode tempNode = communicateElement.firstChild();
        communicateElement.removeChild(tempNode);
    }
    QDomElement configElement = xml->doc.createElement(tr("CONFIG"));
    addXMLTextToElement(configElement,tr("COM_TYPE"),RSIComTypeComboBox->currentText());
    addXMLTextToElement(configElement,tr("IP"),RSIIPLineEdit->text());
    addXMLTextToElement(configElement,tr("PORT"),RSIPortLineEdit->text());
    addXMLTextToElement(configElement,tr("TOLERAT"),RSIAllowDelayLineEdit->text());
    communicateElement.appendChild(configElement);
    communicationWidget->generateCommunicationXMLNode(xml,communicateElement);
    xml->writeXmlDoc();

    // 生成RSI程序文件
    QFile* file = new QFile;
    file->setFileName("demo.rsi");
    if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate))
        QMessageBox::information(this,tr("文件创建失败"),tr("错误的工程名"));
    QTextStream stream(file);
    QString RSIProgram = RSIProgramTextEdit->toPlainText();
    stream << RSIProgram;
    file->close();
    tempPath.setCurrent(currentPath);
}

void RSIconfig::addXMLTextToElement(QDomElement& element, const QString& name, const QString& value)
{
    QDomElement textElement = xml->doc.createElement(name);
    QDomText text = xml->doc.createTextNode(value);
    textElement.appendChild(text);
    element.appendChild(textElement);
}

/*========================================== 自定义变量模块 ======================================**/
RSIVariableWidget::RSIVariableWidget(QWidget* parent):QWidget(parent)
{
    variableNameLabel = new QLabel(tr("变量名："),this);
    variableNameLineEdit = new QLineEdit(this);

    variableTypeLabel = new QLabel(tr("变量类型："),this);
    variableTypeComboBox = new QComboBox(this);
    variableTypeComboBox->addItems(RSITypes);

    variableInitValLabel = new QLabel(tr("变量初始值："),this);
    variableInitValLineEdit = new QLineEdit(this);

    addVariableBtn = new QPushButton(tr("添加变量"),this);
    removeVariabelBtn = new QPushButton(tr("删除变量"),this);
    connect(addVariableBtn,SIGNAL(clicked(bool)),this,SLOT(addVairableSlot()));
    connect(removeVariabelBtn,SIGNAL(clicked(bool)),this,SLOT(removeVariableSlot()));

    setupVariabelModelView();
    rightbottomLayout = new QGridLayout;
    rightbottomLayout->setMargin(10);
    rightbottomLayout->setSpacing(20);
    rightbottomLayout->addWidget(variableNameLabel,0,0);
    rightbottomLayout->addWidget(variableNameLineEdit,0,1);
    rightbottomLayout->addWidget(variableTypeLabel,1,0);
    rightbottomLayout->addWidget(variableTypeComboBox,1,1);
    rightbottomLayout->addWidget(variableInitValLabel,2,0);
    rightbottomLayout->addWidget(variableInitValLineEdit,2,1);
    rightbottomLayout->addWidget(addVariableBtn,3,0);
    rightbottomLayout->addWidget(removeVariabelBtn,3,1);
    rightbottomLayout->setColumnStretch(0,1);
    rightbottomLayout->setColumnStretch(1,1);

    rightLayout = new QVBoxLayout;
    rightLayout->addLayout(rightbottomLayout);
    rightLayout->setStretchFactor(rightbottomLayout,10);

    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(varTableView);
    mainLayout->addStretch(1);
    mainLayout->addLayout(rightLayout);
    mainLayout->setStretchFactor(varTableView,5);
    mainLayout->setStretchFactor(rightLayout,4);
}


void RSIVariableWidget::setupVariabelModelView()
{
    varTableModel = new QStandardItemModel(0,3,this);
    varTableModel->setHeaderData(0,Qt::Horizontal,tr("变量名"));
    varTableModel->setHeaderData(1,Qt::Horizontal,tr("变量类型"));
    varTableModel->setHeaderData(2,Qt::Horizontal,tr("变量初始值"));

    varTableView = new QTableView(this);
    varTableView->setModel(varTableModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(varTableModel);
    varTableView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),varTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
//    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(updateUserDefinedVariables()));
    connect(varTableModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(updateUserDefinedVariables()));
}

void RSIVariableWidget::addVairableSlot()
{
    QString varName = variableNameLineEdit->text();
    if(varNameList.indexOf(varName) != -1 || varName.isEmpty()){
        QMessageBox::critical(this,tr("Error"),tr("请输入正确的变量名！"));
        return;
    }
    varNameList << varName;
    int row = varTableModel->rowCount();
    varTableModel->insertRow(row,QModelIndex());
    QString varType = variableTypeComboBox->currentText();
    QString varInitVal = variableInitValLineEdit->text();
    varTableModel->setData(varTableModel->index(row,0,QModelIndex()),varName);
    varTableModel->setData(varTableModel->index(row,1,QModelIndex()),varType);
    varTableModel->setData(varTableModel->index(row,2,QModelIndex()),varInitVal);
    variableNameLineEdit->clear();
    variableTypeComboBox->setCurrentIndex(0);
    variableInitValLineEdit->clear();
}

void RSIVariableWidget::removeVariableSlot()
{
    int row = varTableView->currentIndex().row();
    if(row < 0)
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
    QString varName = varTableModel->data(varTableModel->index(row,0,QModelIndex())).toString();
    varTableModel->removeRow(row,QModelIndex());
    int index = varNameList.indexOf(varName);
    varNameList.removeAt(index);
}

void RSIVariableWidget::updateUserDefinedVariables()
{
    qDebug() << "rowcount = " << varTableModel->rowCount();
    UserDefinedVar.clear();
    for(int i = 0; i < varTableModel->rowCount(); ++i)
    {
        QString varName = varTableModel->data(varTableModel->index(i,0,QModelIndex())).toString();
        QString varType = varTableModel->data(varTableModel->index(i,1,QModelIndex())).toString();
        UserDefinedVar[varName] = varType;
    }
    qDebug() << "================ Variable ====================" << UserDefinedVar;
}

void RSIVariableWidget::generateVariablesNode(XmlUntity* xml, QDomElement& varRoot)
{
    varRoot = xml->doc.createElement(tr("VARIABLES"));
    for(int i = 0; i < varTableModel->rowCount(); ++i)
    {
        QDomElement varElement = xml->doc.createElement(tr("VAR"));
        QString varName = varTableModel->data(varTableModel->index(i,0,QModelIndex())).toString();
        QString varType = varTableModel->data(varTableModel->index(i,1,QModelIndex())).toString();
        QString varInitVal = varTableModel->data(varTableModel->index(i,2,QModelIndex())).toString();
        varElement.setAttribute("NAME",varName);
        varElement.setAttribute("TYPE",varType);
        if(!varInitVal.isEmpty()) varElement.setAttribute("INIT",varInitVal);
//        qDebug() << "======= generate variable ===============" << varElement;
        varRoot.appendChild(varElement);
    }
}

/*============================== 功能块模块 ======================================= */
RSIFunctionBlockWidget::RSIFunctionBlockWidget(QWidget* parent): QWidget(parent)
{
    fbTypeLabel = new QLabel(tr("功能块类型"),this);
    fbTypeComboBox = new QComboBox(this);
    fbTypeComboBox->addItems(RSIFBTypes);

    fbParametersWidget = new QStackedWidget(this);
    fbPID = new RSIFbPid(this);
    fbPIDN = new RSIFbPidN(this);
    fbSETOFFSET = new RSIFbSetOffset(this);
    fbDELAY = new RSIFbDelay(this);
    fbPOSCORR = new RSIFbPosCorr(this);
    fbAXISCORR = new RSIFbAxisCorr(this);
    fbParametersWidget->addWidget(fbPID);
    fbParametersWidget->addWidget(fbPIDN);
    fbParametersWidget->addWidget(fbSETOFFSET);
    fbParametersWidget->addWidget(fbDELAY);
    fbParametersWidget->addWidget(fbPOSCORR);
    fbParametersWidget->addWidget(fbAXISCORR);
    fbPtrs << fbPID << fbPIDN << fbSETOFFSET << fbDELAY << fbPOSCORR << fbAXISCORR;

    connect(fbTypeComboBox,SIGNAL(currentIndexChanged(int)),fbParametersWidget,SLOT(setCurrentIndex(int)));

    fbNameLabel = new QLabel(tr("功能块名："),this);
    fbNameLineEdit = new QLineEdit(this);
    addFBBtn = new QPushButton(tr("添加功能块"),this);
    removeFBBtn = new QPushButton(tr("删除功能块"),this);
    connect(addFBBtn,SIGNAL(clicked(bool)),this,SLOT(addFBSlot()));
    connect(removeFBBtn,SIGNAL(clicked(bool)),this,SLOT(removeFBSlot()));
    setupFBModelView();
    rightTopLayout = new QHBoxLayout;
    rightTopLayout->addWidget(fbTypeLabel);
    rightTopLayout->addWidget(fbTypeComboBox);
    rightBottomLayout = new QHBoxLayout;
    rightBottomLayout->addWidget(fbNameLabel);
    rightBottomLayout->addWidget(fbNameLineEdit);
    rightBottomLayout->addWidget(addFBBtn);
    rightBottomLayout->addWidget(removeFBBtn);

    rightLayout = new QVBoxLayout;
    rightLayout->setMargin(10);
    rightLayout->setSpacing(10);
    rightLayout->addLayout(rightTopLayout);
    rightLayout->addWidget(fbParametersWidget);
    rightLayout->addLayout(rightBottomLayout);

    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(fbTableView);
    mainLayout->addLayout(rightLayout);
    mainLayout->setStretchFactor(fbTableView,1);
    mainLayout->setStretchFactor(rightLayout,1);
}


void RSIFunctionBlockWidget::setupFBModelView()
{
    fbTableModel = new QStandardItemModel(0,2,this);
    fbTableModel->setHeaderData(0,Qt::Horizontal,tr("功能块名"));
    fbTableModel->setHeaderData(1,Qt::Horizontal,tr("功能块类型"));

    fbTableView = new QTableView(this);
    fbTableView->setModel(fbTableModel);
    QItemSelectionModel* selectionModel = new QItemSelectionModel(fbTableModel);
    fbTableView->setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),fbTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

void RSIFunctionBlockWidget::addFBSlot()
{
    QString fbName = fbNameLineEdit->text();
    if(fbNameList.indexOf(fbName) != -1 || fbName.isEmpty()){
        QMessageBox::critical(this,tr("Error"),tr("请输入正确的功能块名！"));
        return;
    }
    fbNameList << fbName;
    int row = fbTableModel->rowCount();
    fbTableModel->insertRow(row,QModelIndex());
    QString fbType = fbTypeComboBox->currentText();
    fbTableModel->setData(fbTableModel->index(row,0,QModelIndex()),fbName);
    fbTableModel->setData(fbTableModel->index(row,1,QModelIndex()),fbType);
    // 将功能块信息添加到fbInfos中
    QHash<QString,QString> oneFbInfo;
    fbPtrs[fbTypeComboBox->currentIndex()]->getFbInfo(oneFbInfo);
    fbInfos[fbName] = oneFbInfo;
    fbNameLineEdit->clear();
    fbTypeComboBox->setCurrentIndex(0);
}

void RSIFunctionBlockWidget::removeFBSlot()
{
    int row = fbTableView->currentIndex().row();
    if(row < 0)
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行！"));
    QString fbName = fbTableModel->data(fbTableModel->index(row,0,QModelIndex())).toString();
    fbTableModel->removeRow(row,QModelIndex());
    fbInfos.remove(fbName);
    int index = fbNameList.indexOf(fbName);
    fbNameList.removeAt(index);
}

void RSIFunctionBlockWidget::generateFunctionBlockXMLNode(XmlUntity* xml, QDomElement& fbRoot)
{
    fbRoot = xml->doc.createElement(tr("FUNBLOCK"));
    QHash<QString,QHash<QString,QString>>::iterator it = fbInfos.begin();
    for(; it != fbInfos.end(); ++it)
    {
        QDomElement oneFbElement = xml->doc.createElement(tr("FB"));
        QString fbName = it.key();
        oneFbElement.setAttribute("NAME",fbName);
        QHash<QString,QString>::iterator subIt = fbInfos[fbName].begin();
        for(; subIt != fbInfos[fbName].end(); ++subIt)
            oneFbElement.setAttribute(subIt.key(),subIt.value());
        fbRoot.appendChild(oneFbElement);
    }
}
/*==================================== 各功能块小部件 =========================================*/
// PID功能块
RSIFbPid::RSIFbPid(QWidget* parent):RSIFbBase(parent)
{
    KpLabel = new QLabel(tr("Kp"),this);
    KpLabel->setAlignment(Qt::AlignCenter);
    KpLineEdit = new QLineEdit(this);
    KdLabel = new QLabel(tr("Kd"),this);
    KdLabel->setAlignment(Qt::AlignCenter);
    KdLineEdit = new QLineEdit(this);
    KiLabel = new QLabel(tr("Ki"),this);
    KiLabel->setAlignment(Qt::AlignCenter);
    KiLineEdit = new QLineEdit(this);
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(KpLabel,0,0);
    mainLayout->addWidget(KpLineEdit,0,1);
    mainLayout->addWidget(KdLabel,1,0);
    mainLayout->addWidget(KdLineEdit,1,1);
    mainLayout->addWidget(KiLabel,2,0);
    mainLayout->addWidget(KiLineEdit,2,1);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);
}

void RSIFbPid::getFbInfo(QHash<QString,QString>& oneFbInfo)
{
    oneFbInfo["TYPE"] = "PID";
    oneFbInfo["Kp"] = KpLineEdit->text();
    oneFbInfo["Kd"] = KdLineEdit->text();
    oneFbInfo["Ki"] = KiLineEdit->text();
}

// PIDN功能块
RSIFbPidN::RSIFbPidN(QWidget* parent):RSIFbBase(parent)
{
    KpLabel = new QLabel(tr("Kp"),this);
    KpLabel->setAlignment(Qt::AlignCenter);
    KpLineEdit = new QLineEdit(this);
    KdLabel = new QLabel(tr("Kd"),this);
    KdLabel->setAlignment(Qt::AlignCenter);
    KdLineEdit = new QLineEdit(this);
    KiLabel = new QLabel(tr("Ki"),this);
    KiLabel->setAlignment(Qt::AlignCenter);
    KiLineEdit = new QLineEdit(this);
    NLabel = new QLabel(tr("N"),this);
    NLabel->setAlignment(Qt::AlignCenter);
    NLineEdit = new QLineEdit(this);
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(KpLabel,0,0);
    mainLayout->addWidget(KpLineEdit,0,1);
    mainLayout->addWidget(KdLabel,1,0);
    mainLayout->addWidget(KdLineEdit,1,1);
    mainLayout->addWidget(KiLabel,2,0);
    mainLayout->addWidget(KiLineEdit,2,1);
    mainLayout->addWidget(NLabel,3,0);
    mainLayout->addWidget(NLineEdit,3,1);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);
}

void RSIFbPidN::getFbInfo(QHash<QString, QString>& oneFbInfo)
{
    oneFbInfo["TYPE"] = "PIDN";
    oneFbInfo["Kp"] = KpLineEdit->text();
    oneFbInfo["Kd"] = KdLineEdit->text();
    oneFbInfo["Ki"] = KiLineEdit->text();
    oneFbInfo["N"] = NLineEdit->text();
}

// SETOFFSET 功能块
RSIFbSetOffset::RSIFbSetOffset(QWidget* parent):RSIFbBase(parent)
{
    offsetCntLabel = new QLabel(tr("需调整零偏变量个数："),this);
    offsetCntLineEdit = new QLineEdit(this);
    offsetSampleCntLabel = new QLabel(tr("零偏采样次数："),this);
    offsetSampleCntLineEdit = new QLineEdit(this);
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(offsetCntLabel,0,0);
    mainLayout->addWidget(offsetCntLineEdit,0,1);
    mainLayout->addWidget(offsetSampleCntLabel,1,0);
    mainLayout->addWidget(offsetSampleCntLineEdit,1,1);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);
}

void RSIFbSetOffset::getFbInfo(QHash<QString, QString>& oneFbInfo)
{
    oneFbInfo["TYPE"] = "SETOFFSET";
    oneFbInfo["OFFSETCNT"] = offsetCntLineEdit->text();
    oneFbInfo["FILTERCFG"] = offsetSampleCntLineEdit->text();
}

// DELAY 功能块
RSIFbDelay:: RSIFbDelay(QWidget* parent):RSIFbBase(parent)
{
    delayLabel = new QLabel(tr("延迟时间（ms）："),this);
    delayLineEdit = new QLineEdit(this);
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(delayLabel,0,0);
    mainLayout->addWidget(delayLineEdit,0,1);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);
}

void RSIFbDelay::getFbInfo(QHash<QString, QString>& oneFbInfo)
{
    oneFbInfo["TYPE"] = oneFbInfo["DELAY"];
    oneFbInfo["T"] = delayLineEdit->text();
}

// POSCORR 功能块
RSIFbPosCorr::RSIFbPosCorr(QWidget* parent):RSIFbBase(parent)
{
    corrTypeLabel = new QLabel(tr("CorrType"),this);
    corrTypeLineEdit = new QLineEdit(this);
    refCorrSysLabel = new QLabel(tr("RefCorrSys"),this);
    refCorrSysLineEdit = new QLineEdit(this);
    lowerLimXLabel = new QLabel(tr("LowerLimX"),this);
    lowerLimXLineEdit = new QLineEdit(this);
    lowerLimYLabel = new QLabel(tr("LowerLimY"),this);
    lowerLimYLineEdit = new QLineEdit(this);
    lowerLimZLabel = new QLabel(tr("LowerLimZ"),this);
    lowerLimZLineEdit = new QLineEdit(this);
    upperLimXLabel = new QLabel(tr("UpperLimX"),this);
    upperLimXLineEdit = new QLineEdit(this);
    upperLimYLabel = new QLabel(tr("UpperLimY"),this);
    upperLimYLineEdit = new QLineEdit(this);
    upperLimZLabel = new QLabel(tr("UpperLimZ"),this);
    upperLimZLineEdit = new QLineEdit(this);
    maxRotAngleLabel = new QLabel(tr("MaxRotAngle"),this);
    maxRotAngleLineEdit = new QLineEdit(this);
    overTransLimLabel = new QLabel(tr("OverTransLim"),this);
    overTransLimLineEdit = new QLineEdit(this);
    overRotAngleLabel = new QLabel(tr("OverRotAngle"),this);
    overRotAngleLineEdit = new QLineEdit(this);

    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(corrTypeLabel,0,0);
    mainLayout->addWidget(corrTypeLineEdit,0,1);
    mainLayout->addWidget(refCorrSysLabel,0,2);
    mainLayout->addWidget(refCorrSysLineEdit,0,3);
    mainLayout->addWidget(lowerLimXLabel,1,0);
    mainLayout->addWidget(lowerLimXLineEdit,1,1);
    mainLayout->addWidget(upperLimXLabel,1,2);
    mainLayout->addWidget(upperLimXLineEdit,1,3);
    mainLayout->addWidget(lowerLimYLabel,2,0);
    mainLayout->addWidget(lowerLimYLineEdit,2,1);
    mainLayout->addWidget(upperLimYLabel,2,2);
    mainLayout->addWidget(upperLimYLineEdit,2,3);
    mainLayout->addWidget(lowerLimZLabel,3,0);
    mainLayout->addWidget(lowerLimZLineEdit,3,1);
    mainLayout->addWidget(upperLimZLabel,3,2);
    mainLayout->addWidget(upperLimZLineEdit,3,3);
    mainLayout->addWidget(maxRotAngleLabel,4,0);
    mainLayout->addWidget(maxRotAngleLineEdit,4,1);
    mainLayout->addWidget(overTransLimLabel,4,2);
    mainLayout->addWidget(overTransLimLineEdit,4,3);
    mainLayout->addWidget(overRotAngleLabel,5,0);
    mainLayout->addWidget(overRotAngleLineEdit,5,1);
}

void RSIFbPosCorr::getFbInfo(QHash<QString, QString>& oneFbInfo)
{
    oneFbInfo["TYPE"] = "POSCORR";
    oneFbInfo["CorrType"] = corrTypeLineEdit->text();
    oneFbInfo["RefCorrSys"] = refCorrSysLineEdit->text();
    oneFbInfo["LowerLimX"] = lowerLimXLineEdit->text();
    oneFbInfo["LowerLimY"] = lowerLimYLineEdit->text();
    oneFbInfo["LowerLimZ"] = lowerLimZLineEdit->text();
    oneFbInfo["UpperLimX"] = upperLimXLineEdit->text();
    oneFbInfo["UpperLimY"] = upperLimYLineEdit->text();
    oneFbInfo["UpperLimZ"] = upperLimZLineEdit->text();
    oneFbInfo["MaxRotAngle"] = maxRotAngleLineEdit->text();
    oneFbInfo["OverTransLim"] = overTransLimLineEdit->text();
    oneFbInfo["OverRotAngle"] = overRotAngleLineEdit->text();
}

// AXISCORR 功能块
RSIFbAxisCorr::RSIFbAxisCorr(QWidget* parent):RSIFbBase(parent)
{
    lowerLimA1Label = new QLabel(tr("LowerLimA1"),this);
    lowerLimA1LineEdit = new QLineEdit(this);
    lowerLimA2Label = new QLabel(tr("LowerLimA2"),this);
    lowerLimA2LineEdit = new QLineEdit(this);
    lowerLimA3Label = new QLabel(tr("LowerLimA3"),this);
    lowerLimA3LineEdit = new QLineEdit(this);
    lowerLimA4Label = new QLabel(tr("LowerLimA4"),this);
    lowerLimA4LineEdit = new QLineEdit(this);
    lowerLimA5Label = new QLabel(tr("LowerLimA5"),this);
    lowerLimA5LineEdit = new QLineEdit(this);
    lowerLimA6Label = new QLabel(tr("LowerLimA6"),this);
    lowerLimA6LineEdit = new QLineEdit(this);

    upperLimA1Label = new QLabel(tr("UpperLimA1"),this);
    upperLimA1LineEdit = new QLineEdit(this);
    upperLimA2Label = new QLabel(tr("UpperLimA2"),this);
    upperLimA2LineEdit = new QLineEdit(this);
    upperLimA3Label = new QLabel(tr("UpperLimA3"),this);
    upperLimA3LineEdit = new QLineEdit(this);
    upperLimA4Label = new QLabel(tr("UpperLimA4"),this);
    upperLimA4LineEdit = new QLineEdit(this);
    upperLimA5Label = new QLabel(tr("UpperLimA5"),this);
    upperLimA5LineEdit = new QLineEdit(this);
    upperLimA6Label = new QLabel(tr("UpperLimA6"),this);
    upperLimA6LineEdit = new QLineEdit(this);

    overLimA1Label = new QLabel(tr("OverLimA1"),this);
    overLimA1LineEdit = new QLineEdit(this);
    overLimA2Label = new QLabel(tr("OverLimA2"),this);
    overLimA2LineEdit = new QLineEdit(this);
    overLimA3Label = new QLabel(tr("OverLimA3"),this);
    overLimA3LineEdit = new QLineEdit(this);
    overLimA4Label = new QLabel(tr("OverLimA4"),this);
    overLimA4LineEdit = new QLineEdit(this);
    overLimA5Label = new QLabel(tr("OverLimA5"),this);
    overLimA5LineEdit = new QLineEdit(this);
    overLimA6Label = new QLabel(tr("OverLimA6"),this);
    overLimA6LineEdit = new QLineEdit(this);

    corrTypeLabel = new QLabel(tr("CorrType"),this);
    corrTypeLineEdit = new QLineEdit(this);

    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(corrTypeLabel,0,0);
    mainLayout->addWidget(corrTypeLineEdit,0,1);
    mainLayout->addWidget(lowerLimA1Label,1,0);
    mainLayout->addWidget(lowerLimA1LineEdit,1,1);
    mainLayout->addWidget(upperLimA1Label,1,2);
    mainLayout->addWidget(upperLimA1LineEdit,1,3);
    mainLayout->addWidget(lowerLimA2Label,2,0);
    mainLayout->addWidget(lowerLimA2LineEdit,2,1);
    mainLayout->addWidget(upperLimA2Label,2,2);
    mainLayout->addWidget(upperLimA2LineEdit,2,3);
    mainLayout->addWidget(lowerLimA3Label,3,0);
    mainLayout->addWidget(lowerLimA3LineEdit,3,1);
    mainLayout->addWidget(upperLimA3Label,3,2);
    mainLayout->addWidget(upperLimA3LineEdit,3,3);
    mainLayout->addWidget(lowerLimA4Label,4,0);
    mainLayout->addWidget(lowerLimA4LineEdit,4,1);
    mainLayout->addWidget(upperLimA4Label,4,2);
    mainLayout->addWidget(upperLimA4LineEdit,4,3);
    mainLayout->addWidget(lowerLimA5Label,5,0);
    mainLayout->addWidget(lowerLimA5LineEdit,5,1);
    mainLayout->addWidget(upperLimA5Label,5,2);
    mainLayout->addWidget(upperLimA5LineEdit,5,3);
    mainLayout->addWidget(lowerLimA6Label,6,0);
    mainLayout->addWidget(lowerLimA6LineEdit,6,1);
    mainLayout->addWidget(upperLimA6Label,6,2);
    mainLayout->addWidget(upperLimA6LineEdit,6,3);
    mainLayout->addWidget(overLimA1Label,7,0);
    mainLayout->addWidget(overLimA1LineEdit,7,1);
    mainLayout->addWidget(overLimA2Label,7,2);
    mainLayout->addWidget(overLimA2LineEdit,7,3);
    mainLayout->addWidget(overLimA3Label,8,0);
    mainLayout->addWidget(overLimA3LineEdit,8,1);
    mainLayout->addWidget(overLimA4Label,8,2);
    mainLayout->addWidget(overLimA4LineEdit,8,3);
    mainLayout->addWidget(overLimA5Label,9,0);
    mainLayout->addWidget(overLimA5LineEdit,9,1);
    mainLayout->addWidget(overLimA6Label,9,2);
    mainLayout->addWidget(overLimA6LineEdit,9,3);
}


void RSIFbAxisCorr::getFbInfo(QHash<QString, QString>& oneInfo)
{
    oneInfo["TYPE"] = "AXISCORR";
    oneInfo["LowerLimA1"] = lowerLimA1LineEdit->text();
    oneInfo["LowerLimA2"] = lowerLimA2LineEdit->text();
    oneInfo["LowerLimA3"] = lowerLimA3LineEdit->text();
    oneInfo["LowerLimA4"] = lowerLimA4LineEdit->text();
    oneInfo["LowerLimA5"] = lowerLimA5LineEdit->text();
    oneInfo["LowerLimA6"] = lowerLimA6LineEdit->text();
    oneInfo["UpperLimA1"] = upperLimA1LineEdit->text();
    oneInfo["UpperLimA2"] = upperLimA2LineEdit->text();
    oneInfo["UpperLimA3"] = upperLimA3LineEdit->text();
    oneInfo["UpperLimA4"] = upperLimA4LineEdit->text();
    oneInfo["UpperLimA5"] = upperLimA5LineEdit->text();
    oneInfo["UpperLimA6"] = upperLimA6LineEdit->text();
    oneInfo["OverLimA1"] = overLimA1LineEdit->text();
    oneInfo["OverLimA2"] = overLimA2LineEdit->text();
    oneInfo["OverLimA3"] = overLimA3LineEdit->text();
    oneInfo["OverLimA4"] = overLimA4LineEdit->text();
    oneInfo["OverLimA5"] = overLimA5LineEdit->text();
    oneInfo["OverLimA6"] = overLimA6LineEdit->text();
    oneInfo["CorrType"] = corrTypeLineEdit->text();
}


/*=========================================== 通信参数配置模块 ===============================*/
RSICommunicationWidget::RSICommunicationWidget(QWidget* parent):QWidget(parent)
{
    RSISensorTypeLabel = new QLabel(tr("传感器类型："),this);
    RSISensorTypeComboBox = new QComboBox(this);
    RSISensorTypeComboBox->addItem(tr("普通传感器"));
    RSISensorTypeComboBox->addItem(tr("终端设备"));
    RSISensorTrigTypeLabel = new QLabel(tr("传感器触发方式："),this);
    RSISensorTrigTypeComboBox = new QComboBox(this);
    RSISensorTrigTypeComboBox->addItem(tr("周期式"));
    RSISensorTrigTypeComboBox->addItem(tr("触发式"));
    RSICommunicationXMLRootNodeLabel = new QLabel(tr("通信XML根节点"),this);
    RSICommunicationXMLRootNodeLineEdit = new QLineEdit(this);
    RSISensorStackWidget = new QStackedWidget(this);
    sensorWidget = new RSINormalSensorWidget(RSISensorStackWidget);
    terminalWidget = new RSITerminalWidget(RSISensorStackWidget);
    RSISensorStackWidget->addWidget(sensorWidget);
    RSISensorStackWidget->addWidget(terminalWidget);
    connect(RSISensorTypeComboBox,SIGNAL(currentIndexChanged(int)),RSISensorStackWidget,SLOT(setCurrentIndex(int)));
    connect(RSISensorTrigTypeComboBox,SIGNAL(currentIndexChanged(int)),sensorWidget,SLOT(sensorTrigSwitched(int)));

    topLayout = new QHBoxLayout;
    topLayout->setSpacing(5);
    topLayout->addWidget(RSISensorTypeLabel);
    topLayout->addWidget(RSISensorTypeComboBox);
    topLayout->addStretch(1);
    topLayout->addWidget(RSISensorTrigTypeLabel);
    topLayout->addWidget(RSISensorTrigTypeComboBox);
    topLayout->addStretch(1);
    topLayout->addWidget(RSICommunicationXMLRootNodeLabel);
    topLayout->addWidget(RSICommunicationXMLRootNodeLineEdit);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(RSISensorStackWidget);
}

void RSICommunicationWidget::generateCommunicationXMLNode(XmlUntity* xml, QDomElement& communicateElement)
{
    QDomElement configElement = communicateElement.firstChildElement(tr("CONFIG"));
    QDomElement rootNameElement = xml->doc.createElement(tr("ROOTNAME"));
    QDomText rootNameText = xml->doc.createTextNode(RSICommunicationXMLRootNodeLineEdit->text());
    rootNameElement.appendChild(rootNameText);
    configElement.appendChild(rootNameElement);
    int sensorTypeIndex = RSISensorTypeComboBox->currentIndex();
    QString sensorTypeName = sensorTypeIndex ? "TERMINAL":"SENSOR";
    int sensorTrigTypeIndex = RSISensorTrigTypeComboBox->currentIndex();
    QString sensorTrigTypeName = sensorTrigTypeIndex ? "TRIG":"CYCLIC";
    QDomElement sensorTypeElement = xml->doc.createElement(tr("SENSORTYPE"));
    QDomText sensorTypeText = xml->doc.createTextNode(sensorTypeName);
    sensorTypeElement.appendChild(sensorTypeText);
    configElement.appendChild(sensorTypeElement);
    QDomElement sensorTrigElement = xml->doc.createElement(tr("SENSORMODE"));
    QDomText sensorTrigText = xml->doc.createTextNode(sensorTrigTypeName);
    sensorTrigElement.appendChild(sensorTrigText);
    configElement.appendChild(sensorTrigElement);
    if(sensorTypeIndex == 0) sensorWidget->generateNormalSensorXMLNode(xml,communicateElement);
    else                     terminalWidget->generateTerminalXMLNode(xml,communicateElement);
}


// 普通传感器模块
RSINormalSensorWidget::RSINormalSensorWidget(QWidget* parent):QWidget(parent)
{
    leftLayoutInit();
    rightUpperLayoutInit();
    rightDownLayoutInit();
    rightLayout = new QVBoxLayout;
    rightLayout->addLayout(rightUpperLayout);
    rightLayout->addLayout(rightDownLayout);
    rightLayout->setStretchFactor(rightUpperLayout,2);
    rightLayout->setStretchFactor(rightDownLayout,3);
    mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->addLayout(leftLayout);
//    mainLayout->addStretch(1);
    mainLayout->addLayout(rightLayout);
}

void RSINormalSensorWidget::leftLayoutInit()
{
    sensorSectionTypeLabel = new QLabel(tr("字段类型："),this);
    sensorSectionTypeComboBox = new QComboBox(this);
    sensorSectionTypeComboBox->addItems(RSITypes);
    addSensorSectionBtn = new QPushButton(tr("添加配置字段"),this);
    removeSensorSectionBtn = new QPushButton(tr("删除配置字段"),this);
    setSensorCfgModelView();
    addCfgContentBtn = new QPushButton(tr("添加配置命令"),this);
    removeCfgContentBtn = new QPushButton(tr("删除配置命令"),this);
    leftTopLayout = new QHBoxLayout;
    leftTopLayout->addStretch(1);
    leftTopLayout->addWidget(sensorSectionTypeLabel);
    leftTopLayout->addWidget(sensorSectionTypeComboBox);
    leftTopLayout->addWidget(addSensorSectionBtn);
    leftTopLayout->addWidget(removeSensorSectionBtn);
    leftTopLayout->addStretch(1);
    leftBottomLayout = new QHBoxLayout;
    leftBottomLayout->addStretch(1);
    leftBottomLayout->addWidget(addCfgContentBtn);
    leftBottomLayout->addWidget(removeCfgContentBtn);
    leftBottomLayout->addStretch(1);
    leftLayout = new QVBoxLayout;
    leftLayout->addLayout(leftTopLayout);
    leftLayout->addWidget(sensorCfgTableView);
    leftLayout->addLayout(leftBottomLayout);

    connect(addSensorSectionBtn,SIGNAL(clicked(bool)),this,SLOT(onAddSensorConfigSection()));
    connect(removeSensorSectionBtn,SIGNAL(clicked(bool)),this,SLOT(onRemoveSensorConfigSection()));
    connect(addCfgContentBtn,SIGNAL(clicked(bool)),this,SLOT(onAddSensorConfigContent()));
    connect(removeCfgContentBtn,SIGNAL(clicked(bool)),this,SLOT(onRemoveSensorConfigContent()));
}

void RSINormalSensorWidget::rightUpperLayoutInit()
{
    sendSectionTypeLabel = new QLabel(tr("字段类型："),this);
    sendSectionTypeComboBox = new QComboBox(this);
    sendSectionTypeComboBox->addItems(RSITypes);
    addSendSectionBtn = new QPushButton(tr("添加发送字段"),this);
    removeSendSectionBtn = new QPushButton(tr("删除发送字段"),this);
    setSensorSendModelView();
    addSendContentBtn = new QPushButton(tr("添加发送命令"),this);
    removeSendContentBtn = new QPushButton(tr("删除发送命令"),this);
    rightUpperTopLayout = new QHBoxLayout;
    rightUpperTopLayout->addStretch(1);
    rightUpperTopLayout->addWidget(sendSectionTypeLabel);
    rightUpperTopLayout->addWidget(sendSectionTypeComboBox);
    rightUpperTopLayout->addWidget(addSendSectionBtn);
    rightUpperTopLayout->addWidget(removeSendSectionBtn);
    rightUpperTopLayout->addStretch(1);
    rightUpperBottomLayout = new QHBoxLayout;
    rightUpperBottomLayout->addStretch(1);
    rightUpperBottomLayout->addWidget(addSendContentBtn);
    rightUpperBottomLayout->addWidget(removeSendContentBtn);
    rightUpperBottomLayout->addStretch(1);
    rightUpperLayout = new QVBoxLayout;
    rightUpperLayout->addLayout(rightUpperTopLayout);
    rightUpperLayout->addWidget(sensorSendTableView);
    rightUpperLayout->addLayout(rightUpperBottomLayout);

    connect(addSendSectionBtn,SIGNAL(clicked(bool)),this,SLOT(onAddSensorSendSection()));
    connect(removeSendSectionBtn,SIGNAL(clicked(bool)),this,SLOT(onRemoveSensorSendSection()));
    connect(addSendContentBtn,SIGNAL(clicked(bool)),this,SLOT(onAddSensorSendContent()));
    connect(removeSendContentBtn,SIGNAL(clicked(bool)),this,SLOT(onRevmoveSensorSendContent()));
}

void RSINormalSensorWidget::rightDownLayoutInit()
{
    setSensorRecvModelView();
    sensorRecvOffBytesLabel = new QLabel(tr("接收数据起始偏移（单位byte）:"));
    sensorRecvOffBytesLineEdit = new QLineEdit(this);
    addRecvCfgBtn = new QPushButton(tr("添加接收配置"),this);
    removeRecvCfgBtn = new QPushButton(tr("删除接收配置"),this);
    QList<QVariant> varTypes;
    for(int i = 0; i < RSITypes.length(); ++i)
        varTypes << RSITypes[i];
    varTypeDelegate = new ComboDelegate(varTypes,this);
    sensorRecvTableView->setItemDelegateForColumn(0,varTypeDelegate);
    QList<QVariant> varNameEmpty;
    varNameDelegate = new ComboDelegate(varNameEmpty,this);
    sensorRecvTableView->setItemDelegateForColumn(2,varNameDelegate);

    rightDownBottomLayout = new QHBoxLayout;
//    rightDownBottomLayout->addStretch(1);
    rightDownBottomLayout->addWidget(sensorRecvOffBytesLabel);
    rightDownBottomLayout->addWidget(sensorRecvOffBytesLineEdit);
    rightDownBottomLayout->addWidget(addRecvCfgBtn);
    rightDownBottomLayout->addWidget(removeRecvCfgBtn);
//    rightDownBottomLayout->addStretch(1);
    rightDownLayout = new QVBoxLayout;
    rightDownLayout->addWidget(sensorRecvTableView);
    rightDownLayout->addLayout(rightDownBottomLayout);

    connect(addRecvCfgBtn,SIGNAL(clicked(bool)),this,SLOT(onAddSensorRecvConfig()));
    connect(removeRecvCfgBtn,SIGNAL(clicked(bool)),this,SLOT(onRemoveSensorRecvConfig()));
}

void RSINormalSensorWidget::setSensorCfgModelView()
{
    sensorCfgTableModel = new QStandardItemModel(0,0,this);

    sensorCfgTableView = new QTableView(this);
    sensorCfgTableView->setModel(sensorCfgTableModel);
    QItemSelectionModel* selectionModel = new QItemSelectionModel(sensorCfgTableModel);
    sensorCfgTableView->setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),sensorCfgTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}


void RSINormalSensorWidget::setSensorSendModelView()
{
    sensorSendTableModel = new QStandardItemModel(0,0,this);

    sensorSendTableView = new QTableView(this);
    sensorSendTableView->setModel(sensorSendTableModel);
    QItemSelectionModel* selectionModel = new QItemSelectionModel(sensorSendTableModel);
    sensorSendTableView->setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),sensorSendTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

void RSINormalSensorWidget::setSensorRecvModelView()
{
    sensorRecvTableModel = new QStandardItemModel(0,3,this);
    sensorRecvTableModel->setHeaderData(0,Qt::Horizontal,tr("类型"));
    sensorRecvTableModel->setHeaderData(1,Qt::Horizontal,tr("转换比率"));
    sensorRecvTableModel->setHeaderData(2,Qt::Horizontal,tr("变量"));

    sensorRecvTableView = new QTableView(this);
    sensorRecvTableView->setModel(sensorRecvTableModel);
    QItemSelectionModel* selectionModel = new QItemSelectionModel(sensorRecvTableModel);
    sensorRecvTableView->setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),sensorRecvTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}


// 普通传感器产生的XML文件节点
void RSINormalSensorWidget::generateNormalSensorXMLNode(XmlUntity* xml, QDomElement& communicateElement)
{
    QDomElement configElement = communicateElement.firstChildElement("CONFIG");
    int cfgCmdCnt = sensorCfgTableModel->rowCount();
    QDomElement cfgCntElement = xml->doc.createElement(tr("CFGCMDCNT"));
    QDomText cfgCntText = xml->doc.createTextNode(QString::number(cfgCmdCnt));
    cfgCntElement.appendChild(cfgCntText);
    configElement.appendChild(cfgCntElement);
    QDomElement recvOffElement = xml->doc.createElement(tr("RECVOFFSET"));
    QDomText recvOffText = xml->doc.createTextNode(sensorRecvOffBytesLineEdit->text());
    recvOffElement.appendChild(recvOffText);
    configElement.appendChild(recvOffElement);

    // 添加配置类型节点
    QDomElement typecfgElement = xml->doc.createElement(tr("TYPECFG"));
    QDomElement cfgTypeElement = xml->doc.createElement(tr("CFGTYPE"));
    for(int i = 0; i < sensorCfgTableModel->columnCount(); ++i)
    {
        QString tableHeader = sensorCfgTableModel->headerData(i,Qt::Horizontal).toString();
        cfgTypeElement.setAttribute("SECTION"+tableHeader.section("::",1,1),tableHeader.section("::",2,2));
    }
    typecfgElement.appendChild(cfgTypeElement);
    if(sensorSendTableView->isEnabled())
    {
        QDomElement sendTypeElement = xml->doc.createElement(tr("SENDTYPE"));
        for(int i = 0; i < sensorSendTableModel->columnCount(); ++i)
        {
            QString tableHeader = sensorSendTableModel->headerData(i,Qt::Horizontal).toString();
            sendTypeElement.setAttribute("SECTION"+tableHeader.section("::",1,1),tableHeader.section("::",2,2));
        }
        typecfgElement.appendChild(sendTypeElement);
    }
    communicateElement.appendChild(typecfgElement);

    // 添加配置命令节点
    QDomElement cfgPackagesElement = xml->doc.createElement(tr("CFGPACKAGES"));
    for(int i = 0; i < sensorCfgTableModel->rowCount(); ++i)
    {
        QDomElement onePackageElement = xml->doc.createElement(tr("PACKAGE"));
        for(int j = 0; j < sensorCfgTableModel->columnCount(); ++j)
        {
            QString data = sensorCfgTableModel->data(sensorCfgTableModel->index(i,j,QModelIndex())).toString();
            onePackageElement.setAttribute(tr("SECTION")+QString::number(j),data);
        }
        cfgPackagesElement.appendChild(onePackageElement);
    }
    communicateElement.appendChild(cfgPackagesElement);

    // 添加周期触发传感器节点
    if(sensorSendTableView->isEnabled())
    {
        QDomElement sendElement = xml->doc.createElement(tr("SEND"));
        for(int i = 0; i < sensorSendTableModel->rowCount(); ++i){
            QDomElement onePackageElement = xml->doc.createElement(tr("PACKAGE"));
            for(int j = 0; j < sensorSendTableModel->columnCount(); ++j){
                QString data = sensorSendTableModel->data(sensorSendTableModel->index(i,j,QModelIndex())).toString();
                onePackageElement.setAttribute(tr("SECTION")+QString::number(j),data);
            }
            sendElement.appendChild(onePackageElement);
        }
        communicateElement.appendChild(sendElement);
    }

    // 添加接收传感器信息节点
    QDomElement recvElement = xml->doc.createElement(tr("RECEIVE"));
    QDomElement recvPackageElement = xml->doc.createElement(tr("PACKAGE"));
    for(int i = 0; i < sensorRecvTableModel->rowCount(); ++i)
    {
        QDomElement element = xml->doc.createElement(tr("ELEMENT"));
        QString type = sensorRecvTableModel->data(sensorRecvTableModel->index(i,0,QModelIndex())).toString();
        QString ratio = sensorRecvTableModel->data(sensorRecvTableModel->index(i,1,QModelIndex())).toString();
        QString variable = sensorRecvTableModel->data(sensorRecvTableModel->index(i,2,QModelIndex())).toString();
        element.setAttribute("TYPE",type);
        element.setAttribute("RATIO",ratio);
        element.setAttribute("VAR",variable);
        recvPackageElement.appendChild(element);
    }
    recvElement.appendChild(recvPackageElement);
    communicateElement.appendChild(recvElement);
}


/* 界面槽函数 */
/*===================== 传感器配置槽函数 ==========================*/
void RSINormalSensorWidget::onAddSensorConfigSection()
{
    QString type = sensorSectionTypeComboBox->currentText();
    int col = sensorCfgTableModel->columnCount();
    sensorCfgTableModel->insertColumn(col,QModelIndex());
    QString header = "Section::" + QString::number(col)+ "::" + type;
    sensorCfgTableModel->setHeaderData(col,Qt::Horizontal,header);
}

void RSINormalSensorWidget::onRemoveSensorConfigSection()
{
    int col = sensorCfgTableView->currentIndex().column();
    if(col < 0){
        col = sensorCfgTableModel->columnCount();
        if(col > 0)
            sensorCfgTableModel->removeColumn(col-1,QModelIndex());
    }else{
        int colCnt = sensorCfgTableModel->columnCount();
        for(int i = col+1; i < colCnt; ++i){
            QString header = sensorCfgTableModel->headerData(i,Qt::Horizontal).toString();
            QString newHeader = header.section("::",0,0) + "::" + QString::number(i-1) + "::" +  header.section("::",2,2);
            sensorCfgTableModel->setHeaderData(i,Qt::Horizontal,newHeader);
        }
        sensorCfgTableModel->removeColumn(col,QModelIndex());
    }
}

void RSINormalSensorWidget::onAddSensorConfigContent()
{
    int row = sensorCfgTableModel->rowCount();
    sensorCfgTableModel->insertRow(row,QModelIndex());
}

void RSINormalSensorWidget::onRemoveSensorConfigContent()
{
    int row = sensorCfgTableView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行"));
        return;
    }
    sensorCfgTableModel->removeRow(row,QModelIndex());
}


/*========================= 发送槽函数 ==========================*/
void RSINormalSensorWidget::onAddSensorSendSection()
{
    QString type = sendSectionTypeComboBox->currentText();
    int col = sensorSendTableModel->columnCount();
    sensorSendTableModel->insertColumn(col,QModelIndex());
    QString header = "Section::" + QString::number(col)+ "::" + type;
    sensorSendTableModel->setHeaderData(col,Qt::Horizontal,header);
}

void RSINormalSensorWidget::onRemoveSensorSendSection()
{
    int col = sensorSendTableView->currentIndex().column();
    if(col < 0){
        col = sensorSendTableModel->columnCount();
        if(col > 0)
            sensorSendTableModel->removeColumn(col-1,QModelIndex());
    }else{
        int colCnt = sensorSendTableModel->columnCount();
        for(int i = col+1; i < colCnt; ++i){
            QString header = sensorSendTableModel->headerData(i,Qt::Horizontal).toString();
            QString newHeader = header.section("::",0,0) + "::" + QString::number(i-1) + "::" +  header.section("::",2,2);
            sensorSendTableModel->setHeaderData(i,Qt::Horizontal,newHeader);
        }
        sensorSendTableModel->removeColumn(col,QModelIndex());
    }
}

void RSINormalSensorWidget::onAddSensorSendContent()
{
    int row = sensorSendTableModel->rowCount();
    sensorSendTableModel->insertRow(row,QModelIndex());
}

void RSINormalSensorWidget::onRevmoveSensorSendContent()
{
    int row = sensorSendTableView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行"));
        return;
    }
    sensorSendTableModel->removeRow(row,QModelIndex());
}

/*================== 接收配置槽函数 ==============================*/
void RSINormalSensorWidget::onAddSensorRecvConfig()
{
    // 更新变量代理
    varNameDelegate->delegateClearItems();
    QHash<QString,QString>::iterator it = UserDefinedVar.begin();
    for(; it != UserDefinedVar.end(); ++it)
        varNameDelegate->delegateInsertItem(it.key());
    int row = sensorRecvTableModel->rowCount();
    sensorRecvTableModel->insertRow(row,QModelIndex());
}

void RSINormalSensorWidget::onRemoveSensorRecvConfig()
{
    int row = sensorRecvTableView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行"));
        return;
    }
    sensorRecvTableModel->removeRow(row,QModelIndex());
}


void RSINormalSensorWidget::sensorTrigSwitched(int index)
{
    if(index == 1){
        sendSectionTypeComboBox->setDisabled(true);
        addSendSectionBtn->setDisabled(true);
        removeSendSectionBtn->setDisabled(true);
        addSendContentBtn->setDisabled(true);
        removeSendContentBtn->setDisabled(true);
        sensorSendTableView->setDisabled(true);
    }else{
        sendSectionTypeComboBox->setEnabled(true);
        addSendSectionBtn->setEnabled(true);
        removeSendSectionBtn->setEnabled(true);
        addSendContentBtn->setEnabled(true);
        removeSendContentBtn->setEnabled(true);
        sensorSendTableView->setEnabled(true);
    }
}

/*============================== 终端设备 ======================================*/
// 对话框
RSISendRecvNodeDlg::RSISendRecvNodeDlg(const QStringList& varList, QWidget* parent)
    :QDialog(parent)
{
    setWindowTitle(tr("设置变量节点"));
    xmlNodeNameLabel = new QLabel(tr("xml节点名："),this);
    xmlNodeNameLineEdit = new QLineEdit(this);
    xmlNodeAttributeLabel = new QLabel(tr("xml节点属性："),this);
    xmlNodeAttributeLineEdit = new QLineEdit(this);
    variableLabel = new QLabel(tr("变量名："),this);
    variableComboBox =  new QComboBox(this);
    variableComboBox->addItems(varList);
    variableTypeLabel = new QLabel(tr("变量类型"),this);
//    variableTypeComboBox = new QComboBox(this);
//    variableTypeComboBox->addItems(RSITypes);
    variableTypeLineEdit = new QLineEdit(this);
    variableTypeLineEdit->setEnabled(false);

    OkBtn = new QPushButton(tr("确定"),this);
    cancelBtn = new QPushButton(tr("取消"),this);
    topLayout = new QGridLayout;
    topLayout->addWidget(xmlNodeNameLabel,0,0);
    topLayout->addWidget(xmlNodeNameLineEdit,0,1);
    topLayout->addWidget(xmlNodeAttributeLabel,1,0);
    topLayout->addWidget(xmlNodeAttributeLineEdit,1,1);
    topLayout->addWidget(variableLabel,2,0);
    topLayout->addWidget(variableComboBox,2,1);
    topLayout->addWidget(variableTypeLabel,3,0);
//    topLayout->addWidget(variableTypeComboBox,3,1);
    topLayout->addWidget(variableTypeLineEdit,3,1);
    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(OkBtn);
    bottomLayout->addWidget(cancelBtn);
    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    connect(OkBtn,SIGNAL(clicked(bool)),this,SLOT(onOkBtnClicked()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(onCancelBtnClicked()));

    connect(variableComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateVariableType(QString)));
    this->hide();
}

void RSISendRecvNodeDlg::updateVariableList(const QStringList& varList)
{
    variableComboBox->clear();
    qDebug() << "======================= " << varList;
    variableComboBox->addItems(varList);
}


void RSISendRecvNodeDlg::onUpdateVariables()
{
    QStringList newVarList;
    qDebug() << "============== on UpdateVariables =================" << UserDefinedVar;
    QHash<QString,QString>::iterator it = UserDefinedVar.begin();
    for(; it != UserDefinedVar.end(); ++it)
        newVarList << it.key();
    newVarList.sort();
    updateVariableList(newVarList);
    this->show();
}

void RSISendRecvNodeDlg::updateVariableType(QString varName)
{
    if(UserDefinedVar.find(varName) != UserDefinedVar.end())
        variableTypeLineEdit->setText(UserDefinedVar[varName]);
}

void RSISendRecvNodeDlg::onOkBtnClicked()
{
    QString nodeName = xmlNodeNameLineEdit->text();
    QString nodeAttr = xmlNodeAttributeLineEdit->text();
    QString variable = variableComboBox->currentText();
//    QString varType = variableTypeComboBox->currentText();
    QString varType = variableTypeLineEdit->text();
    if(nodeName.isEmpty()){
        QMessageBox::warning(this,tr("Error"),tr("请输入节点名"));
        return;
    }else{
        QStringList resList;
        if(nodeAttr.isEmpty()) resList << nodeName;
        else                    resList << nodeName + "." + nodeAttr;
        resList << varType << variable;
        emit createNewCommunicateNode(resList);
    }
    xmlNodeNameLineEdit->clear();
    xmlNodeAttributeLineEdit->clear();
    variableComboBox->setCurrentIndex(0);
//    variableTypeComboBox->setCurrentIndex(0);
    variableTypeLineEdit->clear();
    OkBtn->setFocus();
    this->hide();
}

void RSISendRecvNodeDlg::onCancelBtnClicked()
{
    xmlNodeNameLineEdit->clear();
    xmlNodeAttributeLineEdit->clear();
    variableComboBox->setCurrentIndex(0);
//    variableTypeComboBox->setCurrentIndex(0);
    variableTypeLineEdit->clear();
    OkBtn->setFocus();
    this->hide();
}


// 终端设备界面
RSITerminalWidget::RSITerminalWidget(QWidget* parent):QWidget(parent)
{
    sendTitleLabel = new QLabel(tr("发送节点配置"),this);
    recvTitleLabel = new QLabel(tr("接收节点配置"),this);
    addSendConfigBtn = new QPushButton(tr("添加发送配置节点"),this);
    removeSendConfigBtn = new QPushButton(tr("删除发送配置节点"),this);
    addRecvConfigBtn = new QPushButton(tr("添加接收配置节点"),this);
    removeRecvConfigBtn = new QPushButton(tr("删除接收配置节点"),this);
    setSendRecvModelView();

    QStringList vars;
    vars << "Fx" << "Fy";
    sendDlg = new RSISendRecvNodeDlg(vars,this);
    recvDlg = new RSISendRecvNodeDlg(vars,this);

    leftTopLayout = new QHBoxLayout;
    leftTopLayout->addWidget(sendTitleLabel);
    leftTopLayout->addStretch(1);
    leftTopLayout->addWidget(addSendConfigBtn);
    leftTopLayout->addWidget(removeSendConfigBtn);
    leftLayout = new QVBoxLayout;
    leftLayout->addLayout(leftTopLayout);
    leftLayout->addWidget(sendConfigTableView);

    rightTopLayout = new QHBoxLayout;
    rightTopLayout->addWidget(recvTitleLabel);
    rightTopLayout->addStretch(1);
    rightTopLayout->addWidget(addRecvConfigBtn);
    rightTopLayout->addWidget(removeRecvConfigBtn);
    rightLayout = new QVBoxLayout;
    rightLayout->addLayout(rightTopLayout);
    rightLayout->addWidget(recvConfigTableView);

    mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    connect(addSendConfigBtn,SIGNAL(clicked(bool)),sendDlg,SLOT(onUpdateVariables()));
    connect(addRecvConfigBtn,SIGNAL(clicked(bool)),recvDlg,SLOT(onUpdateVariables()));
    connect(sendDlg,SIGNAL(createNewCommunicateNode(QStringList)),this,SLOT(updateSendModel(QStringList)));
    connect(recvDlg,SIGNAL(createNewCommunicateNode(QStringList)),this,SLOT(updateRecvModel(QStringList)));
    connect(removeSendConfigBtn,SIGNAL(clicked(bool)),this,SLOT(onRemoveSendConfig()));
    connect(removeRecvConfigBtn,SIGNAL(clicked(bool)),this,SLOT(onRemoveRecvConfig()));
}

void RSITerminalWidget::setSendRecvModelView()
{
    sendConfigTableModel = new QStandardItemModel(0,3,this);
    sendConfigTableModel->setHeaderData(0,Qt::Horizontal,tr("节点名"));
    sendConfigTableModel->setHeaderData(1,Qt::Horizontal,tr("变量类型"));
    sendConfigTableModel->setHeaderData(2,Qt::Horizontal,tr("变量名"));
    sendConfigTableView = new QTableView(this);
    sendConfigTableView->setModel(sendConfigTableModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(sendConfigTableModel);
    sendConfigTableView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),sendConfigTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));

    recvConfigTableModel = new QStandardItemModel(0,3,this);
    recvConfigTableModel->setHeaderData(0,Qt::Horizontal,tr("节点名"));
    recvConfigTableModel->setHeaderData(1,Qt::Horizontal,tr("变量类型"));
    recvConfigTableModel->setHeaderData(2,Qt::Horizontal,tr("变量名"));
    recvConfigTableView = new QTableView(this);
    recvConfigTableView->setModel(recvConfigTableModel);
    QItemSelectionModel* selectionModel2 = new QItemSelectionModel(recvConfigTableModel);
    recvConfigTableView->setSelectionModel(selectionModel2);
    connect(selectionModel2,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),recvConfigTableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

// 产生终端设备xml文件节点
void RSITerminalWidget::generateTerminalXMLNode(XmlUntity* xml, QDomElement& communicateElement)
{
    QDomElement sendNodeElement = xml->doc.createElement(tr("SEND"));
    QDomElement sendElements = xml->doc.createElement(tr("ELEMENTS"));
    for(int i = 0; i < sendConfigTableModel->rowCount(); ++i)
    {
        QDomElement oneElement = xml->doc.createElement(tr("ELEMENT"));
        QString tagName = sendConfigTableModel->data(sendConfigTableModel->index(i,0,QModelIndex())).toString();
        QString varType = sendConfigTableModel->data(sendConfigTableModel->index(i,1,QModelIndex())).toString();
        QString varName = sendConfigTableModel->data(sendConfigTableModel->index(i,2,QModelIndex())).toString();
        oneElement.setAttribute("TAG",tagName);
        oneElement.setAttribute("TYPE",varType);
        oneElement.setAttribute("VAR",varName);
        sendElements.appendChild(oneElement);
    }
    sendNodeElement.appendChild(sendElements);
    communicateElement.appendChild(sendNodeElement);

    QDomElement recvNodeElement = xml->doc.createElement(tr("RECEIVE"));
    QDomElement recvElements = xml->doc.createElement(tr("ELEMENTS"));
    for(int i = 0; i < recvConfigTableModel->rowCount(); ++i)
    {
        QDomElement oneElement = xml->doc.createElement(tr("ELEMENT"));
        QString tagName = recvConfigTableModel->data(recvConfigTableModel->index(i,0,QModelIndex())).toString();
        QString varType = recvConfigTableModel->data(recvConfigTableModel->index(i,1,QModelIndex())).toString();
        QString varName = recvConfigTableModel->data(recvConfigTableModel->index(i,2,QModelIndex())).toString();
        oneElement.setAttribute("TAG",tagName);
        oneElement.setAttribute("TYPE",varType);
        oneElement.setAttribute("VAR",varName);
        recvElements.appendChild(oneElement);
    }
    recvNodeElement.appendChild(recvElements);
    communicateElement.appendChild(recvNodeElement);
}



void RSITerminalWidget::updateSendModel(QStringList node)
{
    int row = sendConfigTableModel->rowCount();
    sendConfigTableModel->insertRow(row,QModelIndex());
    for(int i = 0; i < node.length(); ++i)
        sendConfigTableModel->setData(sendConfigTableModel->index(row,i,QModelIndex()),node[i]);
}

void RSITerminalWidget::updateRecvModel(QStringList node)
{
    int row = recvConfigTableModel->rowCount();
    recvConfigTableModel->insertRow(row,QModelIndex());
    for(int i = 0; i < node.length(); ++i)
        recvConfigTableModel->setData(recvConfigTableModel->index(row,i,QModelIndex()),node[i]);
}

void RSITerminalWidget::onRemoveSendConfig()
{
    int row = sendConfigTableView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行"));
        return;
    }
    sendConfigTableModel->removeRow(row,QModelIndex());
}

void RSITerminalWidget::onRemoveRecvConfig()
{
    int row = recvConfigTableView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,tr("Error"),tr("请选择正确的行"));
        return;
    }
    recvConfigTableModel->removeRow(row,QModelIndex());
}

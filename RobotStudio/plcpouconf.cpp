#include "plcpouconf.h"
#include <QMenu>
#include <QDebug>
#include <QTextCodec>

PLCPouConf::PLCPouConf(QString name, QString type, PLCProjectCtrl* ctrl,QWidget *parent)
    :QWidget(parent),tagName(name),pouType(type),controller(ctrl)
{
    type2Name.insert("FUN","函数");
    type2Name.insert("FB","功能块");
    type2Name.insert("PROG","程序");
    tableNameLabel = new QLabel(type2Name[pouType]+"类型：");
    returnTypeLabel = nullptr;
    returnTypeComboBox = nullptr;
    if(pouType == "FUN") {
        returnTypeLabel = new QLabel(tr("返回值"));
        returnTypeComboBox = new QComboBox;
        QStringList retType;
        retType << "BOOL" << "SINT" << "INT" << "DINT" << "LINT" << "USINT"
                << "UINT" << "UDINT" << "ULINT" << "REAL" << "LREAL" << "TIME"
                << "DATE" << "TOD" << "DT" << "STRING" << "BYTE" << "WORD" << "DWORD" << "LWORD";
        returnTypeComboBox->addItems(retType);
        connect(returnTypeComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(returnTypeChanged(QString)));
    }
    midSplitter = new QSplitter(Qt::Horizontal,this);
    pouBodyTextEdit = new QTextEdit;
    connect(pouBodyTextEdit,SIGNAL(textChanged()),this,SLOT(updatePouBody2Controller()));

    addVariableBtn = new QPushButton(tr("添加变量"));
    rmvVariableBtn = new QPushButton(tr("删除变量"));
    connect(addVariableBtn,SIGNAL(clicked(bool)),this,SLOT(addVariableSlot()));
    connect(rmvVariableBtn,SIGNAL(clicked(bool)),this,SLOT(removeVariableSlot()));

    variableWidgetLayout = new QHBoxLayout;
    variableWidgetLayout->addWidget(tableNameLabel);
    if(returnTypeLabel != nullptr)
    {
        variableWidgetLayout->addWidget(returnTypeLabel);
        variableWidgetLayout->addWidget(returnTypeComboBox);
    }
    variableWidgetLayout->addStretch(1);
    variableWidgetLayout->addWidget(addVariableBtn);
    variableWidgetLayout->addWidget(rmvVariableBtn);

    setupVariableModel();
    setupVariableView();

    QList<QVariant> variableClassfies;
    if(pouType == "FUN")
        variableClassfies << "Input" << "Output" << "InOut" << "Local";
    else
        variableClassfies << "Input" << "Output" << "InOut" << "External"<< "Local" << "Global";
    varClassfyDelegate = new ComboDelegate(variableClassfies,this);
    QList<QVariant> variableOptions;
    variableOptions << "Constant" << "Retain" << "Non-Retain";
    varOptionDelegate = new ComboDelegate(variableOptions,this);

    varTypeDelegate = new VariableTypeDelegate(this);

    pouVariableView->setItemDelegateForColumn(1,varClassfyDelegate);
    pouVariableView->setItemDelegateForColumn(2,varTypeDelegate);
    pouVariableView->setItemDelegateForColumn(4,varOptionDelegate);
    variableLayout = new QVBoxLayout;
    variableLayout->addLayout(variableWidgetLayout);
    variableLayout->addWidget(pouVariableView);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(variableLayout);
//    mainLayout->addWidget(midSplitter);
    mainLayout->addWidget(pouBodyTextEdit);
    mainLayout->setStretchFactor(variableLayout,2);
    mainLayout->setStretchFactor(pouBodyTextEdit,3);

    updateVariablesInfo2Model();
    updateVariableDelegateTypes();
    // 将XML文件中POU程序写入编辑器
    QString pouContent;
    controller->getEditedPouElementText(tagName,pouContent);
    pouBodyTextEdit->setText(pouContent);
}


void PLCPouConf::setupVariableModel()
{
    pouVariableModel = new QStandardItemModel(0,5,this);
    pouVariableModel->setHeaderData(0,Qt::Horizontal,tr("变量名"));
    pouVariableModel->setHeaderData(1,Qt::Horizontal,tr("输入输出属性"));
    pouVariableModel->setHeaderData(2,Qt::Horizontal,tr("变量类型"));
//    pouVariableModel->setHeaderData(3,Qt::Horizontal,tr("变量位置"));
    pouVariableModel->setHeaderData(3,Qt::Horizontal,tr("变量初始值"));
    pouVariableModel->setHeaderData(4,Qt::Horizontal,tr("变量附加属性"));
}


void PLCPouConf::setupVariableView()
{
    pouVariableView = new QTableView;
    pouVariableView->setModel(pouVariableModel);
    QItemSelectionModel* selectionModel1 = new QItemSelectionModel(pouVariableModel);
    pouVariableView->setSelectionModel(selectionModel1);
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),pouVariableView,SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(selectionModel1,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(onVariableTableChanged()));
}


QString PLCPouConf::getPouName()
{
    return tagName.section("::",1);
}


void PLCPouConf::updateVariablesInfo2Model()
{
    QList<QStringList> varsInfo;
    controller->getEditedElementInterfaceVars(tagName,varsInfo);
    qDebug() << "======================= updateVariablesInfo2Model =======================";
    qDebug() << varsInfo;
    for(int i = 0; i < varsInfo.length(); ++i){
        int row = pouVariableModel->rowCount();
        pouVariableModel->insertRow(row,QModelIndex());
        for(int j = 0; j < 5; ++j)
            pouVariableModel->setData(pouVariableModel->index(row,j,QModelIndex()),varsInfo[i][j]);
    }
    if(pouType == "FUN")
    {

        QString retType = controller->getEditedElementInterfaceReturnType(tagName);
        if(!retType.isEmpty()){
            int i = returnTypeComboBox->findText(retType);
            returnTypeComboBox->setCurrentIndex(i);
        }
    }
}

void PLCPouConf::addVariableSlot()
{
    int row = pouVariableModel->rowCount();
    pouVariableModel->insertRow(row,QModelIndex());
}

void PLCPouConf::removeVariableSlot()
{
    int row = pouVariableView->currentIndex().row();
    pouVariableModel->removeRow(row);
}


void PLCPouConf::updateVariableDelegateTypes()
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
    if(pouType != "FUN"){
        QStringList fbTypes;
        controller->getFunctionBlockTypes(tagName,fbTypes);
        varTypeDelegate->addOneMenuList(tr("功能块类型"),fbTypes);
    }
}

// 返回值发生改变槽函数
void PLCPouConf::returnTypeChanged(QString text)
{
    if(!text.isEmpty()){
        qDebug() << "comboBox text =" << text;
        controller->setPouInterfaceReturnType(this->getPouName(),text);
    }
}

void PLCPouConf::onVariableTableChanged()
{
    qDebug() << "on VariableTableChaged ";
    QList<QStringList> varsList;
    for(int i = 0; i < pouVariableModel->rowCount(); ++i)
    {
        QStringList oneVar;
        for(int j = 0; j < pouVariableModel->columnCount();++j)
            oneVar.append(pouVariableModel->data(pouVariableModel->index(i,j,QModelIndex())).toString());
        varsList.append(oneVar);
    }
    qDebug() << varsList;
    controller->setPouInterfaceVars(this->getPouName(),varsList);
}

/**
 * @brief PLCPouConf::updatePouBody2Controller 当编辑框文本发生变化时更新该POU在PLCController中内容
 */
void PLCPouConf::updatePouBody2Controller()
{
    QString pouBodyText = pouBodyTextEdit->toPlainText();
//    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    qDebug() << "=================== updatePouBody2Controller =====================";
    qDebug() << tagName << " "<< pouBodyText;
//    qDebug() << codec->to
    controller->setEditedPouElementText(tagName,pouBodyText);
}

//=====================================================================================
VariableMenu::VariableMenu(QWidget* parent):QMenu(parent)
{
    connect(this,SIGNAL(triggered(QAction*)),this,SLOT(onMenuTriggered(QAction*)));
}

QString VariableMenu::getVariableType()
{
    qDebug() << "Variable text = " << text;

    return text;
}

void VariableMenu:: onMenuTriggered(QAction *action)
{
    text = action->text();
    qDebug() << "text = " << text;
}
/*====================================================================================*/
/*                  Variabel Delegate                                                 */
/*====================================================================================*/
VariableTypeDelegate::VariableTypeDelegate(QObject* parent)
    :QItemDelegate(parent)
{
}

QWidget* VariableTypeDelegate::createEditor(QWidget *parent,
                     const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
    VariableMenu* editor = new VariableMenu(parent);
//    parent->setContextMenuPolicy(Qt::ActionsContextMenu);
    for(int i = 0; i < menuNames.length(); ++i){
        QMenu* oneMenu = new QMenu(menuNames[i],editor);
        for(int j = 0; j < menuContent[i].length(); ++j){
            QAction* oneMenuType = new QAction(menuContent[i][j]);
            oneMenu->addAction(oneMenuType);
        }
        editor->addMenu(oneMenu);
    }
    editor->installEventFilter(const_cast<VariableTypeDelegate*>(this));
    editor->exec(QPoint(option.rect.x()+300,option.rect.y()+180));
    editor->hide();
    return editor;
}

/*
void VariableTypeDelegate::setEditorData(QWidget* edittor,const QModelIndex& index) const
{

}
*/

void VariableTypeDelegate::setModelData(QWidget* editor,
                                 QAbstractItemModel* model,const QModelIndex& index) const
{
    VariableMenu* varMenu = static_cast<VariableMenu*>(editor);
    QString str = varMenu->getVariableType();
    if(!str.isEmpty())
        model->setData(index,str);
}


void VariableTypeDelegate::updateEditorGeometry(QWidget* /*editor*/,
                         const QStyleOptionViewItem& /*option*/,const QModelIndex& /*index*/) const
{
//    qDebug() << "========================= oooooo =================";
//    qDebug() << option.rect;
//    VariableMenu* varMenu = static_cast<VariableMenu*>(editor);
//        varMenu->exec(QPoint(option.rect.x()+300,option.rect.y()+180));
//    editor->setGeometry(option.rect);
}

void VariableTypeDelegate::addOneMenuList(const QString& name, const QStringList& typeList)
{
    int i = 0;
    for(; i < menuNames.length(); ++i)
        if(name == menuNames[i]) break;
    if(i != menuNames.length()){
        menuNames[i] = name;
        menuContent[i] = typeList;
    }else{
        menuNames.append(name);
        menuContent.append(typeList);
    }
}

void VariableTypeDelegate::deleteOneMenuList(const QString& name)
{
    int i = 0;
    for(; i < menuNames.length(); ++i)
        if(name == menuNames[i]) break;
    if(i != menuNames.length()){
        menuNames.removeAt(i);
        menuContent.removeAt(i);
    }
}

#include "addiodlg.h"
#include <QMessageBox>

AddIODlg::AddIODlg(QWidget* parent)
    :QDialog(parent)
{
    setWindowTitle(tr("添加IO模块"));
    IOModuleNameLabel = new QLabel(tr("IO模块名称："));
    IOModuleNameLineEdit = new QLineEdit;
    IOTypeLabel = new QLabel(tr("IO类型："));
    IOTypeBox = new QComboBox;
    IOTypeBox->addItem(tr("请选择"));
    IOTypeBox->addItem(tr("数字IO"));
    IOTypeBox->addItem(tr("模拟IO"));
    IODirLabel = new QLabel(tr("IO方向"));
    IODirBox = new QComboBox;
    IODirBox->addItem(tr("请选择"));
    IODirBox->addItem(tr("输入"));
    IODirBox->addItem(tr("输出"));
    portCountLabel = new QLabel(tr("IO通道数："));
    portCountLineEdit = new QLineEdit;
    OkBtn =  new QPushButton(tr("确定"));
    cancelBtn = new QPushButton(tr("取消"));
    OkBtn->setFocus();
    connect(OkBtn,SIGNAL(clicked(bool)),this,SLOT(pressOKSlot()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(pressCancelSlot()));

    topLayout = new QGridLayout;
    topLayout->addWidget(IOModuleNameLabel,0,0);
    topLayout->addWidget(IOModuleNameLineEdit,0,1);
    topLayout->addWidget(IOTypeLabel,1,0);
    topLayout->addWidget(IOTypeBox,1,1);
    topLayout->addWidget(IODirLabel,2,0);
    topLayout->addWidget(IODirBox,2,1);
    topLayout->addWidget(portCountLabel,3,0);
    topLayout->addWidget(portCountLineEdit,3,1);
    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(OkBtn);
    bottomLayout->addWidget(cancelBtn);
    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    QDialog::hide();
}

void AddIODlg::pressOKSlot()
{
    QString IOModuleName = IOModuleNameLineEdit->text();
//    QString IOtype = IOTypeBox->currentIndex() == 0 ? "digit":"analog";
    int typeIndex = IOTypeBox->currentIndex();
    int dirIndex = IODirBox->currentIndex();
    QString IOtype, IOdir;
    if(typeIndex == 1) IOtype = "digit";
    else if(typeIndex == 2) IOtype = "analog";
    if(dirIndex == 1) IOdir= "input";
    else if(dirIndex == 2) IOdir = "output";
    QString IOport = portCountLineEdit->text();
    if(IOModuleName.isEmpty() || IOport.isEmpty() || typeIndex == 0 || dirIndex == 0){
        QMessageBox::warning(this,tr("Error"),tr("请检查输入"));
        return;
    }
    int portCount = portCountLineEdit->text().toInt();
    emit addIOModule(IOModuleName,IOtype,IOdir,portCount);
    IOModuleNameLineEdit->clear();
    IOTypeBox->setCurrentIndex(0);
    IODirBox->setCurrentIndex(0);
    portCountLineEdit->clear();
    OkBtn->setFocus();
    QDialog::hide();
}

void AddIODlg::pressCancelSlot()
{
    IOModuleNameLineEdit->clear();
    IOTypeBox->setCurrentIndex(0);
    IODirBox->setCurrentIndex(0);
    portCountLineEdit->clear();
    OkBtn->setFocus();
    QDialog::hide();
}

void AddIODlg::showAddIOdialog()
{
    QDialog::show();
}

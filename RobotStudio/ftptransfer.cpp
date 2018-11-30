#include "ftptransfer.h"
#include <QFileInfo>
#include <QDir>

FTPTransfer::FTPTransfer(QObject *parent) : QObject(parent)
{
    url.setScheme("ftp");
}

//设置FTP服务器用户名和密码
void FTPTransfer::FTPSetUserInfo(const QString& user, const QString& pwd)
{
    url.setUserName(user);
    url.setPassword(pwd);
}
//设置地址和端口
void FTPTransfer::FTPSetHostPort(const QString& str, int port)
{
    url.setHost(str);
    url.setPort(port);
}


void FTPTransfer::FTPPut(const QString& srcPath, const QString& destPath)
{
    url.setPath(destPath);
    data = new QFile(srcPath);
    if (data->open(QIODevice::ReadOnly)) {
        reply = nam.put(QNetworkRequest(url), data);
        connect(reply, SIGNAL(uploadProgress(qint64, qint64)),this, SLOT(uploadProgress(qint64, qint64)));
        connect(reply, SIGNAL(finished()), this,SLOT(uploadDone()));
    }
    else
        qDebug() << "Oops";
}


/*=========================================== Dialog ===============================================*/

FTPDialog::FTPDialog(const QString& path,QWidget* parent):QDialog(parent),projectPath(path)
{
    setWindowTitle(tr("下载文件至控制器"));
    ftpIPLabel = new QLabel(tr("控制器IP："),this);
    ftpIPLineEdit = new QLineEdit(this);
    ftpUserLabel = new QLabel(tr("FTP用户名："),this);
    ftpUserLineEdit = new QLineEdit(this);
    ftpPasswdLabel = new QLabel(tr("FTP用户密码"),this);
    ftpPasswdLineEdit = new QLineEdit(this);
    okBtn = new QPushButton(tr("确认"));
    cancelBtn = new QPushButton(tr("取消"));
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(onOKBtnClicked()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(onCancelBtnClicked()));

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(ftpIPLabel,0,0);
    mainLayout->addWidget(ftpIPLineEdit,0,1);
    mainLayout->addWidget(ftpUserLabel,1,0);
    mainLayout->addWidget(ftpUserLineEdit,1,1);
    mainLayout->addWidget(ftpPasswdLabel,2,0);
    mainLayout->addWidget(ftpPasswdLineEdit,2,1);
    mainLayout->addWidget(okBtn,3,0);
    mainLayout->addWidget(cancelBtn,3,1);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);

    this->hide();
}

void FTPDialog::showFTPDialog()
{
    this->show();
}

void FTPDialog::onOKBtnClicked()
{
    QString ipAddr = ftpIPLineEdit->text();
    QString userName = ftpUserLineEdit->text();
    QString passwd = ftpPasswdLineEdit->text();
    client.FTPSetHostPort(ipAddr);
    client.FTPSetUserInfo(userName,passwd);
    okBtn->setFocus();
    this->hide();
    QFileInfo plcXmlFile(projectPath+"/plc.xml");
    if(plcXmlFile.isFile())
        client.FTPPut(projectPath+"/plc.xml","plc.xml");
    QFileInfo configXmlFile(projectPath+"/config.xml");
    if(configXmlFile.isFile())
        client.FTPPut(projectPath+"/config.xml","config.xml");
}

void FTPDialog::onCancelBtnClicked()
{
    cancelBtn->setFocus();
    this->hide();
}


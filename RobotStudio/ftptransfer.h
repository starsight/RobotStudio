#ifndef FTPTRANSFER_H
#define FTPTRANSFER_H

#include <QObject>
#include <QtNetwork>
#include <QFile>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

class FTPTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FTPTransfer(QObject *parent = nullptr);
    void FTPSetUserInfo(const QString& user, const QString& pwd);
    void FTPSetHostPort(const QString& str, int = 21);
    void FTPPut(const QString& srcPath, const QString& destPath);
signals:

public slots:

private slots:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal)    {
        qDebug() << "Uploaded" << bytesSent << "of" << bytesTotal;
    }

    void uploadDone()     {
        qDebug() << "Finished" << reply->error();
        if(reply->error() != 0)
            QMessageBox::warning(nullptr,tr("下载失败"),tr("请检查连接"));
//        delete data;
//        delete reply;
//        data = nullptr;
//        reply = nullptr;
//        data->deleteLater();
//        reply->deleteLater();
    }
private:
    QUrl url;
    QNetworkAccessManager nam;
    QFile *data;
    QNetworkReply *reply;
};


class FTPDialog: public QDialog
{
    Q_OBJECT
public:
    explicit FTPDialog(const QString& path,QWidget* parent = nullptr);
public slots:
    void showFTPDialog();
private slots:
    void onOKBtnClicked();
    void onCancelBtnClicked();
private:
//    QLabel* processLabel;
    QLabel* ftpIPLabel, *ftpUserLabel, *ftpPasswdLabel;
    QLineEdit* ftpIPLineEdit, *ftpUserLineEdit, *ftpPasswdLineEdit;
    QPushButton* okBtn, *cancelBtn;
    QGridLayout* mainLayout;

    QString projectPath;
    FTPTransfer client;
};

#endif // FTPTRANSFER_H

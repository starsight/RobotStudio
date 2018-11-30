#include "taskintervaldelegate.h"
#include <QDebug>

TaskIntervalDlg::TaskIntervalDlg(QWidget* parent):QDialog(parent),confirm(false)
{
    this->setWindowTitle(tr("设置任务周期"));
    dayLabel = new QLabel(tr("天(d):"));
    dayLineEdit = new QLineEdit;
    dayLineEdit->setText("0");
    hourLabel = new QLabel(tr("小时(h):"));
    hourLineEdit = new QLineEdit;
    hourLineEdit->setText("0");
    minuteLabel = new QLabel(tr("分钟(m):"));
    minuteLineEdit = new QLineEdit;
    minuteLineEdit->setText("0");
    secondLabel = new QLabel(tr("秒(s):"));
    secondLineEdit = new QLineEdit;
    secondLineEdit->setText("0");
    millisecondLabel = new QLabel(tr("毫秒(ms):"));
    millisecondLineEdit = new QLineEdit;
    millisecondLineEdit->setText("0");
    microsecondLable = new QLabel(tr("微秒(us)"));
    microsecondLineEdit = new QLineEdit;
    microsecondLineEdit->setText("0");
    okBtn = new QPushButton(tr("确认"));
    cancelBtn = new QPushButton(tr("取消"));
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(onOKBtnClicked()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(onCancelBtnClicked()));

    topLayout = new QGridLayout;
    topLayout->addWidget(dayLabel,0,0);
    topLayout->addWidget(dayLineEdit,0,1);
    topLayout->addWidget(hourLabel,1,0);
    topLayout->addWidget(hourLineEdit,1,1);
    topLayout->addWidget(minuteLabel,2,0);
    topLayout->addWidget(minuteLineEdit,2,1);
    topLayout->addWidget(secondLabel,3,0);
    topLayout->addWidget(secondLineEdit,3,1);
    topLayout->addWidget(millisecondLabel,4,0);
    topLayout->addWidget(millisecondLineEdit,4,1);
    topLayout->addWidget(microsecondLable,5,0);
    topLayout->addWidget(microsecondLineEdit,5,1);
    topLayout->setColumnStretch(0,1);
    topLayout->setColumnStretch(1,1);

    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(okBtn);
    bottomLayout->addWidget(cancelBtn);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);

}


void TaskIntervalDlg::updateDatefromString(QString time)
{
    if(time.isEmpty()) return;
    int index = time.lastIndexOf("ms");
    QString leftStr = time.left(index);
    index = time.indexOf("#");
    leftStr = leftStr.right(leftStr.length()-index-1);
    int indexD = leftStr.indexOf("d");
    if(indexD == -1) dayLineEdit->setText("0");
    else{
        QString day = leftStr.left(indexD);
        dayLineEdit->setText(day);
        leftStr = leftStr.right(leftStr.length()-indexD-1);
    }
    int indexH = leftStr.indexOf("h");
    if(indexH == -1) hourLineEdit->setText("0");
    else{
        QString hour = leftStr.left(indexH);
        hourLineEdit->setText(hour);
        leftStr = leftStr.right(leftStr.length()-indexH-1);
    }
    int indexM = leftStr.indexOf("m");
    if(indexM == -1) minuteLineEdit->setText("0");
    else{
        QString minute = leftStr.left(indexM);
        minuteLineEdit->setText(minute);
        leftStr = leftStr.right(leftStr.length()-indexM-1);
    }
    int indexS = leftStr.indexOf("s");
    if(indexS == -1) secondLineEdit->setText("0");
    else{
        QString second = leftStr.left(indexS);
        secondLineEdit->setText(second);
        leftStr = leftStr.right(leftStr.length()-indexS-1);
    }
    index = leftStr.indexOf(".");
    if(index == -1) {  millisecondLineEdit->setText(leftStr); microsecondLineEdit->setText("0");}
    else{
        millisecondLineEdit->setText(leftStr.left(index));
        int usStr = leftStr.right(leftStr.length()-index-1).toInt();
        microsecondLineEdit->setText(QString::number(usStr,10));
    }
}


QString TaskIntervalDlg::computeTime(const QString& oriTime)
{
    if(confirm)
    {
        QString time = "T#";
        if(!dayLineEdit->text().isEmpty() && dayLineEdit->text() != "0"){
            time += dayLineEdit->text() +  "d"
                    + hourLineEdit->text() + "h"
                    + minuteLineEdit->text() + "m"
                    + secondLineEdit->text() + "s";
        }else if(!hourLineEdit->text().isEmpty() && hourLineEdit->text() != "0"){
            time += hourLineEdit->text() + "h"
                    + minuteLineEdit->text() + "m"
                    + secondLineEdit->text() + "s";
        }else if(!minuteLineEdit->text().isEmpty() && minuteLineEdit->text() != "0"){
            time +=  minuteLineEdit->text() + "m"
                    + secondLineEdit->text() + "s";
        }else if(!secondLineEdit->text().isEmpty() && minuteLineEdit->text() != "0")
            time += secondLineEdit->text() + "s";

        int ms = millisecondLineEdit->text().toInt();
        int us = microsecondLineEdit->text().toInt();
        QString msStr;
        if(us != 0){
            double calms = ms + 0.001 * us;
            msStr = QString::number(calms,10,3) + "ms";
        }else
            msStr = QString::number(ms,10)+"ms";
        time += msStr;
        return time;
    }else
        return oriTime;
}


void TaskIntervalDlg::onOKBtnClicked()
{
    confirm = true;
    this->hide();
}

void TaskIntervalDlg::onCancelBtnClicked()
{
    confirm = false;
    this->hide();
}


TaskIntervalDelegate::TaskIntervalDelegate(QObject *parent) : QItemDelegate(parent)
{
}


QWidget* TaskIntervalDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
    TaskIntervalDlg* editor = new TaskIntervalDlg(parent);
    editor->installEventFilter(const_cast<TaskIntervalDelegate*>(this));
    return editor;
}

void TaskIntervalDelegate::setEditorData(QWidget* editor,const QModelIndex& index) const
{
    QString timeStr = index.model()->data(index).toString();
    TaskIntervalDlg* timeDlg = static_cast<TaskIntervalDlg*>(editor);
    timeDlg->updateDatefromString(timeStr);
}

void TaskIntervalDelegate::setModelData(QWidget* editor,QAbstractItemModel* model,const QModelIndex& index) const
{
    TaskIntervalDlg* timeDlg = static_cast<TaskIntervalDlg*>(editor);
    QString oriTime = index.model()->data(index).toString();
    QString timeStr = timeDlg->computeTime(oriTime);
    model->setData(index,timeStr);
}


void TaskIntervalDelegate::updateEditorGeometry(QWidget* /*editor*/,const QStyleOptionViewItem& /*option*/,const QModelIndex& /*index*/) const
{
//    editor->setGeometry(option.rect);
}

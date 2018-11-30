#ifndef TASKINTERVALDELEGATE_H
#define TASKINTERVALDELEGATE_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QItemDelegate>

class TaskIntervalDlg:public QDialog
{
    Q_OBJECT
public:
    explicit TaskIntervalDlg(QWidget* parent = nullptr);
    void updateDatefromString(QString time);
    QString computeTime(const QString& oriTime);
private:
    QLabel *dayLabel, *hourLabel, *minuteLabel, *secondLabel, *millisecondLabel, *microsecondLable;
    QLineEdit *dayLineEdit, *hourLineEdit, *minuteLineEdit, *secondLineEdit, *millisecondLineEdit, *microsecondLineEdit;
    QPushButton* okBtn, *cancelBtn;

//    QHBoxLayout* labelLayout, *editLayout, btnLayout;
//    QVBoxLayout* mainLayout;
    QGridLayout* topLayout;
    QHBoxLayout* bottomLayout;
    QVBoxLayout* mainLayout;

    bool confirm;
private slots:
    void onOKBtnClicked();
    void onCancelBtnClicked();
};

class TaskIntervalDelegate : public QItemDelegate
{
public:
    explicit TaskIntervalDelegate(QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget* editor,const QModelIndex& index) const;
    void setModelData(QWidget* editor,QAbstractItemModel* model,const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor,const QStyleOptionViewItem& option,const QModelIndex& index) const;
};

#endif // TASKINTERVALDELEGATE_H

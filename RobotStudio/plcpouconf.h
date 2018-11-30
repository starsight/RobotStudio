#ifndef PLCPOUCONF_H
#define PLCPOUCONF_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QTextEdit>
#include <QSplitter>
#include <QItemDelegate>
#include <QMenu>
#include "combodelegate.h"
#include "plcprojectctrl.h"

// 前向声明
class VariableTypeDelegate;

class PLCPouConf : public QWidget
{
    Q_OBJECT
public:
    explicit PLCPouConf(QString name, QString type, PLCProjectCtrl* ctrl, QWidget *parent = nullptr);
    void setupVariableModel();
    void setupVariableView();
    QString getPouName();
    void updateVariablesInfo2Model();
    void updateVariableDelegateTypes();
signals:

public slots:
private slots:
    void addVariableSlot();
    void removeVariableSlot();
    void returnTypeChanged(QString text);
    void onVariableTableChanged();
    void updatePouBody2Controller();
private:
    QLabel *tableNameLabel, *returnTypeLabel;
    QComboBox* returnTypeComboBox;
    QPushButton* addVariableBtn, *rmvVariableBtn;
    QStandardItemModel* pouVariableModel;
    QTableView* pouVariableView;
    QTextEdit* pouBodyTextEdit;
    QHBoxLayout* variableWidgetLayout;
    QVBoxLayout* variableLayout, *mainLayout;
    QSplitter* midSplitter;
private:
    QString tagName, pouType;
    QHash<QString,QString> type2Name;

    ComboDelegate* varClassfyDelegate, *varOptionDelegate;
    VariableTypeDelegate *varTypeDelegate;
    PLCProjectCtrl* controller;
};


class VariableMenu:public QMenu
{
    Q_OBJECT
public:
    VariableMenu(QWidget* parent = nullptr);
    QString getVariableType();
private slots:
    void onMenuTriggered(QAction*);
private:
    QString text;
};


// 类型代理
class VariableTypeDelegate:public QItemDelegate
{
    Q_OBJECT
public:
    VariableTypeDelegate(QObject* parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
//    void setEditorData(QWidget* edittor,const QModelIndex& index) const;
    void setModelData(QWidget* editor,QAbstractItemModel* model,const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor,const QStyleOptionViewItem& option,const QModelIndex& index) const;
    void addOneMenuList(const QString& name, const QStringList& typeList);
    void deleteOneMenuList(const QString& name);
private slots:

private:
//    PLCProjectCtrl* controller;
    QStringList menuNames;
    QList<QStringList> menuContent;
};

#endif // PLCPOUCONF_H

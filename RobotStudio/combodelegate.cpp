#include "combodelegate.h"
#include <QComboBox>
#include <QMap>
#include <QDebug>

ComboDelegate::ComboDelegate(const QList<QVariant>& itemList,QObject* parent)
    :QItemDelegate(parent),editorItemList(itemList)
{
    for(int i = 0; i < itemList.length(); ++i)
        editorItemMap.insert(itemList[i].toString(),i);
}

void ComboDelegate::delegateInsertItem(const QVariant& item)
{
//    editorItemList.append(item);
    editorItemMap[item.toString()] = editorItemList.size();
    editorItemList.append(item);
    qDebug() << "========= delegateInsertItem ===========";
    qDebug() << editorItemMap;
    qDebug() << editorItemList;
}

void ComboDelegate::delegateReplaceItem(const QVariant& oriItem, const QVariant& newItem)
{
    QString oriItemName = oriItem.toString();
    int index = editorItemMap[oriItemName];
    editorItemList[index] = newItem.toString();
    editorItemMap.remove(oriItemName);
    editorItemMap[newItem.toString()] = index;
    qDebug() << "========= delegateReplaceItem ===========";
    qDebug() << editorItemMap;
    qDebug() << editorItemList;
}

void ComboDelegate::delegateClearItems()
{
    editorItemList.clear();
    editorItemList.clear();
}

QWidget* ComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
    QComboBox *editor = new QComboBox(parent);
    QMap<QString,int>::const_iterator it = editorItemMap.constBegin();
    for(; it != editorItemMap.end(); ++it)
        editor->addItem(it.key());
    editor->installEventFilter(const_cast<ComboDelegate*>(this));
    return editor;
}


void ComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString str = index.model()->data(index).toString();
    QComboBox* box = static_cast<QComboBox*>(editor);
    int i = box->findText(str);
    box->setCurrentIndex(i);
}

void ComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);
    QString str = box->currentText();
    model->setData(index,str);
}

void ComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex& /*index*/) const
{
    editor->setGeometry(option.rect);
}

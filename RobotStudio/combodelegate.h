#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H
#include <QItemDelegate>
#include <QMap>

class ComboDelegate : public QItemDelegate
{
public:
    ComboDelegate(const QList<QVariant>& itemList,QObject* parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget* edittor,const QModelIndex& index) const;
    void setModelData(QWidget* editor,QAbstractItemModel* model,const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor,const QStyleOptionViewItem& option,const QModelIndex& index) const;
    void delegateInsertItem(const QVariant& item);
    void delegateReplaceItem(const QVariant& oriItem, const QVariant& newItem);
    void delegateClearItems();
//    void delegateRefreshItems(QList<QVariant>)
private:
//    QList<QVariant> editorItemList;
     QList<QVariant> editorItemList;
     QMap<QString,int> editorItemMap;
};

#endif // COMBODELEGATE_H

#include "plclibrarytreewidget.h"
#include <QDebug>

LibraryTreeWidget::LibraryTreeWidget(PLCProjectCtrl* ctrl,QWidget* parent)
    :QTreeWidget(parent),controller(ctrl)
{
    this->setHeaderHidden(true);
    QStringList topLevelItemNames;
    QList<QStringList> subLevelItemNames;
    qDebug() << "=================== In LibraryTreeWidget constructor =======================";
    controller->getLibraryNodes(topLevelItemNames,subLevelItemNames);
    addLibraryTopLevelItems(topLevelItemNames);
    addLibrarySubLevelItems(subLevelItemNames);
}

void LibraryTreeWidget::addLibraryTopLevelItems(const QStringList& topLevelItemNames)
{
    for(int i = 0; i < topLevelItemNames.length(); ++i){
        QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << topLevelItemNames[i],0);
        topLevelItemsList << newItem;
    }
    this->insertTopLevelItems(0,topLevelItemsList);
}

void LibraryTreeWidget::addLibrarySubLevelItems(const QList<QStringList>& subLevelItemNames, const QList<QStringList>& subItemDatas)
{
    for(int i = 0; i < subLevelItemNames.length(); ++i){
        if(!subItemDatas.isEmpty())
            addItemsToTreeItem(topLevelItemsList[i],subLevelItemNames[i],subItemDatas[i]);
        else
            addItemsToTreeItem(topLevelItemsList[i],subLevelItemNames[i],QStringList());
    }
}

void LibraryTreeWidget::addItemsToTreeItem(QTreeWidgetItem* parent, const QStringList& itemNames, const QStringList& subItemData)
{
    QList<QTreeWidgetItem*> itemList;
    for(int i = 0; i < itemNames.length(); ++i)
    {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << itemNames[i],1);
        if(!subItemData.isEmpty() && !subItemData[i].isEmpty())
            newItem->setData(0,Qt::UserRole,subItemData[i]);
        itemList << newItem;
    }
    parent->addChildren(itemList);
}

//
void LibraryTreeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QTreeWidget::mousePressEvent(event);
}

void LibraryTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            startDrag();
    }
    QTreeWidget::mouseMoveEvent(event);
}


void LibraryTreeWidget::startDrag()
{
    QTreeWidgetItem* item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->data(0,Qt::UserRole).toString());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
//        drag->setPixmap(QPixmap(":/images/person.png"));
        if (drag->start(Qt::MoveAction) == Qt::MoveAction)
            delete item;
    }
}

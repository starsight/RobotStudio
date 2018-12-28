#include "solutiontree.h"
#include <QDebug>
#include <QApplication>

SolutionTree::SolutionTree(QString path,QString projectName,QWidget* parent)
    :QTreeWidget(parent)
{
  this->setHeaderHidden(true);
  QTreeWidgetItem* projectItem = new QTreeWidgetItem(this,QStringList()<< projectName,0);
  QTreeWidgetItem* rbtConfig = new QTreeWidgetItem(QStringList()<<tr("机器人参数配置"),-1);
  QTreeWidgetItem* plcUnit = new QTreeWidgetItem(QStringList()<<tr("PLC模块单元测试"),-3);
  projectItem->addChild(rbtConfig);
  projectItem->addChild(plcUnit);
  loadFiles(path,projectItem);
  this->insertTopLevelItem(0,projectItem);

  projectPath = path;
  classfyName << "DataTypes" << "Functions" << "FunctionBlocks" << "Programs" << "Resources";
}


void SolutionTree::createPLCMenus()
{
    PLCMenu = new QMenu(this);
    connect(PLCMenu,SIGNAL(triggered(QAction*)),this,SLOT(PLCMenuSlot(QAction*)));

    addTypeAction = new QAction(tr("Add Type"));
    addResourceAction = new QAction(tr("Add Resource"));
    addFunAction = new QAction(tr("Add Function"));
    addFBAction = new QAction(tr("Add Funtion Block"));
    addProgAction = new QAction(tr("Add Program"));

    removeAction = new QAction(tr("delete"));
    renameAction = new QAction(tr("rename"));
}

void SolutionTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    this->currentItem()->setFlags(this->currentItem()->flags()& (~Qt::ItemIsEditable));
    QTreeWidget::mouseDoubleClickEvent(event);
}

void SolutionTree::loadFiles(QString path,QTreeWidgetItem *item)
{
    QDir dir(path);
    if (!dir.exists()) return;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    int size = list.size();
    for (int i = 0; i < size; i++) {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..") continue;

        if (info.isDir()) {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << info.fileName(), 0);  //0表示目录
            qDebug() << info.fileName();
            if(info.fileName() == "RC")
            {
                QTreeWidgetItem* RSIItem = new QTreeWidgetItem(QStringList() << tr("RSI通信配置"),-2);  // -2表示RSI通信界面
                fileItem->addChild(RSIItem);
            }
            item->addChild(fileItem);
            loadFiles(info.filePath(),fileItem);
        } else {
            if(info.fileName().contains(".rbt"))
                continue;
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << info.fileName(), 1);   //1表示是文件
            item->addChild(fileItem);
        }
    }
    createPLCMenus();
    connect(this,SIGNAL(itemPressed(QTreeWidgetItem*,int)),this,SLOT(showAdditionMenu(QTreeWidgetItem*,int)));
}


void SolutionTree::insertPLCObj(QTreeWidgetItem* parentItem, QString itemName, int itemType, QString prefix)
{
    QTreeWidgetItem* newItem = new  QTreeWidgetItem(QStringList() << itemName,itemType);
    newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);
    int index = itemType - 2;
    int insertIndex = 0;
    for(int i = 0; i < index; ++i)
        if(!PLCObjList[i].isEmpty()) insertIndex++;
    if(PLCObjList[index].isEmpty()){
        QTreeWidgetItem* classfyItem = new QTreeWidgetItem(QStringList() << classfyName[index],0);
        parentItem->insertChild(insertIndex,classfyItem);
        classfyItem->addChild(newItem);
    }else{
        QTreeWidgetItem* tempItem = parentItem->child(insertIndex);
        tempItem->addChild(newItem);
    }

    if(!prefix.isEmpty()){
        QString fullName = prefix + itemName;
        newItem->setData(0,Qt::UserRole,fullName);
    }

    this->setCurrentItem(newItem);
    emit createNewPLCObj(newItem,0);
    PLCObjList[index].append(itemName);
}


void SolutionTree::deletePLCObj(QTreeWidgetItem* selItem)
{
    int index = selItem->type()-2;
    QString itemName = selItem->text(0);
    QTreeWidgetItem* parentItem = selItem->parent();
    parentItem->removeChild(selItem);
    selectedItem = nullptr;
    if(parentItem->childCount() == 0){
        QTreeWidgetItem* PLCItem = parentItem->parent();
        PLCItem->removeChild(parentItem);
    }
    QStringList::iterator it = PLCObjList[index].begin();
    for(; it != PLCObjList[index].end(); ++it)
        if(*it == itemName){
            PLCObjList[index].erase(it);
            break;
        }
}

/**
 * @brief MainWindow::showAdditionMenu 右击PLC目录显示附加菜单
 * @param item
 */
void SolutionTree::showAdditionMenu(QTreeWidgetItem* item,int /*column*/)
{
    selectedItem = item;
    if(qApp->mouseButtons() == Qt::RightButton)
    {
        QString itemName = item->text(0);
        int itemType = item->type();
        if(itemType == 0 && itemName == "PLC"){
                PLCMenu->clear();
                PLCMenu->addAction(addTypeAction);
                PLCMenu->addAction(addResourceAction);
                PLCMenu->addAction(addFunAction);
                PLCMenu->addAction(addFBAction);
                PLCMenu->addAction(addProgAction);
            PLCMenu->exec(QCursor::pos());
        }else if(itemType >= 2){
            PLCMenu->clear();
            PLCMenu->addAction(removeAction);
            PLCMenu->addAction(renameAction);
            PLCMenu->exec(QCursor::pos());
        }
    }
}

void SolutionTree::PLCMenuSlot(QAction *action)
{
    emit PLCMenuTriggered(action,selectedItem);
}


void SolutionTree::renameItem(QTreeWidgetItem* item)
{
    this->editItem(item);
}

/**
 * @brief SolutionTree::getPLCItem 获取PLC目录的treeItem
 * @return
 */
QTreeWidgetItem* SolutionTree::getPLCItem()
{
    QTreeWidgetItem* rootItem = this->topLevelItem(0);
    int count = rootItem->childCount();
    for(int i = 0; i < count; ++i){
        QTreeWidgetItem* item = rootItem->child(i);
        if(item->text(0) == "PLC")
            return item;
    }
    return nullptr;
}

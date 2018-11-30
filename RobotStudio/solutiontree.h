#ifndef SOLUTIONTREE_H
#define SOLUTIONTREE_H
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDir>
#include <QFileInfo>
#include <QWidget>
#include <QSet>
#include <QStringList>

#include <QMenu>

class SolutionTreeMenu;

class SolutionTree : public QTreeWidget
{
    Q_OBJECT
public:
    SolutionTree(QString path,QString projectName = "New Project",QWidget* parent = nullptr);
    void loadFiles(QString path,QTreeWidgetItem *item);
    void createPLCMenus();
    void insertPLCObj(QTreeWidgetItem* parentItem, QString itemName, int itemType,QString prefix="");
    void deletePLCObj(QTreeWidgetItem* selItem);
    void renameItem(QTreeWidgetItem* item);
    void mouseDoubleClickEvent(QMouseEvent *event);

    QTreeWidgetItem* getPLCItem();
signals:
    void PLCMenuTriggered(QAction*,QTreeWidgetItem*);
    void createNewPLCObj(QTreeWidgetItem* item,int index);
private slots:
    void showAdditionMenu(QTreeWidgetItem* item,int /*column*/);
    void PLCMenuSlot(QAction*);
public:
    QString projectPath;

    QAction* addTypeAction;
    QAction* addResourceAction;
    QAction* addFunAction;
    QAction* addFBAction;
    QAction* addProgAction;
    QAction* removeAction;
    QAction* renameAction;
private:
    QStringList PLCObjList[5];
    QStringList classfyName;
    QMenu* PLCMenu;
    QTreeWidgetItem* selectedItem;
};

#endif // SOLUTIONTREE_H

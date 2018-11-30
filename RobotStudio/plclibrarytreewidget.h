#ifndef PLCLIRARYTREEWIDGET_H
#define PLCLIRARYTREEWIDGET_H
#include <QObject>
#include <QTreeWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <plcprojectctrl.h>

class LibraryTreeWidget: public QTreeWidget
{
    Q_OBJECT
public:
    explicit LibraryTreeWidget(PLCProjectCtrl* ctrl,QWidget *parent = nullptr);
    void addLibraryTopLevelItems(const QStringList& topItemNames);
    void addLibrarySubLevelItems(const QList<QStringList>& subLevelItemNames, const QList<QStringList>& itemsData = QList<QStringList>());
    void addItemsToTreeItem(QTreeWidgetItem* parent, const QStringList& itemNames, const QStringList& itemData);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    void startDrag();
private:
    PLCProjectCtrl* controller;
    QList<QTreeWidgetItem*> topLevelItemsList;
    QPoint startPos;
};
#endif // PLCLIRARYTREEWIDGET_H

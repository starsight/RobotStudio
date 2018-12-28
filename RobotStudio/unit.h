#ifndef UNIT_H
#define UNIT_H

#include <QWidget>
#include <QStandardItemModel>

class VAR_INFO{
public:
    QString attr;
    QString name;
    QString type;
    QString init_value;
    QString result_value;

    VAR_INFO(QString attr_a,QString name_a,QString type_a){
        attr = attr_a;
        name = name_a;
        type = type_a;
    }
};

namespace Ui {
class Unit;
}

class Unit : public QWidget
{
    Q_OBJECT

public:
    explicit Unit(QWidget *parent = nullptr);
    ~Unit();

private slots:
    void on_importButton_released();

    void on_saveButton_released();

    void on_runButton_released();

private:
    Ui::Unit *ui;
    QStandardItemModel *globalVarModel;
    std::vector<VAR_INFO *> parseImportData();
    std::vector<VAR_INFO *> parseExportData();
};
#endif // UNIT_H

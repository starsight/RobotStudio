#include "unit.h"
#include "ui_unit.h"
#include <QMessageBox>
//#include<iostream>
using namespace std;

Unit::Unit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Unit)
{
    ui->setupUi(this);
    globalVarModel = new QStandardItemModel(0,5,this);
    globalVarModel->setHeaderData(0,Qt::Horizontal,tr("变量属性"));
    globalVarModel->setHeaderData(1,Qt::Horizontal,tr("变量名"));
    globalVarModel->setHeaderData(2,Qt::Horizontal,tr("变量类型"));
    globalVarModel->setHeaderData(3,Qt::Horizontal,tr("初始值"));
    globalVarModel->setHeaderData(4,Qt::Horizontal,tr("运行结果"));
    ui->tableView->setModel(globalVarModel);
    ui->tableView->setShowGrid(true);
}

Unit::~Unit()
{
    delete ui;
}

void Unit::on_importButton_released()
{
    vector<VAR_INFO *> vec=parseImportData();
    //定义item
    QStandardItem* item = 0;
    VAR_INFO *var = 0;
//    for(int i = 0;i < vec.size();i++){
//        var =vec[i];
//        item = new QStandardItem(var->attr);
//        globalVarModel->setItem(i,0,item);
//        item = new QStandardItem(var->name);
//        globalVarModel->setItem(i,1,item);
//        item = new QStandardItem(var->type);
//        globalVarModel->setItem(i,2,item);
//    }
    vector<VAR_INFO *>::iterator it;
    int i=0;
    for(it=vec.begin();it!=vec.end();it++){
        var =*it;
        item = new QStandardItem(var->attr);
        globalVarModel->setItem(i,0,item);
        item = new QStandardItem(var->name);
        globalVarModel->setItem(i,1,item);
        item = new QStandardItem(var->type);
        globalVarModel->setItem(i,2,item);
        i++;
    }

}

//解析需要导入的变量信息
vector<VAR_INFO *> Unit::parseImportData(){
    //obtainImportData();
    //verifyImportData();
    VAR_INFO *var1 = new VAR_INFO("VAR_INPUT","input_var1","1");
    VAR_INFO *var2 = new VAR_INFO("VAR_INPUT","input_var2","2");
    VAR_INFO *var3 = new VAR_INFO("VAR_IN_OUT","in_out_var","3");
    VAR_INFO *var4 = new VAR_INFO("VAR_OUTPUT","output_var","4");
    VAR_INFO *var5 = new VAR_INFO("VAR","local_var","1");
    vector<VAR_INFO *> vec;
    vec.push_back(var1);
    vec.push_back(var2);
    vec.push_back(var3);
    vec.push_back(var4);
    vec.push_back(var5);
    return vec;
}


//导出输入值信息
void Unit::on_saveButton_released()
{

}

//更新数据结果到窗口
void Unit::on_runButton_released()
{
    vector<VAR_INFO *> vec = parseExportData();
    vector<VAR_INFO *>::iterator it;
    int i=0;
    QStandardItem* item = 0;
    VAR_INFO *var = 0;
    for(it=vec.begin();it!=vec.end();it++){
        var =*it;
        item = new QStandardItem(var->result_value);
        globalVarModel->setItem(i,4,item);
        i++;
    }
}

//解析导入的信息
vector<VAR_INFO *> Unit::parseExportData(){
    VAR_INFO *var1 = new VAR_INFO("VAR_INPUT","input_var1","1");
    VAR_INFO *var2 = new VAR_INFO("VAR_INPUT","input_var2","2");
    VAR_INFO *var3 = new VAR_INFO("VAR_IN_OUT","in_out_var","3");
    VAR_INFO *var4 = new VAR_INFO("VAR_OUTPUT","output_var","4");
    VAR_INFO *var5 = new VAR_INFO("VAR","local_var","1");
    var1->result_value ="2";
    var2->result_value ="4";
    var3->result_value ="6.0";
    var4->result_value ="output string";
    var5->result_value ="2";
    vector<VAR_INFO *> vec;
    vec.push_back(var1);
    vec.push_back(var2);
    vec.push_back(var3);
    vec.push_back(var4);
    vec.push_back(var5);
    return vec;
}

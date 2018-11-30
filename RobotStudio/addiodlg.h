#ifndef ADDIODLG_H
#define ADDIODLG_H
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

class AddIODlg : public QDialog
{
    Q_OBJECT
public:
    AddIODlg(QWidget* parent = nullptr);
signals:
    void addIOModule(QString name,QString type,QString dir,int count);
public slots:
    void showAddIOdialog();
private slots:
    void pressOKSlot();
    void pressCancelSlot();
private:
    QLabel* IOModuleNameLabel,*IOTypeLabel, *IODirLabel,*portCountLabel;
    QLineEdit* IOModuleNameLineEdit, *portCountLineEdit;
    QComboBox* IOTypeBox,*IODirBox;
    QPushButton* OkBtn, *cancelBtn;
    QGridLayout* topLayout;
    QHBoxLayout* bottomLayout;
    QVBoxLayout* mainLayout;
};

#endif // ADDIODLG_H

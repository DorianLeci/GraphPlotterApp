#ifndef MYDIALOG_H
#define MYDIALOG_H
#include <QApplication>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <string.h>
#include <algorithm>
#include <QDoubleSpinBox>
#include <QVector>


#include "mainwindow.h"

class MyDialog: public QDialog{
    Q_OBJECT
public:
    MyDialog();
    QVBoxLayout* mainLayout,*izbor_layout,*file_layout;
    QWidget* editWidget,*editWidget_x,*editWidget_y;
    QFormLayout* editLayout,*editLayout_x,*editLayout_y;
    QLineEdit* lineEdit,*lineEditIndeks;
    QDialogButtonBox* buttonBox;
    QRadioButton *dark,*light,*izbor_trigon,*izbor_polinom,*izbor_kompleks,*file_save,*file_open;
    QDoubleSpinBox *range_x_poc,*range_x_kraj,*range_y_poc,*range_y_kraj;
    QLabel *label_x_poc,*label_x_kraj,*label_y_poc,*label_y_kraj,*dark_l,*light_l;


    enum Option{
        Polinom=1,Trigon=2,Kompleks=3,Dark=4,Light=5
    };
    enum FileOptions{
        FileDialogOpen=0,FileSave=1,FileOpen=2
    };

    MyDialog(QString msg);
    MyDialog(FileOptions option);
    MyDialog(int izbor); //konstruktor za dijalog s izborom funkcija
    MyDialog(MainWindow::Options selected,MainWindow::Options selected_range,const QVector<QVector<double>>&podjela_po_x,const QVector<QVector<double>>&podjela_po_y); //konstruktor za toolbar
    Option ReturnSelected();
signals:
    void EmitMode(bool mode);
    void fileSave(bool checked);
    void fileOpen(bool checked);

private:
    Option selected;
    FileOptions file_selected;
private slots:
    void PolinomSelected();
    void TrigonSelected();
    void KompleksSelected();
    void setDarkMode();
    void setLightMode();
    void FileSaveToggledMyDialog(bool checked);
    void FileOpenToggledMyDialog(bool checked);

};



#endif // MYDIALOG_H

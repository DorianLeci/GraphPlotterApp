#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyDialog.h"
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
#include <QMouseEvent>
#include <GL/gl.h>
#include <QOpenGLFunctions>
#include <QFileDialog>


using namespace std;
//mainwindow
#define MAX_LEN 1024

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSurfaceFormat format;
    format.setSamples(4);
    format.setVersion(3, 3);  // Specify OpenGL version
    format.setProfile(QSurfaceFormat::CoreProfile);  // Core profile for better performance
    format.setDepthBufferSize(24);  // Optionally, specify depth buffer size
    QSurfaceFormat::setDefaultFormat(format);

    ui->setupUi(this);
    this->resize(2560,1440);
    ui->plot->addLayer("overlay_show_coord", ui->plot->layer("foreground"));
    show_coord=new QCPItemText(ui->plot);
    show_coord->setLayer("overlay_show_coord");
    show_coord->setVisible(false);

    selected_kompleks=KompleksOff;
    selected=LightSelected;
    selected_range=None;

    file=new QAction("File",this);
    add_numbers=new QAction("Add numbers",this);
    subtract_numbers=new QAction("Subtract numbers",this);
    multiply_numbers=new QAction("Multiply numbers",this);
    divide_numbers=new QAction("Divide numbers",this);
    openFileAction=new QAction;
    saveFileAction=new QAction;
    file->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_F));
    openFileAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_O));
    saveFileAction->setShortcut(QKeySequence(Qt::CTRL |Qt::Key_S));

    addAction(openFileAction);
    addAction(saveFileAction);

    mainLayout=new QVBoxLayout;
    centralWidget()->setLayout(mainLayout);
    trenutna_fx=new QLabel;
    trenutni_br1=new QLabel;
    trenutni_action=new QLabel;
    brojevi=new QFormLayout;
    add_brojevi=new QWidget;
    horizontal=new QHBoxLayout;
    trenutna_fx->setFont(QFont("Arial",20,QFont::Bold));
    trenutna_fx->setWordWrap(true);
    trenutna_fx->setFixedSize(500,40);
    trenutna_fx->setAlignment(Qt::AlignTop);
    trenutni_br1->setFont(QFont("Arial",20,QFont::Bold));
    trenutni_br1->setWordWrap(true);
    trenutni_br1->setFixedSize(500,40);
    trenutni_br1->setAlignment(Qt::AlignTop);
    trenutni_action->setFont(QFont("Arial",20,QFont::Bold));
    trenutni_action->setWordWrap(true);
    trenutni_action->setFixedSize(500,40);
    trenutni_action->setAlignment(Qt::AlignTop);
    horizontal->addWidget(trenutna_fx);
    horizontal->addWidget(trenutni_br1);
    horizontal->addWidget(trenutni_action);
    add_brojevi->setLayout(horizontal);
    mainLayout->addWidget(add_brojevi,Qt::AlignTop);
    add_brojevi->setFixedSize(1500,40);
    mainLayout->addWidget(ui->plot);
    ui->plot->addGraph();
    ui->plot->setNotAntialiasedElements(QCP::aeAll);
    ui->plot->xAxis->grid()->setAntialiased(false);   // Disable grid anti-aliasing
    ui->plot->yAxis->grid()->setAntialiased(false);
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot->addGraph();
    ui->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plot->graph(1)->setLineStyle(QCPGraph::lsLine);

    ui->plot->xAxis->setScaleType(QCPAxis::stLinear);
    ui->plot->yAxis->setScaleType(QCPAxis::stLinear);
    ui->plot->xAxis->rescale(true);
    ui->plot->yAxis->rescale(true);
    ui->plot->xAxis->ticker()->setTickCount(5);
    ui->plot->yAxis->ticker()->setTickCount(5);
    ui->plot->graph(0)->rescaleAxes();
    ui->plot->graph(1)->rescaleAxes();
    ishodiste_x=new QCPItemLine(ui->plot);
    ishodiste_y=new QCPItemLine(ui->plot);

    ishodiste_x->setPen(QPen(Qt::red,5));
    ishodiste_y->setPen(QPen(Qt::red,5));

    ishodiste_y->start->setCoords(0,-1e5);
    ishodiste_y->end->setCoords(0,1e5);
    ishodiste_x->start->setCoords(-1e5,0);
    ishodiste_x->end->setCoords(1e5,0);

    kompl_broj_samo_imag_indeks_0=new QCPItemLine(ui->plot);
    kompl_broj_samo_imag_indeks_1=new QCPItemLine(ui->plot);
    kompl_broj_samo_real_indeks_0=new QCPItemLine(ui->plot);
    kompl_broj_samo_real_indeks_1=new QCPItemLine(ui->plot);
    zbrojeni_samo_real=new QCPItemLine(ui->plot);
    zbrojeni_samo_imag=new QCPItemLine(ui->plot);
    zbrojeni_real_i_imag=new QCPItemLine(ui->plot);
    kompl_broj_samo_imag_indeks_0->setVisible(false);
    kompl_broj_samo_imag_indeks_1->setVisible(false);
    kompl_broj_samo_real_indeks_0->setVisible(false);
    kompl_broj_samo_real_indeks_1->setVisible(false);
    zbrojeni_samo_real->setVisible(false);
    zbrojeni_samo_imag->setVisible(false);
    zbrojeni_real_i_imag->setVisible(false);
    zbrojeni_real_i_imag->setLayer("overlay_show_coord");

    qDebug()<<"added line";

    // SetIshodiste();
    QVector<double>temp;
    temp.push_back(ui->plot->xAxis->range().lower);
    temp.push_back(ui->plot->xAxis->range().upper);
    podjela_po_x.push_back(temp);

    temp.clear();
    temp.push_back(ui->plot->yAxis->range().lower);
    temp.push_back(ui->plot->yAxis->range().upper);
    podjela_po_y.push_back(temp);
    temp.clear();

    tool_bar=new QToolBar;
    set_range=new QAction("Set range",this);
    mode=new QAction("Light/Dark mode",this);
    tool_bar->addAction(file);
    tool_bar->addAction(set_range);
    tool_bar->addAction(mode);

    set_range->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_R));
    mode->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_M));

    connect(openFileAction,&QAction::triggered,this,&MainWindow::FileOpenToggled);
    connect(saveFileAction,&QAction::triggered,this,&MainWindow::FileSaveToggled);
    connect(file,&QAction::triggered,this,&MainWindow::FileDialog);
    connect(set_range,&QAction::triggered,this,&MainWindow::SetRangeTriggered);
    connect(mode,&QAction::triggered,this,&MainWindow::DarkLightTriggered);
    connect(add_numbers,&QAction::triggered,this,&MainWindow::AddCNumbers);
    connect(subtract_numbers,&QAction::triggered,this,&MainWindow::SubtractCNumbers);
    connect(multiply_numbers,&QAction::triggered,this,&MainWindow::MultiplyCNumbers);
    connect(divide_numbers,&QAction::triggered,this,&MainWindow::DivideCNumbers);

    addToolBar(tool_bar);

    QPalette light_mode;
    light_mode.setColor(QPalette::Window,QColor(70,70,70));
    light_mode.setColor(QPalette::WindowText,QColor(255,255,255));
    light_mode.setColor(QPalette::Button,QColor(100,100,100));
    light_mode.setColor(QPalette::ButtonText,QColor(255,255,255));
    light_mode.setColor(QPalette::Base,QColor(60,60,60));
    qApp->setPalette(light_mode);
    SetLight();

    this->installEventFilter(this);
    connect(ui->plot,&QCustomPlot::mousePress,this,&MainWindow::MousePress);
    connect(ui->plot,&QCustomPlot::mouseMove,this,&MainWindow::MouseDrag);
    connect(ui->plot,&QCustomPlot::mousePress,this,&MainWindow::ShowCoordinates);
    ConnectZoom();

    flag_minimum=0;
    flag_maximum=0;

    long long int res=1e5*1e5;
    trigon_fx_tocke_x.reserve(res);
    trigon_fx_tocke_y.reserve(res);

    zbrajanje=new QMessageBox(this);
    oduzimanje=new QMessageBox(this);
    mnozenje=new QMessageBox(this);
    dijeljenje=new QMessageBox(this);

}


// MainWindow::~MainWindow()
// {
//     if(ui)delete ui;
//     if(zbrajanje)delete zbrajanje;if(oduzimanje)delete oduzimanje;if(mnozenje)delete mnozenje;if(dijeljenje)delete dijeljenje;
//     if(ishodiste_y)delete ishodiste_y;if(ishodiste_x)delete ishodiste_x;
//     if(kompl_broj_samo_imag_indeks_0)delete kompl_broj_samo_imag_indeks_0;
//     if(kompl_broj_samo_real_indeks_0)delete kompl_broj_samo_real_indeks_0;
//     if(kompl_broj_samo_imag_indeks_1)delete kompl_broj_samo_imag_indeks_1;
//     if(kompl_broj_samo_real_indeks_1)delete kompl_broj_samo_real_indeks_1;
//     if(zbrojeni_samo_real)delete zbrojeni_samo_real;if(zbrojeni_samo_imag)delete zbrojeni_samo_imag;
//     if(zbrojeni_real_i_imag)delete zbrojeni_real_i_imag;
//     if(show_coord)delete show_coord;
//     if(trenutna_fx)delete trenutna_fx;if(trenutni_br1)delete trenutni_br1;if(trenutni_action)delete trenutni_action;

//     funkcija_vezana_lista.clear();
//     niz_x.clear();niz_y.clear();
//     podjela_po_x.clear();podjela_po_y.clear();
//     trigon_fx_tocke_x.clear(),trigon_fx_tocke_y.clear();
//     realni_kompl_br0.clear();realni_kompl_br1.clear();
//     zbrojeni_kompleks.clear();oduzeti_kompleks.clear();pomnozeni_kompleks.clear();dijelj_kompleks.clear();
// }

void MainWindow::addPoint(double x, double y)
{
    niz_x.push_back(x);
    niz_y.push_back(y);
}

void MainWindow::Clear()
{
    niz_x.clear();
    niz_y.clear();
}

void MainWindow::Plot()
{
    ui->plot->graph(0)->setData(niz_x,niz_y);
    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int rez;
    if(event->button()==Qt::LeftButton){
        //ako smo pritisli lijevim mišem,moramo otvoriti objekt klase MyDialog
        MyDialog open_dialog;
        open_dialog.exec();
        int enum_rez=open_dialog.ReturnSelected();
        if(enum_rez==MyDialog::Polinom){
            MyDialog polinom(enum_rez);
            rez=polinom.exec();
            QString unesena_funkcija=polinom.lineEdit->text();
            ofstream ucitaj("temp_lista.txt");
            if(!ucitaj)return;
            string filename="temp_lista.txt";

            string ucitaj_u_file=unesena_funkcija.toStdString();
            ucitaj<<ucitaj_u_file;
            ucitaj.close();
            if(rez==MyDialog::Accepted){
                if(ucitaj_u_file.length()>0){
                    UnosUListu(filename);
                }
            }
        }
        else if(enum_rez==MyDialog::Trigon){
            MyDialog trigon(enum_rez);
            trigon.exec();
            QString unesena_funkcija=trigon.lineEdit->text();
            ofstream ucitaj("trigon.text");
            if(!ucitaj)return;
            string file_name="trigon.text";
            string ucitana_funkcija=unesena_funkcija.toStdString();
            ucitaj<<ucitana_funkcija;
            ucitaj.close();
            if(ucitana_funkcija.length()>0){
                UnosTrigonF(file_name);
            }

        }
        else if(enum_rez==MyDialog::Kompleks){
            MyDialog kompleks(enum_rez);
            kompleks.exec();
            QString unesena_funkcija=kompleks.lineEdit->text();
            QString uneseni_indeks=kompleks.lineEditIndeks->text();
            int indeks=uneseni_indeks.toInt();
            ofstream ucitaj("kompleks.text");
            if(!ucitaj)return;
            string file_name="kompleks.text";
            string ucitana_funkcija=unesena_funkcija.toStdString();
            ucitaj<<ucitana_funkcija;
            ucitaj.close();
            if(ucitana_funkcija.length()>0 && uneseni_indeks.length()>0){
                UnosKompleksBr(file_name,indeks);
            }
        }
    }
}

void MainWindow::UnosUListu(string filename)
{
    list<Polinom>temp_vezana_lista;

    Polinom struktura_polinoma;
    string temp;
    double temp_koef;
    int temp_exp;
    char temp_char,temp_sign;
    ifstream iscitaj(filename);

    getline(iscitaj,temp);

    const char *ptr_c_style=temp.c_str(); //c_str() funkcija za vraćanje konstantnog pointera na početak stringa kako bi se mogao iskoristiti u funkcijama kao strcpy

    char funkcija[MAX_LEN];
    strcpy(funkcija,ptr_c_style);

    for(long unsigned int i=0;funkcija[i]!='\0';i++){
        funkcija[i]=tolower(funkcija[i]);
        if(i==strlen(funkcija)-2 && funkcija[i]=='x' && isdigit(funkcija[i+1])){
            QMessageBox error(this);
            error.setText("Nedozvoljeni format funkcije");
            error.exec();
            return;
        }
        if(i!=strlen(funkcija)-1 && funkcija[i]=='x' && funkcija[i+1]!='-' && funkcija[i+1]!='+' && funkcija[i+1]!='^'){
            QMessageBox error(this);
            error.setText("Nedozvoljeni format funkcije");
            error.exec();
            return;
        }
    }

    int offset=0,numBytes;

    qDebug()<<"Funkcija"<<funkcija;
    while(strlen(funkcija+offset)>0){
        if(sscanf(funkcija+offset,"%[+-]%lfx^%d %n",&temp_sign,&temp_koef,&temp_exp,&numBytes)==3){
            offset+=numBytes;
            qDebug()<<temp_sign;
            if(temp_sign=='+'){
                struktura_polinoma.koef=temp_koef;
            }
            else if(temp_sign=='-'){
                struktura_polinoma.koef=-temp_koef;
            }
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }

        }
        else if(sscanf(funkcija+offset,"%lfx^%d %n",&temp_koef,&temp_exp,&numBytes)==2){
            offset+=numBytes;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%[+-]x^%d%n",&temp_sign,&temp_exp,&numBytes)==2){
            offset+=numBytes;
            if(temp_sign=='+'){
                struktura_polinoma.koef=1;
            }
            else if(temp_sign=='-'){
                struktura_polinoma.koef=-1;
            }

            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"x^%d%n",&temp_exp,&numBytes)==1){
            offset+=numBytes;
            temp_koef=1;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%lf%[x]%n",&temp_koef,&temp_char,&numBytes)==2){
            offset+=numBytes;
            temp_exp=1;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%lf%n",&temp_koef,&numBytes)==1){
            offset+=numBytes;
            temp_exp=0;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }

        else if(sscanf(funkcija+offset,"%[+-]%c%n",&temp_sign,&temp_char,&numBytes)==2){
            offset+=numBytes;
            temp_exp=1;
            if(temp_sign=='-'){
                struktura_polinoma.koef=-1;
            }
            else if(temp_sign=='+'){
                struktura_polinoma.koef=1;
            }

            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%[x]%n",&temp_char,&numBytes)==1){
            offset+=numBytes;
            temp_exp=1;
            struktura_polinoma.koef=1;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else{
            qDebug()<<"break";
            QMessageBox error(this);
            error.setText("Nedozvoljeni format funkcije");
            error.exec();
            qDebug()<<(Options)selected;
            return;
        }

    }
    funkcija_vezana_lista=temp_vezana_lista;
    temp_vezana_lista.clear();
    funkcija_vezana_lista.sort(descending);
    PrintListu();
    QString unos_teksta;
    UnosSkracenogPolinomaNatragUTekst(unos_teksta);
    // for(int i=0;funkcija[i]!='\0';i++){
    //     unos_teksta+=funkcija[i];
    // }
    trenutna_fx->setText("f(x)="+unos_teksta);
    trenutni_br1->setText("");
    trenutni_action->setText("");

    selected_kompleks=KompleksOff;
    if(ui->plot->graph(0)->data()){
        ui->plot->graph(0)->data()->clear();
    }
    if(ui->plot->graph(1)->data()){
        ui->plot->graph(1)->data()->clear();
    }
    RemoveToolBarsKompleks();
    ResetItemLines();
    DrawGraph();
    if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
        trigon_fx_tocke_x.clear();
        trigon_fx_tocke_y.clear();
    }
    EmptyKompleks();

    iscitaj.close();
}

void MainWindow::UnosUListuIzDatoteke(char *funkcija)
{
    list<Polinom>temp_vezana_lista;

    Polinom struktura_polinoma;
    double temp_koef;
    int temp_exp;
    char temp_char,temp_sign;


    for(int i=0;funkcija[i]!='\0';i++){
        funkcija[i]=tolower(funkcija[i]);
        for(int i=0;funkcija[i]!='\0';i++){
            funkcija[i]=tolower(funkcija[i]);
            if(i==strlen(funkcija)-2 && funkcija[i]=='x' && isdigit(funkcija[i+1])){
                QMessageBox error(this);
                error.setText("Nedozvoljeni format funkcije");
                error.exec();
                return;
            }
            if(i!=strlen(funkcija)-1 && funkcija[i]=='x' && funkcija[i+1]!='-' && funkcija[i+1]!='+' && funkcija[i+1]!='^'){
                QMessageBox error(this);
                error.setText("Nedozvoljeni format funkcije");
                error.exec();
                return;
            }
        }
    }

    int offset=0,numBytes;

    qDebug()<<"Funkcija"<<funkcija;

    while(strlen(funkcija+offset)>0){
        if(sscanf(funkcija+offset,"%[+-]%lfx^%d %n",&temp_sign,&temp_koef,&temp_exp,&numBytes)==3){
            offset+=numBytes;
            qDebug()<<temp_sign;
            if(temp_sign=='+'){
                struktura_polinoma.koef=temp_koef;
            }
            else if(temp_sign=='-'){
                struktura_polinoma.koef=-temp_koef;
            }
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%lfx^%d %n",&temp_koef,&temp_exp,&numBytes)==2){
            offset+=numBytes;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%[+-]x^%d%n",&temp_sign,&temp_exp,&numBytes)==2){
            offset+=numBytes;
            if(temp_sign=='+'){
                struktura_polinoma.koef=1;
            }
            else if(temp_sign=='-'){
                struktura_polinoma.koef=-1;
            }

            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"x^%d%n",&temp_exp,&numBytes)==1){
            offset+=numBytes;
            temp_koef=1;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%lf%[x]%n",&temp_koef,&temp_char,&numBytes)==2){
            offset+=numBytes;
            temp_exp=1;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%lf%n",&temp_koef,&numBytes)==1){
            offset+=numBytes;
            temp_exp=0;
            struktura_polinoma.koef=temp_koef;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }

        else if(sscanf(funkcija+offset,"%[+-]%c%n",&temp_sign,&temp_char,&numBytes)==2){
            offset+=numBytes;
            temp_exp=1;
            if(temp_sign=='-'){
                struktura_polinoma.koef=-1;
            }
            else if(temp_sign=='+'){
                struktura_polinoma.koef=1;
            }

            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else if(sscanf(funkcija+offset,"%[x]%n",&temp_char,&numBytes)==1){
            offset+=numBytes;
            temp_exp=1;
            struktura_polinoma.koef=1;
            struktura_polinoma.exp=temp_exp;
            int rez=ProvjeraEksponenta(temp_vezana_lista,struktura_polinoma);
            if(rez==1){
                temp_vezana_lista.push_back(struktura_polinoma);
            }
        }
        else{
            qDebug()<<"break";
            QMessageBox error(this);
            error.setText("Nedozvoljeni format funkcije");
            error.exec();
            return;
        }

    }
    funkcija_vezana_lista=temp_vezana_lista;
    temp_vezana_lista.clear();
    PrintListu();
    if(ui->plot->graph(0)->data()){
        ui->plot->graph(0)->data()->clear();
    }
    if(ui->plot->graph(1)->data()){
        ui->plot->graph(1)->data()->clear();
    }
    QString unos_teksta;
    UnosSkracenogPolinomaNatragUTekst(unos_teksta);
    // for(int i=0;funkcija[i]!='\0';i++){
    //     unos_teksta+=funkcija[i];
    // }
    trenutna_fx->setText("f(x)="+unos_teksta);
    trenutni_br1->setText("");
    trenutni_action->setText("");

    selected_kompleks=KompleksOff;
    RemoveToolBarsKompleks();
    ResetItemLines();
    DrawGraph();
    if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
        trigon_fx_tocke_x.clear();
        trigon_fx_tocke_y.clear();
    }
    EmptyKompleks();
}

void MainWindow::UnosTrigonFxIzDatoteke(char *trigonom_funkcija)
{
    char trigon_fx[MAX_LEN],argument;
    int offset=0,numBytes;
    double koef_argumenta,pomak_y,pomak_x;

    for(int i=0;trigonom_funkcija[i]!='\0';i++){
        trigonom_funkcija[i]=tolower(trigonom_funkcija[i]);
    }
    char pom[MAX_LEN],pom_s_predznakom[MAX_LEN];
    sscanf(trigonom_funkcija,"%3s",pom);
    sscanf(trigonom_funkcija,"%4s",pom_s_predznakom);
    qDebug()<<pom;
    if(!TrigonCheck(pom) && !TrigonCheckSPredznakom(pom_s_predznakom)){
        qDebug()<<"Ne valja";
        QMessageBox warning(this);
        warning.setText("Nepostojeća trigonometrijska funkcija");
        warning.exec();
        return;
    }
    else if(!ZnakoviPoslijeX(trigonom_funkcija)){
        QMessageBox warning(this);
        warning.setText("Nepostojeća trigonometrijska funkcija.Nepoznati znakovi");
        warning.exec();
        return;
    }
    else{
        qDebug()<<"else";
        if(TrigonCheckSPredznakom(pom_s_predznakom)==1){
            qDebug()<<"1 slucaj";
            trigonom_fx.predznak=-1;
            offset=1;
        }
        else if(TrigonCheck(pom)==1){
            qDebug()<<"2 slucaj";
            trigonom_fx.predznak=1;
            offset=0;
        }
        qDebug()<<trigonom_funkcija;
        for(int i=3;trigonom_funkcija[i]!='\0';i++){

            if(trigonom_funkcija[i]!='+'&& trigonom_funkcija[i]!='-' && trigonom_funkcija[i]!='x' &&trigonom_funkcija[i]!='(' && trigonom_funkcija[i]!=')'
                && !isDecimal(trigonom_funkcija)){
                if(!isdigit(trigonom_funkcija[i])){
                    QMessageBox warning(this);
                    warning.setText("Nepostojeća trigonometrijska funkcija.Nepoznati znakovi");
                    warning.exec();
                }
                return;
            }
        }
    }

    while(strlen(trigonom_funkcija+offset)>0){
        if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x]%lf)%lf%n",trigon_fx,&koef_argumenta,&argument,&pomak_x,&pomak_y,&numBytes)==5){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x]%lf)%n",trigon_fx,&koef_argumenta,&argument,&pomak_x,&numBytes)==4){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x]%lf)%lf%n",trigon_fx,&argument,&pomak_x,&pomak_y,&numBytes)==5){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x]%lf)%n",trigon_fx,&argument,&pomak_x,&numBytes)==3){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x])%lf%n",trigon_fx,&koef_argumenta,&argument,&pomak_y,&numBytes)==4){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x])%n",trigon_fx,&koef_argumenta,&argument,&numBytes)==3){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x])%lf%n",trigon_fx,&argument,&pomak_y,&numBytes)==3){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x])%n",trigon_fx,&argument,&numBytes)==2){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else{
            QMessageBox warning(this);
            warning.setText("Nepostojeća trigonometrijska funkcija");
            warning.exec();
            return;
        }
    }

    if(trigon_fx_tocke_x.data() && trigon_fx_tocke_y.data()){
        trigon_fx_tocke_x.clear();
        trigon_fx_tocke_y.clear();
    }

    QString unos_teksta;
    for(int i=0;trigonom_funkcija[i]!='\0';i++){
        unos_teksta+=trigonom_funkcija[i];
    }
    trenutna_fx->setText("f(x)="+unos_teksta);
    trenutni_br1->setText("");
    trenutni_action->setText("");
    if(ui->plot->graph(0)->data()){
        ui->plot->graph(0)->data()->clear();
    }
    if(ui->plot->graph(1)->data()){
        ui->plot->graph(1)->data()->clear();
    }
    selected_kompleks=KompleksOff;
    RemoveToolBarsKompleks();
    ResetItemLines();
    DrawTrigonGraph(trigonom_fx);
    if(funkcija_vezana_lista.size()>0){
        funkcija_vezana_lista.clear();
    }
    EmptyKompleks();
}

void MainWindow::UnosKomplBrIzDatoteke(char *kompl_br,int indeks){
        Kompleks temp_kompl;
    if(indeks>1){
        QMessageBox error(this);
        error.setText("Moguće je imati samo dva kompleksna broja.Dozvoljeni indeksi su 0 i 1.");
        error.exec();
        return;
    }

    int offset=0,numBytes;
    double real,imag;
    char imag_i,predznak;

    int rez=CheckKompleks(kompl_br);
    qDebug()<<kompl_br;
    if(!rez){
        QMessageBox error(this);
        error.setText("Neispravan kompleksni broj.Imaginarni i realni dio trebaju odmah biti zbrojeni");
        error.exec();
        return;
    }
    else if(rez==-1){
        QMessageBox error(this);
        error.setText("Neispravni znakovi u kompleksnom broju.");
        error.exec();
        return;
    }
    while(strlen(kompl_br+offset)>0){
        if(sscanf(kompl_br+offset,"%lf%lf%[i]%n",&real,&imag,&imag_i,&numBytes)==3){
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%lf%[+-]i%n",&real,&predznak,&numBytes)==2){
            if(predznak=='+'){
                imag=1;
            }
            else if(predznak=='-'){
                imag=-1;
            }
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%lf%[i]%lf%n",&imag,&imag_i,&real,&numBytes)==3){
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }
            qDebug()<<real<<imag<<imag_i;
        }
        else if(sscanf(kompl_br+offset,"%lf%[i]%n",&imag,&imag_i,&numBytes)==2){
            real=0;
            qDebug()<<"da";
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%lf%n",&real,&numBytes)==1){
            qDebug()<<"Samo realni dio broja";
            imag=0;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
                qDebug()<<realni_kompl_br1.last();
            }
        }
        else if(sscanf(kompl_br+offset,"%[+-]i%lf%n",&predznak,&real,&numBytes)==2){
            if(predznak=='+'){
                imag=1;
            }
            else if(predznak=='-'){
                imag=-1;
            }

            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%[i]%lf%n",&imag_i,&real,&numBytes)==2){
            imag=1;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }
        }
        else if(sscanf(kompl_br+offset,"%[+-]%c%n",&predznak,&imag_i,&numBytes)==2){
            qDebug()<<"Predznak"<<predznak;
            if(predznak=='+'){
                imag=1;
            }
            else if(predznak=='-'){
                imag=-1;
            }
            real=0;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%[i]%n",&imag_i,&numBytes)==1){
            qDebug()<<"Predznak"<<predznak;
            real=0;
            imag=1;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }


        else{
            QMessageBox error(this);
            error.setText("Neispravan kompleksni broj");
            error.exec();
            return;
        }
        offset+=numBytes;
    }
    if(indeks==0){
        qDebug()<<realni_kompl_br0[realni_kompl_br0.size()-1].getReal()<<realni_kompl_br0[realni_kompl_br0.size()-1].getImag();
    }
    else if(indeks==1){
        qDebug()<<"Indeks 1 debug: "<<realni_kompl_br1[realni_kompl_br1.size()-1].getReal()<<realni_kompl_br1[realni_kompl_br1.size()-1].getImag();
    }

    if(ui->plot->graph(indeks)->data()){
        ui->plot->graph(indeks)->data()->clear();
    }

    if(ui->plot->graph(0)->data() &&selected_kompleks!=KompleksOn){
        if(funkcija_vezana_lista.size()>0){
            funkcija_vezana_lista.clear();
        }
        if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
            trigon_fx_tocke_x.clear();
            trigon_fx_tocke_y.clear();
        }
        ui->plot->graph(0)->data()->clear();
    }
    if(ui->plot->graph(1)->data()&& selected_kompleks!=KompleksOn){
        if(funkcija_vezana_lista.size()>0){
            funkcija_vezana_lista.clear();
        }
        if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
            trigon_fx_tocke_x.clear();
            trigon_fx_tocke_y.clear();
        }
        ui->plot->graph(1)->data()->clear();
    }
    selected_kompleks=KompleksOn;
    if(selected_kompleks==KompleksOn){
        tool_bar->addAction(add_numbers);
        tool_bar->addAction(subtract_numbers);
        tool_bar->addAction(multiply_numbers);
        tool_bar->addAction(divide_numbers);
    }

    UnosTeksta(indeks);
    DrawKomplNum(indeks);


}

void MainWindow::UnosUProgram()
{
    QString x,y,line;
    char funkcija[MAX_LEN];
    QString filename=QFileDialog::getOpenFileName(this,"Open file... ","/home/dorian/MojApp","MojApp file (*.mapp)");

    qDebug()<<filename;
    if(!filename.isEmpty()){
        QFile unos(filename);
        if (!unos.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::information(this, "Unable to OpenFile", unos.errorString());
        return;
        }
        qDebug()<<"otvoreno";
        QTextStream in(&unos);
        QString provjera=in.readLine();
        qDebug()<<provjera;
        qDebug()<<selected;
        if(provjera!="Moj app file"){
            QMessageBox error(this);
            error.setText("Nije moguće prepoznati radi li se o MojApp file-u");
            error.exec();
            return;
        }
        line=in.readLine();
        if((line.mid(line.indexOf(":")+2))=="LightSelected"){
            selected=LightSelected;
            QPalette light_mode;
            light_mode.setColor(QPalette::Window,QColor(70,70,70));
            light_mode.setColor(QPalette::WindowText,QColor(255,255,255));
            light_mode.setColor(QPalette::Button,QColor(100,100,100));
            light_mode.setColor(QPalette::ButtonText,QColor(255,255,255));
            light_mode.setColor(QPalette::Base,QColor(60,60,60));
            qApp->setPalette(light_mode);
            qDebug()<<"Light";
            SetLight();

        }
        else if((line.mid(line.indexOf(":")+2))=="DarkSelected"){
            qDebug()<<"Dark";
            selected=DarkSelected;
            QPalette dark_mode;
            dark_mode.setColor(QPalette::Window,QColor(0,0,0));
            dark_mode.setColor(QPalette::WindowText,QColor(255,255,255));
            dark_mode.setColor(QPalette::Button,QColor(48,25,52));
            dark_mode.setColor(QPalette::Base,QColor(48,25,52));

            qApp->setPalette(dark_mode);
            ChangeUI();
        }
        else{
            QMessageBox error(this);
            error.setText("Neispravna datoteka.Nedostatak podatka o mode-u");
            error.exec();
            return;
        }
        line=in.readLine();

        int index=line.indexOf(":");
        QStringList check=line.split(":");
        qDebug()<<check[0];
        if(check[0]!="Range x koordinata"){
            QMessageBox error(this);
            error.setText("Neispravna datoteka.Nedostatak koordinata u range-u.Reset na default");
            error.exec();
            RenewRange(0,5,0,5);
            return;
        }
        x=line.mid(index+2);
        QStringList x_split=x.split(" ");
        QVector<double>podjela_temp;
        podjela_temp.push_back(x_split[0].toDouble());
        podjela_temp.push_back(x_split[1].toDouble());
        podjela_po_x.push_back(podjela_temp);

        line=in.readLine();
        check=line.split(":");
        if(check[0]!="Range y koordinata"){
            QMessageBox error(this);
            error.setText("Neispravna datoteka.Nedostatak koordinata u range-u.Reset na default");
            error.exec();
            RenewRange(0,5,0,5);
            return;
        }
        index=line.indexOf(":");
        y=line.mid(index+2);
        QStringList y_split=y.split(" ");
        podjela_temp[0]=y_split[0].toDouble();
        podjela_temp[1]=y_split[1].toDouble();
        podjela_po_y.push_back(podjela_temp);

        RenewRange(podjela_po_x.last()[0],podjela_po_x.last()[1],podjela_po_y.last()[0],podjela_po_y.last()[1]);

        line=in.readLine();
        QStringList read=line.split(":");
        if(read[0]=="Polinom funkcija"){
            qDebug()<<"Polinom funkcija";
            QString fx=read[1].mid(read[1].indexOf(" ")+1);
            qDebug()<<fx;
            string fx_string=fx.toStdString();
            const char *ptr=fx_string.c_str();
            strcpy(funkcija,ptr);
            for(int i=0;funkcija[i]!='\0';i++){
                funkcija[i]=tolower(funkcija[i]);
            }
            UnosUListuIzDatoteke(funkcija);

        }
        else if(read[0]=="Trigonometrijska funkcija"){
            qDebug()<<"Trigonometrijska funkcija";
            QString fx=read[1].mid(read[1].indexOf(" ")+1);
            qDebug()<<fx;
            string fx_string=fx.toStdString();
            const char *ptr=fx_string.c_str();
            strcpy(funkcija,ptr);
            for(int i=0;funkcija[i]!='\0';i++){
                funkcija[i]=tolower(funkcija[i]);
            }
            UnosTrigonFxIzDatoteke(funkcija);

        }
        else if(read[0]=="Kompleksni broj1"){
            qDebug()<<"Kompleksni broj1";
            QString fx=read[1].mid(read[1].indexOf(" ")+1);
            qDebug()<<fx;
            string fx_string=fx.toStdString();
            for(int i=0;i<fx_string.length();i++){
                if(fx_string[i]=='+' && fx_string[i+1]=='-'){
                    fx_string.erase(i,1);
                }
            }
            const char *ptr=fx_string.c_str();
            strcpy(funkcija,ptr);
            for(int i=0;funkcija[i]!='\0';i++){
                funkcija[i]=tolower(funkcija[i]);
            }

            UnosKomplBrIzDatoteke(funkcija,0);

            line=in.readLine();
            read=line.split(":");
            if(read[0]=="Kompleksni broj2"){
                qDebug()<<"Kompleksni broj2";
                QString fx=read[1].mid(read[1].indexOf(" ")+1);
                qDebug()<<fx;
                string fx_string=fx.toStdString();
                for(int i=0;i<fx_string.length();i++){
                    if(fx_string[i]=='+' && fx_string[i+1]=='-'){
                        fx_string.erase(i,1);
                    }
                }
                const char *ptr=fx_string.c_str();
                strcpy(funkcija,ptr);
                for(int i=0;funkcija[i]!='\0';i++){
                    funkcija[i]=tolower(funkcija[i]);
                }
                UnosKomplBrIzDatoteke(funkcija,1);

                line=in.readLine();
                read=line.split(":");
                if(read[0]=="Zbrojeni"){
                    AddCNumbers();
                }
                else if(read[0]=="Oduzeti"){
                    SubtractCNumbers();
                }
                else if(read[0]=="Pomnozeni"){
                    MultiplyCNumbers();
                }
                else if(read[0]=="Podijeljeni"){
                    DivideCNumbers();
                }
            }

        }
        else if(read[0]=="Kompleksni broj2"){
            qDebug()<<"Kompl broj2 koji je prvi na redu";
            QString fx=read[1].mid(read[1].indexOf(" ")+1);
            qDebug()<<fx;
            string fx_string=fx.toStdString();
            for(int i=0;i<fx_string.length();i++){
                if(fx_string[i]=='+' && fx_string[i+1]=='-'){
                    fx_string.erase(i,1);
                }
            }
            const char *ptr=fx_string.c_str();
            strcpy(funkcija,ptr);
            for(int i=0;funkcija[i]!='\0';i++){
                funkcija[i]=tolower(funkcija[i]);
            }

            UnosKomplBrIzDatoteke(funkcija,1);
        }
        else{
            QMessageBox error(this);
            error.setText("Nema podataka o funkciji");
            error.exec();
            RenewRange(0,5,0,5);
            qDebug()<<"elsic";
            if(!ui->plot->graph(0)->data()->isEmpty()){
                ui->plot->graph(0)->data()->clear();
            }
            if(!ui->plot->graph(1)->data()->isEmpty()){
                ui->plot->graph(1)->data()->clear();
            }
            ResetItemLines();
            trenutna_fx->setText("");
            trenutni_br1->setText("");
            trenutni_action->setText("");
            RemoveToolBarsKompleks();
            ui->plot->replot();
            ui->plot->update();
        }




    }
    qDebug()<<"File open "<<podjela_po_x.last()[0]<<podjela_po_x.last()[1]<<podjela_po_y.last()[0]<<podjela_po_y.last()[1];
}

void MainWindow::UnosTrigonF(string filename)
{
    ifstream dat(filename);
    if(!dat)return;
    string buffer_string;
    char trigon_fx[MAX_LEN],argument;
    getline(dat,buffer_string);

    const char *buffer_char=buffer_string.c_str();
    int offset,numBytes;
    double koef_argumenta,pomak_y,pomak_x;

    char trigonom_funkcija[MAX_LEN];
    strcpy(trigonom_funkcija,buffer_char);

    for(int i=0;trigonom_funkcija[i]!='\0';i++){
        trigonom_funkcija[i]=tolower(trigonom_funkcija[i]);
    }
    char pom[MAX_LEN],pom_s_predznakom[MAX_LEN];
    sscanf(trigonom_funkcija,"%3s",pom);
    sscanf(trigonom_funkcija,"%4s",pom_s_predznakom);
    qDebug()<<pom;
    qDebug()<<pom_s_predznakom;

    if(!TrigonCheck(pom) && !TrigonCheckSPredznakom(pom_s_predznakom)){
        qDebug()<<"Ne valja";
        QMessageBox warning(this);
        warning.setText("Nepostojeća trigonometrijska funkcija");
        warning.exec();
        return;
    }
    else if(!ZnakoviPoslijeX(trigonom_funkcija)){
        QMessageBox warning(this);
        warning.setText("Nepostojeća trigonometrijska funkcija.Nepoznati znakovi");
        warning.exec();
        return;
    }
    else{
        qDebug()<<"else";
        if(TrigonCheckSPredznakom(pom_s_predznakom)==1){
            qDebug()<<"1 slucaj";
            trigonom_fx.predznak=-1;
            offset=1;
        }
        else if(TrigonCheck(pom)==1){
            qDebug()<<"2 slucaj";
            trigonom_fx.predznak=1;
            offset=0;
        }
        qDebug()<<trigonom_funkcija;
        for(int i=3;trigonom_funkcija[i]!='\0';i++){

            if(trigonom_funkcija[i]!='+'&& trigonom_funkcija[i]!='-' && trigonom_funkcija[i]!='x' &&trigonom_funkcija[i]!='(' && trigonom_funkcija[i]!=')'
                && !isDecimal(trigonom_funkcija)){
                if(!isdigit(trigonom_funkcija[i])){
                    QMessageBox warning(this);
                    warning.setText("Nepostojeća trigonometrijska funkcija.Nepoznati znakovi");
                    warning.exec();
                    return;
                }
                QMessageBox warning(this);
                warning.setText("Nepostojeća trigonometrijska funkcija.Nepoznati znakovi");
                warning.exec();
                return;
            }
        }
    }
    while(strlen(trigonom_funkcija+offset)>0){
        if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x]%lf)%lf%n",trigon_fx,&koef_argumenta,&argument,&pomak_x,&pomak_y,&numBytes)==5){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x]%lf)%n",trigon_fx,&koef_argumenta,&argument,&pomak_x,&numBytes)==4){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x]%lf)%lf%n",trigon_fx,&argument,&pomak_x,&pomak_y,&numBytes)==5){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x]%lf)%n",trigon_fx,&argument,&pomak_x,&numBytes)==3){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=pomak_x;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_x<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x])%lf%n",trigon_fx,&koef_argumenta,&argument,&pomak_y,&numBytes)==4){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%lf%[x])%n",trigon_fx,&koef_argumenta,&argument,&numBytes)==3){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=koef_argumenta;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x])%lf%n",trigon_fx,&argument,&pomak_y,&numBytes)==3){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=pomak_y;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else if(sscanf(trigonom_funkcija+offset,"%3s(%[x])%n",trigon_fx,&argument,&numBytes)==2){
            if(!TrigonCheck(trigon_fx))return;
            trigonom_fx.funkcija=trigon_fx;
            trigonom_fx.koef_arg=1;
            trigonom_fx.arg=argument;
            trigonom_fx.pomak_y=0;
            trigonom_fx.pomak_x=0;
            qDebug()<<trigonom_fx.funkcija<<trigonom_fx.koef_arg<<trigonom_fx.arg<<trigonom_fx.pomak_y;
            offset+=numBytes;
        }
        else{
            QMessageBox warning(this);
            // if(!ui->plot->graph(0)->data()->isEmpty()){
            //     DrawTrigonGraph(trigonom_fx);
            //     ui->plot->replot();
            //     ui->plot->update();
            // }

            warning.setText("Nepostojeća trigonometrijska funkcija.Nepoznati znakovi");
            warning.exec();
            return;
        }
    }
    if(trigon_fx_tocke_x.data() && trigon_fx_tocke_y.data()){
        trigon_fx_tocke_x.clear();
        trigon_fx_tocke_y.clear();
    }
    QString unos_teksta;
    for(int i=0;trigonom_funkcija[i]!='\0';i++){
        unos_teksta+=trigonom_funkcija[i];
    }
    trenutna_fx->setText("f(x)="+unos_teksta);
    trenutni_br1->setText("");
    trenutni_action->setText("");

    if(ui->plot->graph(0)->data()){
        ui->plot->graph(0)->data()->clear();
    }
    if(ui->plot->graph(1)->data()){
        ui->plot->graph(1)->data()->clear();
    }
    selected_kompleks=KompleksOff;
    RemoveToolBarsKompleks();
    ResetItemLines();
    DrawTrigonGraph(trigonom_fx);
    if(funkcija_vezana_lista.size()>0){
        funkcija_vezana_lista.clear();
    }
    EmptyKompleks();
    dat.close();


}

void MainWindow::UnosKompleksBr(string filename,int indeks)
{
    Kompleks temp_kompl;
    if(indeks>1){
        QMessageBox error(this);
        error.setText("Moguće je imati samo dva kompleksna broja.Dozvoljeni indeksi su 0 i 1.");
        error.exec();
        return;
    }
    ifstream dat(filename);
    string temp;
    getline(dat,temp);

    const char *ptr_c_style=temp.c_str(); //c_str() funkcija za vraćanje konstantnog pointera na početak stringa kako bi se mogao iskoristiti u funkcijama kao strcpy

    char kompl_br[MAX_LEN];
    strcpy(kompl_br,ptr_c_style);

    for(int i=0;kompl_br[i]!='\0';i++){
        kompl_br[i]=tolower(kompl_br[i]);
    }

    int offset=0,numBytes;
    double real,imag;
    char imag_i,predznak;

    int rez=CheckKompleks(kompl_br);
    qDebug()<<kompl_br;
    if(!rez){
        QMessageBox error(this);
        error.setText("Neispravan kompleksni broj.Imaginarni i realni dio trebaju odmah biti zbrojeni");
        error.exec();
        return;
    }
    else if(rez==-1){
        QMessageBox error(this);
        error.setText("Neispravni znakovi u kompleksnom broju.");
        error.exec();
        return;
    }
    while(strlen(kompl_br+offset)>0){
        if(sscanf(kompl_br+offset,"%lf%lf%[i]%n",&real,&imag,&imag_i,&numBytes)==3){
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%lf%[+-]i%n",&real,&predznak,&numBytes)==2){
            if(predznak=='+'){
                imag=1;
            }
            else if(predznak=='-'){
                imag=-1;
            }
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%lf%[i]%lf%n",&imag,&imag_i,&real,&numBytes)==3){
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }
            qDebug()<<real<<imag<<imag_i;
        }
        else if(sscanf(kompl_br+offset,"%lf%[i]%n",&imag,&imag_i,&numBytes)==2){
            real=0;
            qDebug()<<"da";
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%lf%n",&real,&numBytes)==1){
            qDebug()<<"Samo realni dio broja";
            imag=0;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
                qDebug()<<realni_kompl_br1.last();
            }
        }
        else if(sscanf(kompl_br+offset,"%[+-]i%lf%n",&predznak,&real,&numBytes)==2){
            if(predznak=='+'){
                imag=1;
            }
            else if(predznak=='-'){
                imag=-1;
            }

            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%[i]%lf%n",&imag_i,&real,&numBytes)==2){
            imag=1;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }
        }
        else if(sscanf(kompl_br+offset,"%[+-]%c%n",&predznak,&imag_i,&numBytes)==2){
            qDebug()<<"Predznak"<<predznak;
            if(predznak=='+'){
                imag=1;
            }
            else if(predznak=='-'){
                imag=-1;
            }
            real=0;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }
        else if(sscanf(kompl_br+offset,"%[i]%n",&imag_i,&numBytes)==1){
            qDebug()<<"Predznak"<<predznak;
            real=0;
            imag=1;
            if(!temp_kompl.Set(real,imag)){
                return;
            }
            if(indeks==0){
                realni_kompl_br0.push_back(temp_kompl);
            }
            else if(indeks==1){
                realni_kompl_br1.push_back(temp_kompl);
            }

        }


        else{
            QMessageBox error(this);
            error.setText("Neispravan kompleksni broj");
            error.exec();
            return;
        }
        offset+=numBytes;
    }
    if(indeks==0){
        qDebug()<<"Indeks 0 debug: "<<realni_kompl_br0[realni_kompl_br0.size()-1].getReal()<<realni_kompl_br0[realni_kompl_br0.size()-1].getImag();
    }
    else if(indeks==1){
        qDebug()<<"Indeks 1 debug: "<<realni_kompl_br1[realni_kompl_br1.size()-1].getReal()<<realni_kompl_br1[realni_kompl_br1.size()-1].getImag();
    }

    if(ui->plot->graph(indeks)->data()){
        ui->plot->graph(indeks)->data()->clear();
    }

    if(ui->plot->graph(0)->data() &&selected_kompleks!=KompleksOn){
        if(funkcija_vezana_lista.size()>0){
            funkcija_vezana_lista.clear();
        }
        if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
            trigon_fx_tocke_x.clear();
            trigon_fx_tocke_y.clear();
        }
        ui->plot->graph(0)->data()->clear();
    }
    if(ui->plot->graph(1)->data()&& selected_kompleks!=KompleksOn){
        if(funkcija_vezana_lista.size()>0){
            funkcija_vezana_lista.clear();
        }
        if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
            trigon_fx_tocke_x.clear();
            trigon_fx_tocke_y.clear();
        }
        ui->plot->graph(1)->data()->clear();
    }
    selected_kompleks=KompleksOn;
    if(selected_kompleks==KompleksOn){
        tool_bar->addAction(add_numbers);
        tool_bar->addAction(subtract_numbers);
        tool_bar->addAction(multiply_numbers);
        tool_bar->addAction(divide_numbers);
    }

    UnosTeksta(indeks);
    DrawKomplNum(indeks);



}

int MainWindow::CheckKompleks(char *kompl_br)
{
    int count_i=0;
    int count_real=0;
    int count_sign=0;
    for(int i=0;kompl_br[i]!='\0';i++){
        if(kompl_br[i]=='i'){
            count_i++;
        }
        if(isdigit(kompl_br[i])){
            count_real++;
        }
        if(i!=0 && (kompl_br[i]=='-'||kompl_br[i]=='+')){
            count_sign++;
        }
        if(kompl_br[i]!='-' && kompl_br[i]!='+' && kompl_br[i]!='i' && !isdigit(kompl_br[i]) && !isDecimal(kompl_br)){
            return -1;
        }
        if(i==strlen(kompl_br)-2 && kompl_br[i]=='i')return -1;
    }
    if(count_i>1)return 0;
    if(count_real>0 && count_sign>0 && count_i==0)return 0;
    return 1;
}

int MainWindow::isDecimal(char *kompl_br)
{
    int len=strlen(kompl_br);
    qDebug()<<"Is decimaL: "<<kompl_br;
    for(int i=0;i<len;i++){
        if(kompl_br[i]=='.' && !isdigit(kompl_br[i+1]))return 0;
        if(kompl_br[i]=='.' && i==len-1)return 0;
    }
    return 1;
}

int MainWindow::ZnakoviPoslijeX(char *kompl_br)
{
    int len=strlen(kompl_br);
    qDebug()<<"Is decimaL: "<<kompl_br;
    for(int i=0;i<len;i++){
        if(kompl_br[i]=='x' && kompl_br[i+1]!=')' && kompl_br[i+2]==')')return 0;
    }
    return 1;
}

int MainWindow::TrigonCheck(char *trigon_fx)
{
    if(strcmp(trigon_fx,"sin") && strcmp(trigon_fx,"cos") && strcmp(trigon_fx,"tan")
        && strcmp(trigon_fx,"cot")){
        return 0;
    }
    return 1;
}

int MainWindow::TrigonCheckSPredznakom(char *trigon_fx)
{
    if(strcmp(trigon_fx,"-sin") && strcmp(trigon_fx,"-cos") && strcmp(trigon_fx,"-tan")
        && strcmp(trigon_fx,"-cot")){
        return 0;
    }
    return 1;
}

void MainWindow::PrintListu()
{
    qDebug()<<"Trenutna lista\n\n";
    list<Polinom>::iterator pos;
    for(pos=funkcija_vezana_lista.begin();pos!=funkcija_vezana_lista.end();pos++){
        qDebug()<<pos->koef<<"x^"<<pos->exp;
    }
}



void MainWindow::on_AddButton_clicked()
{
    double x=ui->SpinBox_x->value();
    double y=ui->SpinBox_y->value();
    qDebug() << "Adding Point: x = " << x << ", y = " << y;

    addPoint(x,y);
    //uzima iz widgeta koji sam dodao upisani tekst i converta ga u odgovarajući tip
    //u funkciji add point u dva niza tipa double spremamo koordinate x i y na kraj niza
    Plot();
    //zatim se poziva funkcija Plot koja će dodati nove točke na graf
}


void MainWindow::on_ResetButton_clicked()
{
    Clear();
    Plot();
}

void MainWindow::SetRangeTriggered()
{

    selected_range=SetRange;
    MyDialog dialog(selected,selected_range,podjela_po_x,podjela_po_y);
    int rez=dialog.exec();
    selected_range=None;

    if(rez==MyDialog::Accepted){
        RenewRange(dialog.range_x_poc->value(),dialog.range_x_kraj->value(),dialog.range_y_poc->value(),dialog.range_y_kraj->value());
    }
}

void MainWindow::RenewRange(double x_poc,double x_kraj,double y_poc,double y_kraj)
{
    double compare=1e-7;
    qDebug()<<x_poc<<x_kraj<<y_poc<<y_kraj;
    if(!qFuzzyCompare(x_poc,x_kraj) && !qFuzzyCompare(y_poc,y_kraj) &&x_poc<x_kraj && y_poc<y_kraj &&
        x_poc<=10000 && x_kraj<=10000 && y_poc<=10000 && y_kraj<=10000
        && !FuzzyEqual(x_poc,compare) && !FuzzyEqual(x_kraj,compare)
        && !FuzzyEqual(y_poc,compare) && !FuzzyEqual(y_kraj,compare)){
        ui->plot->xAxis->setRange(x_poc,x_kraj);
        ui->plot->yAxis->setRange(y_poc,y_kraj);
        ui->plot->replot();
        ui->plot->update();

        QVector<double>temp_x;
        QVector<double>temp_y;

        temp_x.push_back(x_poc);
        temp_x.push_back(x_kraj);
        podjela_po_x.push_back(temp_x);

        temp_y.push_back(y_poc);
        temp_y.push_back(y_kraj);
        podjela_po_y.push_back(temp_y);
    }
    else{
        QString msg="Unesene granice nisu ispravne";
        MyDialog warning(msg);
        warning.exec();

    }


}

void MainWindow::ChangeUI()
{
    ui->plot->setBackground(QColor(0,0,0));
    ui->plot->xAxis->setBasePen(QPen(Qt::white));
    ui->plot->yAxis->setBasePen(QPen(Qt::white));
    ui->plot->xAxis->setTickLabelColor(Qt::white);
    ui->plot->yAxis->setTickLabelColor(Qt::white);
    ui->plot->xAxis->setVisible(true);
    ui->plot->yAxis->setVisible(true);

    ui->plot->graph(0)->setPen(QPen(Qt::white,5));
    ui->plot->graph(1)->setPen(QPen(Qt::yellow,5));

    ui->plot->xAxis->setTickPen(QPen(Qt::white));
    ui->plot->yAxis->setTickPen(QPen(Qt::white));
    ui->plot->xAxis->grid()->setPen(QPen(Qt::white));  // White grid lines for x-axis
    ui->plot->yAxis->grid()->setPen(QPen(Qt::white));
    ishodiste_x->setPen(QPen(Qt::red,5));
    ishodiste_y->setPen(QPen(Qt::red,5));

    show_coord->setBrush(QBrush(Qt::black));
    show_coord->setColor(Qt::yellow);

    kompl_broj_samo_real_indeks_0->setPen(QPen(Qt::white,5));
    kompl_broj_samo_imag_indeks_0->setPen(QPen(Qt::white,5));
    kompl_broj_samo_real_indeks_1->setPen(QPen(Qt::yellow,5));
    kompl_broj_samo_imag_indeks_1->setPen(QPen(Qt::yellow,5));
    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::SetLight()
{;
    ui->plot->setBackground(QColor(255,255,255));
    ui->plot->xAxis->setBasePen(QPen(Qt::black));
    ui->plot->yAxis->setBasePen(QPen(Qt::black));
    ui->plot->xAxis->setTickLabelColor(Qt::black);
    ui->plot->yAxis->setTickLabelColor(Qt::black);
    ui->plot->xAxis->setVisible(true);
    ui->plot->yAxis->setVisible(true);

    ui->plot->graph(0)->setPen(QPen(Qt::blue,5));
    ui->plot->graph(1)->setPen(QPen(Qt::red,6));

    ui->plot->xAxis->setTickPen(QPen(Qt::black));
    ui->plot->yAxis->setTickPen(QPen(Qt::black));
    ui->plot->xAxis->grid()->setPen(QPen(Qt::black));  // White grid lines for x-axis
    ui->plot->yAxis->grid()->setPen(QPen(Qt::black));
    ishodiste_x->setPen(QPen(Qt::black,5));
    ishodiste_y->setPen(QPen(Qt::black,5));

    show_coord->setBrush(QBrush(QColor(0,0,0,128)));
    show_coord->setColor(Qt::white);

    kompl_broj_samo_real_indeks_0->setPen(QPen(Qt::blue,5));
    kompl_broj_samo_imag_indeks_0->setPen(QPen(Qt::blue,5));
    kompl_broj_samo_real_indeks_1->setPen(QPen(Qt::red,5));
    kompl_broj_samo_imag_indeks_1->setPen(QPen(Qt::red,5));
    zbrojeni_samo_imag->setPen(QPen(QColor(0,255,0,115),15));
    zbrojeni_samo_real->setPen(QPen(QColor(0,255,0,115),15));
    zbrojeni_real_i_imag->setPen(QPen(QColor(0,255,0,115),15));
    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::ConnectZoom()
{
    connect(ui->plot,&QCustomPlot::mouseWheel,this,&MainWindow::onMouseWheel);
}

void MainWindow::PrintPodjelu()
{
    for(int i=0;i<podjela_po_x.size();i++){
        for(int j=0;j<podjela_po_x[i].size();j++){
            qDebug()<<podjela_po_x[i][j];
        }
    }

    for(int i=0;i<podjela_po_y.size();i++){
        for(int j=0;j<podjela_po_y[i].size();j++){
            qDebug()<<podjela_po_y[i][j];
        }
    }
}

void MainWindow::DrawGraph()
{
    QVector<double>tocke;
    double sum;
    list<Polinom>::iterator pos;

    for(double i=-100000;i<100000;i+=0.1){
        sum=0;

        for(pos=funkcija_vezana_lista.begin();pos!=funkcija_vezana_lista.end();pos++){
            sum+=pos->koef*pow(i,pos->exp);
        }
        if(fabs(sum)<1e10){
            tocke.push_back(sum);
            ui->plot->graph(0)->addData(i,sum);
        }

    }
    ui->plot->replot();
    ui->plot->update();


}

bool MainWindow::FuzzyEqual(double a,double b)
{
    double epsilon=1e-7;
    return fabs(a-b)<epsilon;
}

void MainWindow::SetIshodiste()
{
    ishodiste_y->start->setCoords(0,ui->plot->yAxis->range().lower);
    ishodiste_y->end->setCoords(0,ui->plot->yAxis->range().upper);
    ishodiste_x->start->setCoords(ui->plot->xAxis->range().lower,0);
    ishodiste_x->end->setCoords(ui->plot->xAxis->range().upper,0);
}

int MainWindow::ProvjeraTeksta(QString ref)
{
    char temp[MAX_LEN];
    char temp_ucitaj[MAX_LEN];
    string ref_string=ref.toStdString();
    const char *ptr_c_style=ref_string.c_str();
    strcpy(temp,ptr_c_style);
    sscanf(temp,"%s",temp_ucitaj);
    if(strcmp(temp_ucitaj,"Kompleksni")!=0){
        return 0;
    }
    return 1;
}

bool MainWindow::ActionFind(QAction *action, QToolBar *tool_bar)
{
    QList<QAction*>::iterator pos;
    for(pos=tool_bar->actions().begin();pos!=tool_bar->actions().end();pos++){
        if(*pos==action)return true;
    }
    return false;
}

void MainWindow::RemoveToolBarsKompleks()
{
    if(ActionFind(add_numbers,tool_bar)){
        tool_bar->removeAction(add_numbers);
    }
    if(ActionFind(subtract_numbers,tool_bar)){
        tool_bar->removeAction(subtract_numbers);
    }
    if(ActionFind(multiply_numbers,tool_bar)){
        tool_bar->removeAction(multiply_numbers);
    }
    if(ActionFind(divide_numbers,tool_bar)){
        tool_bar->removeAction(divide_numbers);
    }
}

int MainWindow::ProvjeraEksponenta(list<Polinom>&temp_vezana_lista,const Polinom &ref)
{
    list<Polinom>::iterator pos;
    for(pos=temp_vezana_lista.begin();pos!=temp_vezana_lista.end();pos++){
        if(pos->exp==ref.exp){
            pos->koef+=ref.koef;
            qDebug()<<pos->koef;
            return 0;
        }
    }
    return 1;
}

void MainWindow::UnosSkracenogPolinomaNatragUTekst(QString &unos_teksta)
{
    list<Polinom>::iterator pos;
    list<Polinom>::iterator temp;
    for(pos=funkcija_vezana_lista.begin();pos!=funkcija_vezana_lista.end();pos++){

        if(pos->koef && pos->koef!=1){
            if(pos->koef==-1 && pos->exp){
                unos_teksta+='-';
            }
            else{
                unos_teksta+=QString::number(pos->koef);
            }

        }
        if(!pos->exp && pos->koef==1){
            unos_teksta+='1';
        }
        if(pos->exp && pos->exp!=1){
            unos_teksta+="x^"+QString::number(pos->exp);
        }
        if(pos->exp==1){
            unos_teksta+='x';
        }
        temp=pos;
        if((++temp)!=funkcija_vezana_lista.end()){
            if(temp->koef>0){
                unos_teksta+='+';
            }
        }
    }
}

bool MainWindow::descending(const Polinom &a, const Polinom &b)
{
    return a.exp>b.exp;
}

inline void MainWindow::DrawTrigonGraph(const Trigonom &struktura)
{
    double value;
    for(double i=-20000;i<=20000;i+=0.05){
        if(struktura.funkcija=="sin"){
            trigon_fx_tocke_y.push_back(struktura.predznak*qSin(struktura.koef_arg*i+struktura.pomak_x)+struktura.pomak_y);
        }
        else if(struktura.funkcija=="cos"){
            trigon_fx_tocke_y.push_back(struktura.predznak*qCos(struktura.koef_arg*i+struktura.pomak_x)+struktura.pomak_y);
        }
        else if(struktura.funkcija=="tan" ){
            value=struktura.predznak*qTan(struktura.koef_arg*i+struktura.pomak_x)+struktura.pomak_y;
            trigon_fx_tocke_y.push_back(value);

        }
        else if(struktura.funkcija=="cot"){
            value=1/(struktura.predznak*qTan(struktura.koef_arg*i+struktura.pomak_x)+struktura.pomak_y);
            trigon_fx_tocke_y.push_back(value);

        }
        trigon_fx_tocke_x.push_back(i);

    }
    ui->plot->graph(0)->setData(trigon_fx_tocke_x,trigon_fx_tocke_y);
    ui->plot->replot();
}

int MainWindow::CheckNumbersInString(char trigonom_funkcija)
{
    QVector<int>ascii_brojevi;
    for(int i=48;i<58;i++){
        ascii_brojevi.push_back(i);
    }
    for(int i=0;i<ascii_brojevi.size();i++){
        if(int(trigonom_funkcija)==ascii_brojevi[i]){
            return 0;
        }
    }
    return 1;
}

int MainWindow::isQVectorMatrixEmpty(QVector<QVector<double>>podjela)
{
    QVector<QVector<double>>::iterator outer;
    for(outer=podjela.begin();outer!=podjela.end();outer++){
        if(outer->isEmpty())return 1;
    }

    return 0;
}

void MainWindow::DrawKomplNum(int indeks)
{
    int size0=realni_kompl_br0.size()-1;
    int size1=realni_kompl_br1.size()-1;
    qDebug()<<"Size 1: "<<size1;
    ui->plot->graph(indeks)->addData(0,0);
    if(indeks==0){
        if(!realni_kompl_br0[size0].getReal()){
            kompl_broj_samo_real_indeks_0->start->setCoords(0,0);
            kompl_broj_samo_real_indeks_0->end->setCoords(0,realni_kompl_br0[size0].getImag());
            kompl_broj_samo_real_indeks_0->setVisible(true);
            kompl_broj_samo_imag_indeks_0->start->setCoords(0,0);
            kompl_broj_samo_imag_indeks_0->end->setCoords(0,0);
            kompl_broj_samo_imag_indeks_0->setVisible(false);
        }
        else if(!realni_kompl_br0[size0].getImag()){
            kompl_broj_samo_imag_indeks_0->start->setCoords(0,0);
            kompl_broj_samo_imag_indeks_0->end->setCoords(realni_kompl_br0[size0].getReal(),0);
            kompl_broj_samo_imag_indeks_0->setVisible(true);
            kompl_broj_samo_real_indeks_0->start->setCoords(0,0);
            kompl_broj_samo_real_indeks_0->end->setCoords(0,0);
            kompl_broj_samo_real_indeks_0->setVisible(false);
        }
        else{
            ui->plot->graph(indeks)->addData(realni_kompl_br0[size0].getReal(),realni_kompl_br0[size0].getImag());
            kompl_broj_samo_real_indeks_0->setVisible(false);
            kompl_broj_samo_imag_indeks_0->setVisible(false);
            kompl_broj_samo_imag_indeks_0->start->setCoords(0,0);
            kompl_broj_samo_imag_indeks_0->end->setCoords(0,0);
            kompl_broj_samo_real_indeks_0->start->setCoords(0,0);
            kompl_broj_samo_real_indeks_0->end->setCoords(0,0);
        }
    }
    else if(indeks==1){
        if(!realni_kompl_br1[size1].getReal()){

            kompl_broj_samo_real_indeks_1->start->setCoords(0,0);
            kompl_broj_samo_real_indeks_1->end->setCoords(0,realni_kompl_br1[size1].getImag());
            kompl_broj_samo_real_indeks_1->setVisible(true);
            kompl_broj_samo_imag_indeks_1->start->setCoords(0,0);
            kompl_broj_samo_imag_indeks_1->end->setCoords(0,0);
            kompl_broj_samo_imag_indeks_1->setVisible(false);

        }
        else if(!realni_kompl_br1[size1].getImag()){
            qDebug()<<"Indeks 1 samo real";
            kompl_broj_samo_imag_indeks_1->start->setCoords(0,0);
            kompl_broj_samo_imag_indeks_1->end->setCoords(realni_kompl_br1[size1].getReal(),0);
            kompl_broj_samo_imag_indeks_1->setVisible(true);
            kompl_broj_samo_real_indeks_1->start->setCoords(0,0);
            kompl_broj_samo_real_indeks_1->end->setCoords(0,0);
            kompl_broj_samo_real_indeks_1->setVisible(false);
        }
        else{
            ui->plot->graph(indeks)->addData(realni_kompl_br1[size1].getReal(),realni_kompl_br1[size1].getImag());
            kompl_broj_samo_real_indeks_1->setVisible(false);
            kompl_broj_samo_imag_indeks_1->setVisible(false);
            kompl_broj_samo_imag_indeks_1->start->setCoords(0,0);
            kompl_broj_samo_imag_indeks_1->end->setCoords(0,0);
            kompl_broj_samo_real_indeks_1->start->setCoords(0,0);
            kompl_broj_samo_real_indeks_1->end->setCoords(0,0);
        }
    }


    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::DrawZbrojeni()
{
    QString ubaci;
    int size=zbrojeni_kompleks.size()-1;
    if(!zbrojeni_kompleks[size].getReal() && !zbrojeni_kompleks[size].getImag()){
        qDebug()<<"Oba nula";
        ubaci="Zbrojeni: "+QString::number(0);
        trenutni_action->setText(ubaci);
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,0);
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(0,0);
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(0,0);

        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
    }
    else if(!zbrojeni_kompleks[size].getReal()){
        qDebug()<<"prvi slucaj";
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,zbrojeni_kompleks[size].getImag());
        zbrojeni_samo_imag->setVisible(true);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Zbrojeni: "+QString::number(zbrojeni_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);
        // trenutni_br1->setText(trenutni_br1->text()+"\t    "+"Zbrojeni: "+ubaci);
    }
    else if(!zbrojeni_kompleks[size].getImag()){
        qDebug()<<"drugi slucaj";
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(zbrojeni_kompleks[size].getReal(),0);
        zbrojeni_samo_real->setVisible(true);
        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Zbrojeni: "+QString::number(zbrojeni_kompleks[size].getReal());
        trenutni_action->setText(ubaci);

        // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }
    else{
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_samo_imag->setVisible(false);
        qDebug()<<"else";
        ui->plot->replot();
        double realni=zbrojeni_kompleks[size].getReal();
        double imag=zbrojeni_kompleks[size].getImag();
        qDebug()<<realni<<imag;
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(realni,imag);
        zbrojeni_real_i_imag->setVisible(true);

        ubaci="Zbrojeni: "+QString::number(zbrojeni_kompleks[size].getReal())+" + "+QString::number(zbrojeni_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);    // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }

    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::DrawOduzeti()
{
    QString ubaci;
    int size=oduzeti_kompleks.size()-1;
    if(!oduzeti_kompleks[size].getReal() && !oduzeti_kompleks[size].getImag()){
        ubaci="Oduzeti: "+QString::number(0);
        trenutni_action->setText(ubaci);
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,0);
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(0,0);
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(0,0);

        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);

    }
    else if(!oduzeti_kompleks[size].getReal()){
        qDebug()<<"prvi slucaj";
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,oduzeti_kompleks[size].getImag());
        zbrojeni_samo_imag->setVisible(true);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Oduzeti: "+QString::number(oduzeti_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);
        // trenutni_br1->setText(trenutni_br1->text()+"\t    "+"Zbrojeni: "+ubaci);
    }
    else if(!oduzeti_kompleks[size].getImag()){
        qDebug()<<"drugi slucaj";
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(oduzeti_kompleks[size].getReal(),0);
        zbrojeni_samo_real->setVisible(true);
        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Oduzeti: "+QString::number(oduzeti_kompleks[size].getReal());
        trenutni_action->setText(ubaci);

        // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }
    else{
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_samo_imag->setVisible(false);
        qDebug()<<"else";
        ui->plot->replot();
        double realni=oduzeti_kompleks[size].getReal();
        double imag=oduzeti_kompleks[size].getImag();
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(realni,imag);
        zbrojeni_real_i_imag->setVisible(true);

        ubaci="Oduezti: "+QString::number(oduzeti_kompleks[size].getReal())+" + "+QString::number(oduzeti_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);    // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }

    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::DrawPomnozeni()
{
    QString ubaci;
    int size=pomnozeni_kompleks.size()-1;
    if(!pomnozeni_kompleks[size].getReal() && !pomnozeni_kompleks[size].getImag()){
        ubaci="Pomnozeni: "+QString::number(0);
        trenutni_action->setText(ubaci);
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,0);
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(0,0);
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(0,0);

        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);

    }
    else if(!pomnozeni_kompleks[size].getReal()){
        qDebug()<<"prvi slucaj";
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,pomnozeni_kompleks[size].getImag());
        zbrojeni_samo_imag->setVisible(true);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Pomnozeni: "+QString::number(pomnozeni_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);
        // trenutni_br1->setText(trenutni_br1->text()+"\t    "+"Zbrojeni: "+ubaci);
    }
    else if(!pomnozeni_kompleks[size].getImag()){
        qDebug()<<"drugi slucaj";
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(pomnozeni_kompleks[size].getReal(),0);
        zbrojeni_samo_real->setVisible(true);
        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Pomnozeni: "+QString::number(pomnozeni_kompleks[size].getReal());
        trenutni_action->setText(ubaci);

        // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }
    else{
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_samo_imag->setVisible(false);
        qDebug()<<"else";
        ui->plot->replot();
        double realni=pomnozeni_kompleks[size].getReal();
        double imag=pomnozeni_kompleks[size].getImag();
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(realni,imag);
        zbrojeni_real_i_imag->setVisible(true);

        ubaci="Pomnozeni: "+QString::number(pomnozeni_kompleks[size].getReal())+" + "+QString::number(pomnozeni_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);    // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }

    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::DrawDijelj()
{
    QString ubaci;
    int size=dijelj_kompleks.size()-1;
    if(!dijelj_kompleks[size].getReal() && !dijelj_kompleks[size].getImag()){
        ubaci="Podijeljeni: "+QString::number(0);
        trenutni_action->setText(ubaci);
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,0);
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(0,0);
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(0,0);

        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);

    }
    else if(!dijelj_kompleks[size].getReal()){
        qDebug()<<"prvi slucaj";
        zbrojeni_samo_imag->start->setCoords(0,0);
        zbrojeni_samo_imag->end->setCoords(0,dijelj_kompleks[size].getImag());
        zbrojeni_samo_imag->setVisible(true);
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Podijeljeni: "+QString::number(dijelj_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);
        // trenutni_br1->setText(trenutni_br1->text()+"\t    "+"Zbrojeni: "+ubaci);
    }
    else if(!dijelj_kompleks[size].getImag()){
        qDebug()<<"drugi slucaj";
        zbrojeni_samo_real->start->setCoords(0,0);
        zbrojeni_samo_real->end->setCoords(dijelj_kompleks[size].getReal(),0);
        zbrojeni_samo_real->setVisible(true);
        zbrojeni_samo_imag->setVisible(false);
        zbrojeni_real_i_imag->setVisible(false);
        ubaci="Podijeljeni: "+QString::number(dijelj_kompleks[size].getReal());
        trenutni_action->setText(ubaci);

        // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }
    else{
        zbrojeni_samo_real->setVisible(false);
        zbrojeni_samo_imag->setVisible(false);
        qDebug()<<"else";
        ui->plot->replot();
        double realni=dijelj_kompleks[size].getReal();
        double imag=dijelj_kompleks[size].getImag();
        zbrojeni_real_i_imag->start->setCoords(0,0);
        zbrojeni_real_i_imag->end->setCoords(realni,imag);
        zbrojeni_real_i_imag->setVisible(true);

        ubaci="Podijeljeni: "+QString::number(dijelj_kompleks[size].getReal())+" + "+QString::number(dijelj_kompleks[size].getImag())+"i";
        trenutni_action->setText(ubaci);    // trenutni_br1->setText(trenutni_br1->text()+"\t   "+"Zbrojeni: "+ubaci);
    }

    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::ResetItemLines()
{
    kompl_broj_samo_real_indeks_0->setVisible(false);
    kompl_broj_samo_real_indeks_0->start->setCoords(0,0);
    kompl_broj_samo_real_indeks_0->end->setCoords(0,0);
    kompl_broj_samo_imag_indeks_0->setVisible(false);
    kompl_broj_samo_imag_indeks_0->start->setCoords(0,0);
    kompl_broj_samo_imag_indeks_0->end->setCoords(0,0);
    kompl_broj_samo_real_indeks_1->setVisible(false),
        kompl_broj_samo_real_indeks_1->start->setCoords(0,0);
    kompl_broj_samo_real_indeks_1->end->setCoords(0,0);
    kompl_broj_samo_imag_indeks_1->setVisible(false);
    kompl_broj_samo_imag_indeks_1->start->setCoords(0,0);
    kompl_broj_samo_imag_indeks_1->end->setCoords(0,0);
    zbrojeni_real_i_imag->setVisible(false);
    zbrojeni_real_i_imag->start->setCoords(0,0);
    zbrojeni_real_i_imag->end->setCoords(0,0);
    zbrojeni_samo_real->setVisible(false);
    zbrojeni_samo_real->start->setCoords(0,0);
    zbrojeni_samo_real->end->setCoords(0,0);
    zbrojeni_samo_imag->setVisible(false);
    zbrojeni_samo_imag->start->setCoords(0,0);
    zbrojeni_samo_imag->end->setCoords(0,0);
}

void MainWindow::EmptyKompleks()
{
    if(!realni_kompl_br0.isEmpty()){
        realni_kompl_br0.clear();
    }
    if(!realni_kompl_br1.isEmpty()){
        realni_kompl_br1.clear();
    }
    if(!zbrojeni_kompleks.isEmpty()){
        zbrojeni_kompleks.clear();
    }
    if(!oduzeti_kompleks.isEmpty()){
        oduzeti_kompleks.clear();
    }
    if(!pomnozeni_kompleks.isEmpty()){
        pomnozeni_kompleks.clear();
    }
    if(!dijelj_kompleks.isEmpty()){
        dijelj_kompleks.clear();
    }
}

void MainWindow::UnosTeksta(int indeks)
{
    QString unos_teksta;
    int size0=realni_kompl_br0.size()-1;
    int size1=realni_kompl_br1.size()-1;
    if(indeks==0){
        if(!realni_kompl_br0[size0].getReal() && !realni_kompl_br0[size0].getImag()){
            unos_teksta=QString::number(0);
        }
        else if(!realni_kompl_br0[size0].getReal()){
            unos_teksta=QString::number(realni_kompl_br0[size0].getImag())+"i";

        }
        else if(!realni_kompl_br0[size0].getImag()){
            unos_teksta=QString::number(realni_kompl_br0[size0].getReal());
        }
        else{
            unos_teksta=QString::number(realni_kompl_br0[size0].getReal())+"+"+QString::number(realni_kompl_br0[size0].getImag())+"i";
        }

        trenutna_fx->setText("Kompleksni broj1:"+unos_teksta);
        if(ui->plot->graph(1)->data()->isEmpty()){
            trenutni_br1->setText("");
        }

    }
    else if(indeks==1){
        if(!realni_kompl_br1[size1].getReal() && !realni_kompl_br1[size1].getImag()){
            unos_teksta=QString::number(0);
        }
        else if(!realni_kompl_br1[size1].getReal()){
            unos_teksta=QString::number(realni_kompl_br1[size1].getImag())+"i";

        }
        else if(!realni_kompl_br1[size1].getImag()){
            unos_teksta=QString::number(realni_kompl_br1[size1].getReal());
        }
        else{
            unos_teksta=QString::number(realni_kompl_br1[size1].getReal())+"+"+QString::number(realni_kompl_br1[size1].getImag())+"i";
        }
        trenutni_br1->setText("Kompleksni broj2:"+unos_teksta);
        if(trenutna_fx->text()!="" && !ProvjeraTeksta(trenutna_fx->text())){
            trenutna_fx->setText("");
        }
    }
}

void MainWindow::onMouseWheel(QWheelEvent *event)
{
    double max=10000;
    double compare=1e-7;
    double precision=pow(10.0,10);
    if(event->modifiers()==Qt::ControlModifier){ //zoom uz ctrl i miš
        int delta=event->angleDelta().y();
        if(delta>0){ //scroll prema gore
            flag_maximum=0;
            if(flag_minimum==1){
                return;
            }
            ui->plot->xAxis->setRange(ui->plot->xAxis->range().lower/1.3,ui->plot->xAxis->range().upper/1.3);
            ui->plot->yAxis->setRange(ui->plot->yAxis->range().lower/1.3,ui->plot->yAxis->range().upper/1.3);
            if(podjela_po_x.data()!=nullptr &&podjela_po_y.data()!=nullptr){
                if(FuzzyEqual(ui->plot->xAxis->range().lower,compare) || FuzzyEqual(ui->plot->yAxis->range().lower,compare)
                    ||FuzzyEqual(ui->plot->xAxis->range().upper,compare) ||FuzzyEqual(ui->plot->yAxis->range().upper,compare)){
                    // SetIshodiste();
                    ui->plot->replot();
                    ui->plot->update();
                    QMessageBox msg;
                    msg.setText("Dosegnuta minimalna granica");
                    msg.exec();
                    flag_minimum=1;
                    return;
                }

                else{
                    podjela_po_x.last()[0]=round(ui->plot->xAxis->range().lower*precision)/precision;
                    podjela_po_x.last()[1]=round(ui->plot->xAxis->range().upper*precision)/precision;
                    podjela_po_y.last()[0]=round(ui->plot->yAxis->range().lower*precision)/precision;
                    podjela_po_y.last()[1]=round(ui->plot->yAxis->range().upper*precision)/precision;
                }
            }
        }
        else if(delta<0){ //scroll prema dolje
            flag_minimum=0;
            if(flag_maximum==1){
                return;
            }

            ui->plot->xAxis->setRange(ui->plot->xAxis->range().lower*1.3,ui->plot->xAxis->range().upper*1.3);
            ui->plot->yAxis->setRange(ui->plot->yAxis->range().lower*1.3,ui->plot->yAxis->range().upper*1.3);
            if(podjela_po_x.data()!=nullptr &&podjela_po_y.data()!=nullptr){
                if(ui->plot->xAxis->range().lower>=max || ui->plot->yAxis->range().lower>=max
                    ||ui->plot->xAxis->range().upper>=max ||ui->plot->yAxis->range().upper>=max){
                    // SetIshodiste();
                    ui->plot->replot();
                    ui->plot->update();
                    QMessageBox msg;
                    msg.setText("Dosegnuta maksimalna granica");
                    msg.exec();
                    flag_maximum=1;
                    return;
                }

                else{
                    podjela_po_x.last()[0]=round(ui->plot->xAxis->range().lower*precision)/precision;
                    podjela_po_x.last()[1]=round(ui->plot->xAxis->range().upper*precision)/precision;
                    podjela_po_y.last()[0]=round(ui->plot->yAxis->range().lower*precision)/precision;
                    podjela_po_y.last()[1]=round(ui->plot->yAxis->range().upper*precision)/precision;
                }
            }
        }
        // SetIshodiste();
        ui->plot->replot();
        ui->plot->update();
    }




}

void MainWindow::MouseDrag(QMouseEvent *event)
{
    double factor=pow(10.0,100);
    ui->plot->setInteractions((QCP::iRangeDrag));
    if(event->buttons()==Qt::LeftButton){ //smanjuje pomak
        double pos_x_koord=ui->plot->xAxis->pixelToCoord(event->pos().x())-ui->plot->xAxis->pixelToCoord((prev_mouse_pos.x()));
        pos_x_koord=round(pos_x_koord*factor)/factor;
        double pos_y_koord=ui->plot->yAxis->pixelToCoord(event->pos().y())-ui->plot->yAxis->pixelToCoord((prev_mouse_pos.y()));
        pos_y_koord=round(pos_y_koord*factor)/factor;
        double new_lower_x = round((ui->plot->xAxis->range().lower + pos_x_koord) * factor) / factor;
        double new_upper_x = round((ui->plot->xAxis->range().upper + pos_x_koord) * factor) / factor;
        double new_lower_y = round((ui->plot->yAxis->range().lower + pos_y_koord) * factor) / factor;
        double new_upper_y = round((ui->plot->yAxis->range().upper + pos_y_koord) * factor) / factor;

        ui->plot->xAxis->setRange(new_lower_x,new_upper_x);
        ui->plot->yAxis->setRange(new_lower_y,new_upper_y);
        // SetIshodiste();
        podjela_po_x.last()[0]=new_lower_x;
        podjela_po_x.last()[1]=new_upper_x;
        podjela_po_y.last()[0]=new_lower_y;
        podjela_po_y.last()[1]=new_upper_y;

        prev_mouse_pos=event->pos();

    }
}

void MainWindow::MousePress(QMouseEvent *event)
{
    if(event->buttons()==Qt::LeftButton){
        prev_mouse_pos=event->pos();
    }
}

void MainWindow::ShowCoordinates(QMouseEvent *event)
{
    if(event->buttons()==Qt::RightButton){
        show_coord->setVisible(true);
        double mouse_x=ui->plot->xAxis->pixelToCoord(event->pos().x());
        double mouse_y=ui->plot->yAxis->pixelToCoord(event->pos().y());
        show_coord->position->setCoords(mouse_x,mouse_y);
        QString text="("+QString::number(mouse_x)+","+QString::number(mouse_y)+")";
        show_coord->setText(text);
        show_coord->setFont(QFont("Arial",13,QFont::Bold));
        if(selected==DarkSelected){
            show_coord->setColor(Qt::yellow);
            show_coord->setPadding(QMargins(7, 7, 7, 7));
            show_coord->setBrush(QBrush(Qt::black));
            ui->plot->replot();
            ui->plot->update();
        }
        else if(selected==LightSelected){
            show_coord->setColor(Qt::white);
            show_coord->setPadding(QMargins(7, 7, 7, 7));
            QColor transparent(0,0,0,128);
            show_coord->setBrush(transparent);
            ui->plot->replot();
            ui->plot->update();
        }

    }
    else if(event->buttons()==Qt::LeftButton){
        show_coord->setVisible(false);
        ui->plot->replot();
        ui->plot->update();
    }



}

void MainWindow::AddCNumbers()
{
    if(trenutna_fx->text()=="" || trenutni_br1->text()==""){
        QMessageBox error;
        error.setText("Dva kompleksna broja moraju biti nacrtana");
        error.exec();
        return;
    }
    QString br1=trenutna_fx->text().mid(trenutna_fx->text().indexOf(":")+1);
    QString br2=trenutni_br1->text().mid(trenutna_fx->text().indexOf(":")+1);
    zbrajanje->setWindowTitle("Zbrajanje dva kompleksna broja");
    zbrajanje->setText("Zbrajanje uspjesno:");
    zbrajanje->setDetailedText("Pribrojnik1: "+br1+"\n"+"Pribrojnik2: "+br2);
    zbrajanje->setDefaultButton(nullptr);
    zbrajanje->exec();
    zbrojeni_kompleks.push_back(Kompleks(realni_kompl_br0[realni_kompl_br0.size()-1]+realni_kompl_br1[realni_kompl_br1.size()-1]));

    qDebug()<<zbrojeni_kompleks[zbrojeni_kompleks.size()-1];
    DrawZbrojeni();



}

void MainWindow::SubtractCNumbers()
{
    if(trenutna_fx->text()=="" || trenutni_br1->text()==""){
        QMessageBox error;
        error.setText("Dva kompleksna broja moraju biti nacrtana");
        error.exec();
        return;
    }
    QString br1=trenutna_fx->text().mid(trenutna_fx->text().indexOf(":")+1);
    QString br2=trenutni_br1->text().mid(trenutna_fx->text().indexOf(":")+1);
    oduzimanje->setWindowTitle("Oduzimanje dva kompleksna broja");
    oduzimanje->setText("Oduzimanje uspjesno:");
    oduzimanje->setDetailedText("Minuend: "+br1+"\n"+"Suptrahend: "+br2);
    oduzimanje->setDefaultButton(nullptr);
    oduzimanje->exec();
    oduzeti_kompleks.push_back(Kompleks(realni_kompl_br0[realni_kompl_br0.size()-1]-realni_kompl_br1[realni_kompl_br1.size()-1]));

    qDebug()<<oduzeti_kompleks[oduzeti_kompleks.size()-1];
    DrawOduzeti();
}

void MainWindow::MultiplyCNumbers()
{
    if(trenutna_fx->text()=="" || trenutni_br1->text()==""){
        QMessageBox error(this);
        error.setText("Dva kompleksna broja moraju biti nacrtana");
        error.exec();
        return;
    }
    QString br1=trenutna_fx->text().mid(trenutna_fx->text().indexOf(":")+1);
    QString br2=trenutni_br1->text().mid(trenutna_fx->text().indexOf(":")+1);
    mnozenje->setWindowTitle("Mnozenje dva kompleksna broja");
    mnozenje->setText("Mnozenje uspjesno:");
    mnozenje->setDetailedText("Multiplikand: "+br1+"\n"+"Multiplikator: "+br2);
    mnozenje->setDefaultButton(nullptr);
    mnozenje->exec();
    pomnozeni_kompleks.push_back(Kompleks(realni_kompl_br0[realni_kompl_br0.size()-1]*realni_kompl_br1[realni_kompl_br1.size()-1]));

    qDebug()<<pomnozeni_kompleks[pomnozeni_kompleks.size()-1];
    DrawPomnozeni();
}

void MainWindow::DivideCNumbers()
{
    if(trenutna_fx->text()=="" || trenutni_br1->text()==""){
        QMessageBox error;
        error.setText("Dva kompleksna broja moraju biti nacrtana");
        error.exec();
        return;
    }
    Kompleks test=realni_kompl_br0[realni_kompl_br0.size()-1]/realni_kompl_br1[realni_kompl_br1.size()-1];
    if(isnan(test.getReal()) && isnan(test.getImag()))return;

    QString br1=trenutna_fx->text().mid(trenutna_fx->text().indexOf(":")+1);
    QString br2=trenutni_br1->text().mid(trenutna_fx->text().indexOf(":")+1);
    dijeljenje->setWindowTitle("Dijeljenje dva kompleksna broja");
    dijeljenje->setText("Dijeljenje uspjesno:");
    dijeljenje->setDetailedText("Dividend: "+br1+"\n"+"Divizor: "+br2);
    dijeljenje->setDefaultButton(nullptr);
    dijeljenje->exec();
    dijelj_kompleks.push_back(Kompleks(realni_kompl_br0[realni_kompl_br0.size()-1]/realni_kompl_br1[realni_kompl_br1.size()-1]));

    qDebug()<<dijelj_kompleks[dijelj_kompleks.size()-1];
    DrawDijelj();
}

void MainWindow::FileDialog()
{
    MyDialog file_dialog(MyDialog::FileDialogOpen);
    connect(&file_dialog,&MyDialog::fileSave,this,&MainWindow::FileSaveToggled);
    connect(&file_dialog,&MyDialog::fileOpen,this,&MainWindow::FileOpenToggled);
    file_dialog.exec();

}

void MainWindow::FileSaveToggled(bool checked)
{
    QString filename=QFileDialog::getSaveFileName(this,"Save as...","/home/dorian/MojApp","MojApp file(*.mapp)");
    if(!filename.isEmpty() && !filename.endsWith(".mapp")){
        filename.append(".mapp");
    }
    if(!filename.isEmpty()){
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QMessageBox::information(this,"Unable to Open File", file.errorString());
            return;
        }
        QTextStream out(&file);
        out<<"Moj app file"<<Qt::endl;
        if(selected==LightSelected){
            out<<"Mode: LightSelected"<<Qt::endl;
        }
        if(selected==DarkSelected){
            out<<"Mode: DarkSelected"<<Qt::endl;
        }

        if(!podjela_po_x.isEmpty() && !podjela_po_y.isEmpty()){
            if(!isQVectorMatrixEmpty(podjela_po_x) && !isQVectorMatrixEmpty(podjela_po_y)){
                out<<"Range x koordinata: "<<podjela_po_x[podjela_po_x.size()-1][0]<<" "<<podjela_po_x[podjela_po_x.size()-1][1]<<Qt::endl;
                out<<"Range y koordinata: "<<podjela_po_y[podjela_po_x.size()-1][0]<<" "<<podjela_po_y[podjela_po_x.size()-1][1]<<Qt::endl;
            }
        }

        if(funkcija_vezana_lista.size()>0){
            QString temp=trenutna_fx->text().mid(trenutna_fx->text().indexOf("=")+1);
            out<<"Polinom funkcija: "<<temp<<Qt::endl;
        }
        if(!trigon_fx_tocke_x.isEmpty() && !trigon_fx_tocke_y.isEmpty()){
            QString temp=trenutna_fx->text().mid(trenutna_fx->text().indexOf("=")+1);
            out<<"Trigonometrijska funkcija: "<<temp<<Qt::endl;
        }
        if(!realni_kompl_br0.isEmpty()){
            out<<trenutna_fx->text()<<Qt::endl;
        }
        if(!realni_kompl_br1.isEmpty()){
            out<<trenutni_br1->text()<<Qt::endl;
        }
        if(!zbrojeni_kompleks.isEmpty() || !oduzeti_kompleks.isEmpty() || !pomnozeni_kompleks.isEmpty() || !dijelj_kompleks.isEmpty()){
            out<<trenutni_action->text()<<Qt::endl;
        }
    }

}

void MainWindow::FileOpenToggled(bool checked)
{
    UnosUProgram();
}


MyDialog::MyDialog(){
    izbor_layout=new QVBoxLayout;
    izbor_polinom=new QRadioButton;
    izbor_trigon=new QRadioButton;
    izbor_kompleks=new QRadioButton;

    izbor_polinom->setText("Polinom");
    izbor_trigon->setText("Trigonometrijska");
    izbor_kompleks->setText("Kompleksni broj");

    izbor_layout->addWidget(izbor_polinom);
    izbor_layout->addWidget(izbor_trigon);
    izbor_layout->addWidget(izbor_kompleks);

    connect(izbor_polinom,&QRadioButton::toggled,this,&MyDialog::PolinomSelected);
    connect(izbor_trigon,&QRadioButton::toggled,this,&MyDialog::TrigonSelected);
    connect(izbor_kompleks,&QRadioButton::toggled,this,&MyDialog::KompleksSelected);

    setLayout(izbor_layout);
    setWindowTitle("Izbor vrste funkcije");
    setMinimumSize(200,50);

}

MyDialog::MyDialog(FileOptions option)
{
    file_layout=new QVBoxLayout;
    file_save=new QRadioButton;
    file_save->setText("File save");
    file_open=new QRadioButton;
    file_open->setText("File open");

    file_layout->addWidget(file_save);
    file_layout->addWidget(file_open);
    setLayout(file_layout);
    setWindowTitle("File Options");
    setMinimumSize(120,30);

    connect(file_save,&QRadioButton::toggled,this,&MyDialog::FileSaveToggledMyDialog);
    connect(file_open,&QRadioButton::toggled,this,&MyDialog::FileOpenToggledMyDialog);

}

MyDialog::MyDialog(QString msg)
{
    mainLayout=new QVBoxLayout;
    QLabel *novi=new QLabel;
    novi->setText(msg);

    mainLayout->addWidget(novi);
    setLayout(mainLayout);
    setWindowTitle("Warning");
}

MyDialog::MyDialog(int izbor)
{
    lineEdit=new QLineEdit; //sluzi za ono sto upisujemo u listu
    editLayout=new QFormLayout;

    QLabel *novi=new QLabel;
    novi->setText("Unesi funkciju");
    editLayout->addRow(novi,lineEdit);

    editWidget=new QWidget;
    editWidget->setLayout(editLayout);

    buttonBox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

    mainLayout=new QVBoxLayout;
    mainLayout->addWidget(editWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    QString nadopuna_naslova,window_title;
    if(izbor==1){
        nadopuna_naslova="polinom";
        window_title="Unos nove "+nadopuna_naslova+ " funkcije";
    }
    else if(izbor==2){
        nadopuna_naslova="trigonometrijske";
        window_title="Unos nove "+nadopuna_naslova+ " funkcije";
    }
    else if(izbor==3){
        window_title="Unos kompleksnog broja i indeksa grafa";
        novi->setText("Unesi kompleksni broj");
        lineEditIndeks=new QLineEdit;
        QLabel *ind=new QLabel;
        ind->setText("Unesi indeks grafa");
        editLayout->addRow(ind,lineEditIndeks);
    }
    setWindowTitle(window_title);
}

MyDialog::MyDialog(MainWindow::Options selected,MainWindow::Options selected_range,const QVector<QVector<double>>&podjela_po_x,const QVector<QVector<double>>&podjela_po_y)
{
    double faktor(pow(10.0,100));
    if(selected_range==MainWindow::SetRange){
        mainLayout=new QVBoxLayout;
        range_x_poc=new QDoubleSpinBox;
        range_x_kraj=new QDoubleSpinBox;
        range_y_poc=new QDoubleSpinBox;
        range_y_kraj=new QDoubleSpinBox;
        range_x_poc->setMinimum(-100000);range_x_poc->setMaximum(pow(100.0,1000));
        range_x_kraj->setMinimum(-100000);range_x_kraj->setMaximum(pow(100.0,1000));
        range_y_poc->setMinimum(-100000);range_y_poc->setMaximum(pow(100.0,1000));
        range_y_kraj->setMinimum(-100000);range_y_kraj->setMaximum(pow(100.0,1000));
        qDebug()<<"Podjela"<<podjela_po_x[podjela_po_x.size()-1][0]<<podjela_po_x[podjela_po_x.size()-1][1];
        qDebug()<<"Podjela"<<podjela_po_y[podjela_po_y.size()-1][0]<<podjela_po_y[podjela_po_y.size()-1][1];
        range_x_poc->setDecimals(20);
        range_x_kraj->setDecimals(20);
        range_y_poc->setDecimals(20);
        range_y_kraj->setDecimals(20);
        range_x_poc->setValue(round(podjela_po_x[podjela_po_x.size()-1][0]*faktor)/faktor);
        range_x_kraj->setValue(round(podjela_po_x[podjela_po_x.size()-1][1]*faktor)/faktor);
        range_y_poc->setValue(round(podjela_po_y[podjela_po_y.size()-1][0]*faktor)/faktor);
        range_y_kraj->setValue((podjela_po_y[podjela_po_y.size()-1][1]*faktor)/faktor);

        label_x_poc=new QLabel;
        label_x_kraj=new QLabel;
        label_y_poc=new QLabel;
        label_y_kraj=new QLabel;

        label_x_poc->setText("x1");
        label_x_kraj->setText("x2");
        label_y_poc->setText("y1");
        label_y_kraj->setText("y2");

        editLayout_x=new QFormLayout;
        editLayout_y=new QFormLayout;

        editWidget_x=new QWidget;
        editWidget_y=new QWidget;

        editLayout_x->addRow(label_x_poc,range_x_poc);
        editLayout_x->addRow(label_x_kraj,range_x_kraj);
        editLayout_y->addRow(label_y_poc,range_y_poc);
        editLayout_y->addRow(label_y_kraj,range_y_kraj);

        editWidget_x->setLayout(editLayout_x);
        editWidget_y->setLayout(editLayout_y);

        mainLayout->addWidget(editWidget_x);
        mainLayout->addWidget(editWidget_y);


        buttonBox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
        connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
        connect(buttonBox,SIGNAL(rejected()),this,SLOT(close()));
        mainLayout->addWidget(buttonBox);

        setLayout(mainLayout);
        setWindowTitle("Set Range");

    }

    else if(selected==MainWindow::DarkLightMode){
        mainLayout=new QVBoxLayout;
        dark=new QRadioButton;
        light=new QRadioButton;
        dark_l=new QLabel;
        light_l=new QLabel;

        dark_l->setText("Dark mode");
        light_l->setText("Light mode");

        editLayout=new QFormLayout;
        editWidget=new QWidget;

        editLayout->addRow(dark_l,dark);
        editLayout->addRow(light_l,light);

        editWidget->setLayout(editLayout);
        mainLayout->addWidget(editWidget);

        setLayout(mainLayout);
        setWindowTitle("Mode");

        connect(dark,&QRadioButton::toggled,this,&MyDialog::setDarkMode);
        connect(light,&QRadioButton::toggled,this,&MyDialog::setLightMode);
    }


}

void MyDialog::setDarkMode()
{

    QPalette dark_mode;
    dark_mode.setColor(QPalette::Window,QColor(0,0,0));
    dark_mode.setColor(QPalette::WindowText,QColor(255,255,255));
    dark_mode.setColor(QPalette::Button,QColor(48,25,52));
    dark_mode.setColor(QPalette::Base,QColor(48,25,52));

    qApp->setPalette(dark_mode);
    emit EmitMode(true);
    accept();
}

void MyDialog::setLightMode()
{

    QPalette light_mode;
    light_mode.setColor(QPalette::Window,QColor(70,70,70));
    light_mode.setColor(QPalette::WindowText,QColor(255,255,255));
    light_mode.setColor(QPalette::Button,QColor(100,100,100));
    light_mode.setColor(QPalette::ButtonText,QColor(255,255,255));
    light_mode.setColor(QPalette::Base,QColor(60,60,60));


    qApp->setPalette(light_mode);
    emit EmitMode(false);
    accept();
}

void MyDialog::FileSaveToggledMyDialog(bool checked)
{
    emit fileSave(checked);
    accept();
}

void MyDialog::FileOpenToggledMyDialog(bool checked)
{
    emit fileOpen(checked);
    accept();
}



void MainWindow::DarkLightTriggered()
{
    selected=DarkLightMode;
    MyDialog change_mode(selected,selected_range,podjela_po_x,podjela_po_y);
    connect(&change_mode,&MyDialog::EmitMode,this,&MainWindow::ChangeMode);
    change_mode.exec();

}

void MainWindow::ChangeMode(bool DarkMode)
{
    if(DarkMode){
        selected=DarkSelected;
        ChangeUI();
    }
    else if(!DarkMode){
        selected=LightSelected;
        SetLight();
    }
}



void MyDialog::PolinomSelected()
{
    selected=Polinom;
    accept();
}

void MyDialog::TrigonSelected()
{
    selected=Trigon;
    accept();
}

void MyDialog::KompleksSelected()
{
    selected=Kompleks;
    accept();
}

MyDialog::Option MyDialog::ReturnSelected()
{
    return selected; //jer je privatna varijabla tipa enum Option
}









Kompleks::Kompleks()
{
    real=imag=0;
}

Kompleks::Kompleks(double real, double imag)
{
    int rez=Set(real,imag);
    if(!rez){
        real=imag=0;
    }
    else if(rez){
        this->real=real;
        this->imag=imag;
    }
}


double Kompleks::getReal()
{
    return this->real;
}

double Kompleks::getImag()
{
    return this->imag;
}

int Kompleks::Set(double real,double imag)
{
    if(real>10000 || imag>10000){
        QMessageBox error;
        error.setText("Preko granica kompleksnog broja.");
        error.exec();
        return 0;
    }
    this->real=real;
    this->imag=imag;
    return 1;
}

Kompleks Kompleks::operator+(const Kompleks &other)
{
    Kompleks zbroj(this->real+other.real,this->imag+other.imag);
    return zbroj;
}

Kompleks Kompleks::operator-(const Kompleks &other)
{
    Kompleks oduzimanje(this->real-other.real,this->imag-other.imag);
    return oduzimanje;
}

Kompleks Kompleks::operator*(const Kompleks &other)
{
    double real=this->real*other.real+(-1*this->imag*other.imag);
    double imag=this->imag*other.real+this->real*other.imag;

    Kompleks mnozenje(real,imag);
    return mnozenje;
}

Kompleks Kompleks::operator/(const Kompleks &other)
{
    if(other.real==0 || other.imag==0){
        QMessageBox error;
        error.setText("Dividend ne moze imati 0 kao realni ili imaginarni dio");
        error.exec();
        return Kompleks(numeric_limits<double>::quiet_NaN(),numeric_limits<double>::quiet_NaN());
    }
    double real=(this->real*other.real+this->imag*other.imag)/(pow(other.real,2)+pow(other.imag,2));
    double imag=(other.real*this->imag-this->real*other.imag)/(pow(other.real,2)+pow(other.imag,2));
    Kompleks dijelj(real,imag);
    return dijelj;
}

ostream& operator<<(ostream &os,const Kompleks &k){
    os<<k.real<<" + "<<k.imag<<" i "<<endl;
    return os;
}
QDebug operator<<(QDebug deb,const Kompleks &k){
    deb<<k.real<<" + "<<k.imag<<" i ";
    return deb;
}

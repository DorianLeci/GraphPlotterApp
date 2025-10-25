
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <list>
#include "kompleks.h"
using namespace std;
#include <QApplication>
#include <QMainWindow>
#include <QVector>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QFormLayout>
#include <qcustomplot.h>
#include <iostream>
#include <QTextStream>




QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    // ~MainWindow();

    enum Options{
        SetRange=1,
        DarkLightMode=2,
        LightSelected=3,
        DarkSelected=4,
        KompleksOn=5,
        KompleksOff=6,
        Kompleks_0=7,
        Kompleks_1=8,
        None=9
    };
private slots:
    void on_AddButton_clicked();
    void on_ResetButton_clicked();
    void SetRangeTriggered();
    void DarkLightTriggered();
    void ChangeMode(bool DarkMode);
    void onMouseWheel(QWheelEvent *event);
    void MouseDrag(QMouseEvent *event);
    void MousePress(QMouseEvent *event);
    void ShowCoordinates(QMouseEvent *event);
    void AddCNumbers();
    void SubtractCNumbers();
    void MultiplyCNumbers();
    void DivideCNumbers();
    void FileDialog();
    void FileSaveToggled(bool checked);
    void FileOpenToggled(bool checked);

private:
    typedef struct{
        double koef;
        int exp;
    }Polinom;

    typedef struct{
        string funkcija,arg;
        double koef_arg,pomak_y,pomak_x;
        int predznak;
    }Trigonom;

    Ui::MainWindow *ui;
    QVector<double>niz_x,niz_y;
    list<Polinom>funkcija_vezana_lista;
    QVector<QVector<double>>podjela_po_x;
    QVector<QVector<double>>podjela_po_y;
    QVector<double>trigon_fx_tocke_x,trigon_fx_tocke_y;
    Options selected,selected_kompleks,selected_kompleks_indeks,selected_range;
    QMessageBox *zbrajanje,*oduzimanje,*mnozenje,*dijeljenje;
    QToolBar *tool_bar;
    QAction *set_range,*mode,*zoom,*add_numbers,*subtract_numbers,*multiply_numbers,*divide_numbers,*file,*openFileAction,*saveFileAction;
    QVBoxLayout *mainLayout,*warning_msg;
    QPoint prev_mouse_pos;
    int flag_minimum,flag_maximum;
    QCPItemLine *ishodiste_y,*ishodiste_x,*kompl_broj_samo_imag_indeks_0,*kompl_broj_samo_real_indeks_0,*kompl_broj_samo_imag_indeks_1,*kompl_broj_samo_real_indeks_1;
    QCPItemLine *zbrojeni_samo_real,*zbrojeni_samo_imag,*zbrojeni_real_i_imag;
    QCPItemText *show_coord;
    QLabel *trenutna_fx,*trenutni_br1,*trenutni_action;
    Trigonom trigonom_fx;
    QVector<Kompleks> realni_kompl_br0,realni_kompl_br1,zbrojeni_kompleks,oduzeti_kompleks,pomnozeni_kompleks,dijelj_kompleks;
    QFormLayout *brojevi;
    QWidget *add_brojevi,*widget_for_row;
    QHBoxLayout *horizontal;

    void addPoint(double x,double y);
    void Clear();
    void Plot();
    void mousePressEvent(QMouseEvent *event);
    void UnosUListu(string filename);
    void UnosUListuIzDatoteke(char *funkcija);
    void UnosTrigonFxIzDatoteke(char *funkcija);
    void UnosKomplBrIzDatoteke(char *funkcija,int indeks);
    void UnosUProgram();
    void UnosTrigonF(string filename);
    void UnosKompleksBr(string filename,int indeks);
    int CheckKompleks(char *kompl_br);
    int isDecimal(char *kompl_br);
    int ZnakoviPoslijeX(char *kompl_br);
    int TrigonCheck(char *trigon_fx);
    int TrigonCheckSPredznakom(char *trigon_fx);
    void PrintListu();
    void OtvoriDijalogZaRange();
    void RenewRange(double x_poc,double x_kraj,double y_poc,double y_kraj);
    void ChangeUI();
    void SetLight();
    void ConnectZoom();
    void PrintPodjelu();
    void maintainAspectRatio();
    void DrawGraph();
    bool FuzzyEqual(double a,double b);
    void SetIshodiste();
    int ProvjeraTeksta(QString ref);
    bool ActionFind(QAction* action,QToolBar *tool_bar);
    void RemoveToolBarsKompleks();
    int ProvjeraEksponenta(list<Polinom>&temp_vezana_lista,const Polinom &ref);
    void UnosSkracenogPolinomaNatragUTekst(QString &unos_teksta);
    static bool descending(const Polinom &a,const Polinom &b);

    inline void DrawTrigonGraph(const Trigonom &struktura);
    int CheckNumbersInString(char);
    bool isNearAsymptote(double x);
    int isQVectorMatrixEmpty(QVector<QVector<double>>podjela);
    void DrawKomplNum(int indeks);
    void DrawZbrojeni();
    void DrawOduzeti();
    void DrawPomnozeni();
    void DrawDijelj();
    void ResetItemLines();
    void EmptyKompleks();
    void UnosTeksta(int indeks);

};
#endif // MAINWINDOW_H

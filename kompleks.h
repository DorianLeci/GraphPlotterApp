#ifndef KOMPLEKS_H
#define KOMPLEKS_H
#include <iostream>
using namespace std;
#include <QDebug>

class Kompleks{
public:
    Kompleks();
    Kompleks(double real,double imag);
    double getReal();
    double getImag();
    int Set(double real,double imag);
    friend ostream &operator<<(ostream &os,const Kompleks &k);
    friend QDebug operator<<(QDebug deb,const Kompleks &k);
    Kompleks operator+(const Kompleks &other);
    Kompleks operator-(const Kompleks &other);
    Kompleks operator*(const Kompleks &other);
    Kompleks operator/(const Kompleks &other);
private:
    double real,imag;

};
#endif // KOMPLEKS_H

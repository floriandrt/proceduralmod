#include <stdio.h>
#include <string.h>
#include "point.h"

using namespace std;

Point::Point(int dim){
//    cout << "construction point taille : " << dim << endl;
    n = dim;
    data = new double[dim];
}

Point::Point(const Point& p){
//     cout << "recopie d'un point" << endl;
    n = p.n;
    data = new double[n];
    for(int i = 0; i<n; i++){
        data[i] = p.data[i];
    }
}

Point::~Point(){
//     cout << "destruction de point" << endl;
    delete[] data;
}

int Point::getSize() const{
    return n;
}

ostream& operator<<( ostream &flux, Point const& p ){
    flux << "(";
    for(int i = 0; i<p.n-1; i++){
        flux << p.data[i] << ", ";
    }
    flux << p.data[p.n-1] << ")";
    return flux;
}

double& Point::operator[](int i){
    if(i<0 || i>=n){
        cerr << "Index not in the range : " << i << ", limite : " << n << endl;
    }
    return data[i];
}

Point& Point::operator=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] = p.data[i];
    }
    return *this;
}


Point& Point::operator+=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] += p.data[i];
    }
    return *this;
}


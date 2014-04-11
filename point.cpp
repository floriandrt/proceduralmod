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

double Point::operator[](int i) const{
    if(i<0 || i>=n){
        cerr << "Index not in the range : " << i << ", limite : " << n << endl;
    }
    return data[i];
}

double& Point::operator[](int i){
    if(i<0 || i>=n){
        cerr << "Index not in the range : " << i << ", limite : " << n << endl;
    }
    return data[i];
}

Point& Point::operator=(Point const& p){
    if(n == 0){
        delete[] data;
        data = new double[p.n];
        n = p.n;
    }
    if(p.n != n){
        cerr << "Points are not the same size op =" << endl;
        cerr << "taille : " << n << ", " << p.n << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] = p.data[i];
    }
    return *this;
}

Point Point::operator+(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size op +" << endl;
        cerr << "taille : " << n << ", " << p.n << endl;
    }
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] + p.data[i];
    }
    return res;
}

Point Point::operator+(int val){
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] + val;
    }
    return res;
}

Point Point::operator-(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] - p.data[i];
    }
    return res;
}

Point Point::operator-(int val){
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] - val;
    }
    return res;
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

Point& Point::operator+=(int val){
    for(int i = 0; i<n; i++){
        data[i] += val;
    }
    return *this;
}

Point& Point::operator-=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] -= p.data[i];
    }
    return *this;
}

Point& Point::operator-=(int val){
    for(int i = 0; i<n; i++){
        data[i] -= val;
    }
    return *this;
}

Point& Point::operator/=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] /= p.data[i];
    }
    return *this;
}

Point& Point::operator*=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] *= p.data[i];
    }
    return *this;
}

bool Point::operator !=(Point const& p){
    if(n != p.n){
        cerr << "comparaison de 2 points de différente dimension" << endl;
    }
    for(int i = 0; i<n; i++){
        if(data[i] != p.data[i]){
            return true;
        }
    }
    return false;
}

bool Point::operator<(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        if(data[i] >= p.data[i] ){
            return false;
        }
    }
    return true;
}

bool Point::operator>(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        if(data[i] <= p.data[i] ){
            return false;
        }
    }
    return true;
}

bool Point::operator ==(Point const& p){
    if(p.n != n){
        return false;
    }
    for(int i = 0; i<n; i++){
        if(data[i] != p.data[i]){
            return false;
        }
    }
    return true;
}

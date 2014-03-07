#include "data.h"
#include <string.h>

using namespace std;

Data::~Data(){
//    for(int i = 0; i<mean.getSize(); i++){
//        delete[] var[i];
//    }
//    delete[] var;
}

Data::Data(const Data& d){
    dataSize = d.dataSize;
    // if(data.size() != d.data.size()){
    // 	cerr << "Internal error, data are not the same size (during a recopy)" << endl;
    // }
    mean = d.mean;
    var = d.var;
    for(vector<Point>::size_type i=0; i < d.data.size(); ++i) {
        data.push_back(d.data[i]);
    }
    recentInsert = d.recentInsert;
    posInsert = d.posInsert;
}

void Data::addData(Point newData){
    cerr << "ENTREE ADD DATA" << endl;
    data.push_back(newData);
    dataSize++;
    cerr << "SORTIE ADD DATA" << endl;
}

//marche seulement dans le cas de coordonnées de points comme daa
void Data::insertData4D(Point newData, double pos){
    cout << "ENTREE INSERT DATA 4D" << endl;
    int i = 0;
    for (std::vector<Point>::iterator it = data.begin() ; it != data.end(); ++it){
        if(pos >= data[i][0] && pos < data[i][1]){
            data[i][2] = newData[0];
            data[i][3] = newData[1];
            data[i+1][0] = newData[2];
            data[i+1][1] = newData[3];
            data.insert(++it,newData);
            dataSize++;
            cout << *this << endl;
            cout << "i : " << i << endl;
            cout << "SORTIE AVEC INSERT DATA 4D" << endl;
            recentInsert = true;
            posInsert = i+1;
            return;
        }
        i++;
    }

    cout << "SORTIE SANS INSERT DATA 4D" << endl;
}

int Data::getDataSize() const{
    return dataSize;
}

Point Data::getMean(){
    return mean;
}

double** Data::getVar(){
    return var;
}

void Data::setMean(Point m){
    cerr << "ENTREE SET MEAN" << endl;
    mean = m;
    cerr << "SORTIE SET MEAN" << endl;
}

void Data::setVar(double **v){
    var = v;
}

bool Data::getRecentInsert(){
    return recentInsert;
}

int Data::getPosInsert(){
    return posInsert;
}

// Point Data::getData(int i, int j) const{
// 	return data[i][j];
// }

Point& Data::operator[](int i){
    return data[i];
}

ostream& operator<<( ostream &flux, Data const& d ){
    for(std::size_t i=0; i<d.data.size(); ++i) {
        flux << "group " << i+1 << endl;
        flux << d.data[i] << endl;
    }
    return flux;
}

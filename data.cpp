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
    mean = d.mean;
    var = d.var;
    for(vector<Point>::size_type i=0; i < d.data.size(); ++i) {
        data.push_back(d.data[i]);
    }
    for(vector<Point>::size_type i=0; i < d.sample.size(); ++i) {
        sample.push_back(d.sample[i]);
    }
    recentInsert = d.recentInsert;
    posInsert = d.posInsert;
}

void Data::addData(Point newData){
//    cerr << "ENTREE ADD DATA" << endl;
    data.push_back(newData);
    dataSize++;
//    cerr << "SORTIE ADD DATA" << endl;
}

void Data::eraseData(){
    if(posInsert+1<dataSize){
        data[posInsert+1][0] = data[posInsert][0];
        data[posInsert+1][1] = data[posInsert][1];
    }else{
        data[posInsert-1][2] = data[posInsert][2];
        data[posInsert-1][3] = data[posInsert][3];
    }
    data.erase(data.begin()+posInsert);
    dataSize--;
}

//marche seulement dans le cas de coordonnées de points comme data
void Data::insertData4D(Point newData, double pos){
    cerr << "ENTREE INSERT DATA 4D; pos : " << pos << endl;
    int i = 0;
    double temp1, temp2;
    int delta = newData[2] - newData[0] + 1;
    cerr << "deltaINSIDE : " << delta << endl;
    for (std::vector<Point>::iterator it = data.begin() ; it != data.end(); ++it){
        if(pos > data[i][0] && pos < data[i][2]){
            for(int j = i+1; j<dataSize; j++){
                data[j][0] += delta;
                data[j][2] += delta;
            }
            temp1 = data[i][2];
            temp2 = data[i][3];
            data[i][2] = newData[0];
            data[i][3] = newData[1];
            it = data.insert(++it,newData);
            dataSize++;
            newData[0] = newData[2];
            newData[1] = newData[3];
            newData[2] = temp1+delta;
            newData[3] = temp2;
            data.insert(++it,newData);
            dataSize++;
            cerr << *this << endl;
            cerr << "i : " << i << endl;
            cerr << "SORTIE AVEC INSERT DATA 4D" << endl;
            recentInsert = true;
            posInsert = i+1;
            return;
        }
        if(pos == data[i][0]){
            for(int j = i; j<dataSize; j++){
                data[j][0] += delta;
                data[j][2] += delta;
            }
            temp1 = data[i][2];
            temp2 = data[i][3];
            data[i][0] = newData[2];
            data[i][1] = newData[3];
            data.insert(it,newData);
            dataSize++;
            recentInsert = true;
            posInsert = i;
            cerr << *this << endl;
            cerr << "i : " << i << endl;
            cerr << "SORTIE AVEC INSERT DATA 4D" << endl;
            return;
        }
        if(pos == data[dataSize-1][2]){
            data[i][2] = newData[0];
            data[i][3] = newData[1];
            data.push_back(newData);
            dataSize++;
            recentInsert = true;
            posInsert = i+1;
            cerr << *this << endl;
            cerr << "i : " << i << endl;
            cerr << "SORTIE AVEC INSERT DATA 4D" << endl;
            return;
        }
        i++;
    }
    cerr << "SORTIE SANS INSERT DATA 4D" << endl;
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

vector<Point> Data::getSample(){
    return sample;
}

void Data::setMean(Point m){
    mean = m;
}

void Data::setVar(double **v){
    var = v;
}

void Data::setRecentInsert(bool b){
    recentInsert = b;
}

void Data::addSample(Point value){
    sample.push_back(value);
}

bool Data::getRecentInsert(){
    return recentInsert;
}

int Data::getPosInsert(){
    return posInsert;
}

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

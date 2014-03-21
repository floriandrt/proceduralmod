#include "data.h"
#include <string.h>

using namespace std;

Data::~Data(){

}

Data::Data(const Data& d){
    mean = d.mean;
    var = d.var;
    for(vector<Point>::size_type i=0; i < d.data.size(); ++i) {
        data.push_back(d.data[i]);
    }
    for(vector<Point>::size_type i=0; i < d.sample.size(); ++i) {
        sample.push_back(d.sample[i]);
    }
    delete[] posInsert;
    int sizePos = sizeof(d.posInsert)/sizeof(int);
    posInsert = new int[sizePos];
    for(int i = 0; i<sizePos; i++){
        posInsert[i] = d.posInsert[i];
    }
    recentInsert = d.recentInsert;
}

void Data::addData(Point newData){
    data.push_back(newData);
}

void Data::eraseData(){
    cerr << "ENTREE eraseData, posInsert : " << posInsert << endl;
    /*
    if(posInsert+1<(int)data.size()){
        data[posInsert+1][0] = data[posInsert][0];
        data[posInsert+1][1] = data[posInsert][1];
    }else{
        data[posInsert-1][2] = data[posInsert][2];
        data[posInsert-1][3] = data[posInsert][3];
    }
    data.erase(data.begin()+posInsert);
    recentInsert = false;
    */
    cerr << "SORTIE eraseData" << endl;
}

//delta < 0
int Data::reduceData(int delta, int pos){
    cerr << "ENTREE reduceData, delta : " << delta << endl;
    for (int i = 0; i < (int)data.size(); i++){
        if(pos >= data[i][0] && pos <= data[i][2]){
            int ecart = data[i][2] - data[i][0];
            //posInsert = i;
            if(delta+ecart <= 0){
                eraseData();
            }else{
                for(int j = i+1; j<(int)data.size(); j++){
                    data[j][0] += delta;
                    data[j][2] += delta;
                }
                data[i][2] += delta;
            }
            cerr << "SORTIE VRAIE reduceData" << endl;
            return delta+ecart;
        }
    }
    cerr << "SORTIE FAUSSE reduceData" << endl;
    return delta;
}

/*
void Data::updateData(int delta){
    int pos;
    for(int k = 0; k<sizePos; k++){
        pos = posInsert[k];
        if(delta < 0){
            int ecart = points[pos][2] - points[pos][0];
            if(delta+ecart <= 0){
                points.eraseData();
            }else{
                for(int j = pos+1; j<points.getDataSize(); j++){
                    points[j][0] += delta;
                    points[j][2] += delta;
                }
                points[pos][2] += delta;
            }
        }
        if(delta > 0){
            points[pos][2] += delta;
            for(int i = pos+1; i<points.getDataSize(); i++){
                points[i][0] += delta;
                points[i][2] += delta;
            }
            if(Tools::isGaussian(points[pos],points.getMean(),points.getVar(),points.getSample())){
                recentInsert[k] = false;
            }
        }
    }
}

*/
void Data::insertDataNet(){

}

//marche seulement dans le cas de coordonnées de points comme data
void Data::insertData4D(Point newData, int pos){
    cerr << "ENTREE INSERT DATA 4D; pos : " << pos << endl;
    /*
    int i = 0;
    double temp1, temp2;
    int delta = newData[2] - newData[0] + 1;
    for (std::vector<Point>::iterator it = data.begin() ; it != data.end(); ++it){
        if(pos > data[i][0] && pos < data[i][2]){
            for(int j = i+1; j<(int)data.size(); j++){
                data[j][0] += delta;
                data[j][2] += delta;
            }
            temp1 = data[i][2];
            temp2 = data[i][3];
            data[i][2] = newData[0];
            data[i][3] = newData[1];
            it = data.insert(++it,newData);
            newData[0] = newData[2];
            newData[1] = newData[3];
            newData[2] = temp1+delta;
            newData[3] = temp2;
            data.insert(++it,newData);
            cerr << *this << endl;
            cerr << "i : " << i << endl;
            cerr << "SORTIE AVEC INSERT DATA 4D" << endl;
            recentInsert = true;
            posInsert = i+1;
            return;
        }
        if(pos == data[i][0]){
            for(int j = i; j<(int)data.size(); j++){
                data[j][0] += delta;
                data[j][2] += delta;
            }
            temp1 = data[i][2];
            temp2 = data[i][3];
            data[i][0] = newData[2];
            data[i][1] = newData[3];
            data.insert(it,newData);
            recentInsert = true;
            posInsert = i;
            cerr << *this << endl;
            cerr << "i : " << i << endl;
            cerr << "SORTIE AVEC INSERT DATA 4D" << endl;
            return;
        }
        if(pos == data[data.size()-1][2]){
            data[i][2] = newData[0];
            data[i][3] = newData[1];
            data.push_back(newData);
            recentInsert = true;
            posInsert = i+1;
            cerr << *this << endl;
            cerr << "i : " << i << endl;
            cerr << "SORTIE AVEC INSERT DATA 4D" << endl;
            return;
        }
        i++;
    }
    */
    cerr << "SORTIE SANS INSERT DATA 4D" << endl;
}

int Data::getDataSize() const{
    return data.size();
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
//    recentInsert = b;
}

void Data::addSample(Point value){
    sample.push_back(value);
}

bool Data::getRecentInsert(int pos){
    return recentInsert[pos];
}

int Data::getPosInsert(){
    return 0; //posInsert;
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

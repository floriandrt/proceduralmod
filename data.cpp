#include "data.h"
#include <string.h>

using namespace std;

Data::~Data(){

}

Data::Data(const Data& d){
    mean = d.mean;
    var = d.var;
    minX = d.minX;
    minY = d.minY;
    maxX = d.maxX;
    maxY = d.maxY;
    for(vector<Point>::size_type i=0; i < d.data.size(); ++i) {
        data.push_back(d.data[i]);
    }
    for(vector<Point>::size_type i=0; i < d.sample.size(); ++i) {
        sample.push_back(d.sample[i]);
    }
    posInsert = d.posInsert;
}

int min(int a, int b){
    if(a < b){
        return a;
    }
    return b;
}

int max(int a, int b){
    if(a > b){
        return a;
    }
    return b;
}

Point Data::normalize4D(Point p){
    Point res(4);
    res[0] = 0;
    res[1] = 0;
    res[2] = p[2] - p[0];
    res[3] = p[3] - p[1];
    return res;
}

void Data::addData(Point newData){
    if((int)data.size() == 0){
        minX = min(newData[0],newData[2]);
        maxX = max(newData[0],newData[2]);
        minY = min(newData[1],newData[3]);
        maxY = max(newData[1],newData[3]);
    }else{
        minX = min(newData[0],minX);
        minX = min(newData[2],minX);
        maxX = max(newData[0],maxX);
        maxX = max(newData[2],maxX);
        minY = min(newData[1],minY);
        minY = min(newData[3],minY);
        maxY = max(newData[1],maxY);
        maxY = max(newData[3],maxY);
    }
    data.push_back(newData);
    Point norm = normalize4D(newData);
    normData.push_back(norm);
}

void Data::eraseData(int delta){
    cerr << "ENTREE eraseData, posInsert : " << posInsert << endl;
    if(posInsert+1<(int)data.size()){
        data[posInsert+1][0] = data[posInsert][0];
        data[posInsert+1][1] = data[posInsert][1];
    }else if(posInsert-1 >= 0){
        data[posInsert-1][2] = data[posInsert][2] + delta;
        data[posInsert-1][3] = data[posInsert][3];
    }
    minY = data[0][0];
    maxY = data[0][0];
    for(int i = 0; i<(int)data.size(); i++){
        minY = data[i][1] < minY ? data[i][1] : minY;
        maxY = data[i][1] < minY ? data[i][1] : minY;
        minY = data[i][3] < minY ? data[i][1] : minY;
        maxY = data[i][3] < minY ? data[i][1] : minY;
    }
    data.erase(data.begin()+posInsert);
    cerr << "SORTIE eraseData" << endl;
}

//delta < 0
int Data::reduceData(int delta, int index, bool& doClear){
    cerr << "ENTREE reduceData, delta : " << delta << endl;
    int ecart = data[index][2] - data[index][0];
    posInsert = index;
    if(delta+ecart <= 0){
        eraseData(delta);
        doClear = true;
    }
    for(int j = index+1; j<(int)data.size(); j++){
        data[j][0] += delta;
        data[j][2] += delta;
    }
    data[index][2] += delta;
    maxX += delta;
    cerr << "SORTIE VRAIE reduceData" << endl;
    return delta+ecart;
}

//marche seulement dans le cas de coordonnées de points comme data
void Data::insertData4D(Point newData, int pos, int ind){
    cerr << "ENTREE INSERT DATA 4D; pos : " << pos << "; index : " << ind << endl;
    double temp1, temp2;
    int delta = newData[2] - newData[0] + 1;
    maxX += delta;
    minY = (newData[1] < minY) ? newData[1] : minY;
    maxY = (newData[1] > maxY) ? newData[1] : maxY;
    minY = (newData[3] < minY) ? newData[3] : minY;
    maxY = (newData[3] > maxY) ? newData[3] : maxY;
    cerr << "delta : " << delta << endl;
    if(pos == data[ind][0]){
        for(int j = ind; j<(int)data.size(); j++){
            data[j][0] += delta;
            data[j][2] += delta;
        }
        data[ind][0] = newData[2];
        data[ind][1] = newData[3];
        data.insert(data.begin()+ind,newData);
        posInsert = ind;
    }else if(pos == data[data.size()-1][2]){
        data[ind][2] = newData[0];
        data[ind][3] = newData[1];
        data.push_back(newData);
        posInsert = ind+1;
    }else{
        for(int j = ind+1; j<(int)data.size(); j++){
            data[j][0] += delta;
            data[j][2] += delta;
        }
        temp1 = data[ind][2];
        temp2 = data[ind][3];
        data[ind][2] = newData[0];
        data[ind][3] = newData[1];
        data.insert(data.begin()+ind+1,newData);
        newData[0] = newData[2];
        newData[1] = newData[3];
        newData[2] = temp1+delta;
        newData[3] = temp2;
        data.insert(data.begin()+ind+2,newData);
        posInsert = ind+1;

    }
    //cerr << *this << endl;
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

void Data::addSample(Point value){
    sample.push_back(value);
}

int Data::getPosInsert(){
    return posInsert;
}

int Data::xMin(){
    return minX;
}

int Data::xMax(){
    return maxX;
}

int Data::yMin(){
    return minY;
}

int Data::yMax(){
    return maxY;
}

void Data::updateX(double newX){
    minX = (newX < minX) ? newX : minX;
    maxX = (newX > maxX) ? newX : maxX;
}

void Data::updateY(double newY){
    minY = (newY < minY) ? newY : minY;
    maxY = (newY > maxY) ? newY : maxY;
}

void Data::shiftMinX(double delta){
    minX += delta;
}

void Data::shiftMaxX(double delta){
    maxX += delta;
}

Point Data::getSample(int i){
    return sample[i];
}

int Data::getSampleSize(){
    return sample.size();
}

Point Data::getNormData(int i){
    return normData[i];
}

Point Data::operator[](int i) const{
    return data[i];
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

#include "data.h"
#include <string.h>

using namespace std;

Data::~Data(){}

Data::Data(const Data& d){
    dataSize = d.dataSize;
    // if(data.size() != d.data.size()){
    // 	cerr << "Internal error, data are not the same size (during a recopy)" << endl;
    // }
    for(vector<Point>::size_type i=0; i < d.data.size(); ++i) {
        data.push_back(d.data[i]);
    }
}

void Data::addData(Point newData){
    data.push_back(newData);
    dataSize++;
}

//marche seulement dans le cas de coordonnées de points comme daa
void Data::insertData4D(Point newData, int pos){
    vector<Point>::iterator it = data.begin()+pos;
    data.insert(it,newData);
    if(pos > 0){
        data[pos-1][2] = newData[0];
        data[pos-1][3] = newData[1];
    }
    if(pos < dataSize-1){
        data[pos+1][0] = newData[2];
        data[pos+1][1] = newData[3];
    }

}

int Data::getDataSize() const{
    return dataSize;
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

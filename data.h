#ifndef DATA_H
#define DATA_H

#include <iostream>
#include "point.h"
#include <vector>

class Data{
    private:
        int dataSize; //number of data
        std::vector<Point> data;
    public:
        Data(): dataSize(0){}
        Data(const Data& d);
        ~Data();
        void addData(Point newData);
        void insertData4D(Point newData, int pos);
        int getDataSize() const;
        // Point getData(int i, int j) const;
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H

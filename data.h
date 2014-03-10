#ifndef DATA_H
#define DATA_H

#include <iostream>
#include "point.h"
#include <vector>

class Data{
    private:
        int dataSize; //number of data
        std::vector<Point> data;
        Point mean;
        double** var;
        bool recentInsert;
        int posInsert;
    public:
        Data(): dataSize(0), mean(0), var(NULL), recentInsert(false), posInsert(-1){}
        Data(const Data& d);
        ~Data();
        void addData(Point newData);
        void insertData4D(Point newData, double pos);
        int getDataSize() const;
        Point getMean();
        double** getVar();
        void setMean(Point m);
        void setVar(double** v);
        void setRecentInsert(bool b);
        bool getRecentInsert();
        int getPosInsert();
        // Point getData(int i, int j) const;
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H

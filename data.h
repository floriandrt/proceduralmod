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
        std::vector<Point> sample;
    public:
        Data(): dataSize(0), data(), mean(0), var(NULL), recentInsert(false), posInsert(-1), sample(){}
        Data(const Data& d);
        ~Data();
        void addData(Point newData);
        void eraseData();
        void insertData4D(Point newData, double pos);
        int getDataSize() const;
        Point getMean();
        double** getVar();
        std::vector<Point> getSample();
        void setMean(Point m);
        void setVar(double** v);
        void setRecentInsert(bool b);
        void addSample(Point value);
        bool getRecentInsert();
        int getPosInsert();
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H

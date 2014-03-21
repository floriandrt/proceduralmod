#ifndef DATA_H
#define DATA_H

#include <iostream>
#include "point.h"
#include <vector>

class Data{
    private:
//        int dataSize; //number of data
        std::vector<Point> data;
        std::vector<Point> base;
        std::vector<Point> sample;
        Point mean;
        double** var;
        bool* recentInsert;
        int* posInsert;
    public:
        Data(): data(), base(), sample(), mean(0), var(NULL), recentInsert(false), posInsert(){}
        Data(const Data& d);
        ~Data();
        void addData(Point newData);
        void eraseData();
        int reduceData(int delta, int pos);
        void updateDate(int delta);
        void insertDataNet();
        void insertData4D(Point newData, int pos);
        int getDataSize() const;
        Point getMean();
        double** getVar();
        std::vector<Point> getSample();
        void setMean(Point m);
        void setVar(double** v);
        void setRecentInsert(bool b);
        void addSample(Point value);
        bool getRecentInsert(int pos);
        int getPosInsert();
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H

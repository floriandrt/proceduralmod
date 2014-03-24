#ifndef DATA_H
#define DATA_H

#include <iostream>
#include "point.h"
#include <vector>

class Data{
    private:
//        int dataSize; //number of data
        std::vector<Point> data;
        std::vector<Point> sample;
        Point mean;
        double** var;
        bool* recentInsert;
        int* posInsert;
        int minX;
        int maxX;
        int minY;
        int maxY;
    public:
        Data(): data(), sample(), mean(0), var(NULL), recentInsert(false), posInsert(), minX(0), maxX(0), minY(0), maxY(0){}
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
        int xMin();
        int xMax();
        int yMin();
        int yMax();
        bool has(int x, int y);
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H

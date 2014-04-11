#ifndef DATA_H
#define DATA_H

#include <iostream>
#include "point.h"
#include <vector>

class Data{
    private:
//        int dataSize; //number of data
        std::vector<Point> data;
        std::vector<Point> normData;
        std::vector<Point> sample;
        Point mean;
        double** var;
        int posInsert;
        int minX;
        int maxX;
        int minY;
        int maxY;
    public:
        Data(): data(), sample(), mean(0), var(NULL), posInsert(-1), minX(0), maxX(0), minY(0), maxY(0){}
        Data(const Data& d);
        ~Data();
        static Point normalize4D(Point p);
        void addData(Point newData);
        void eraseData(int delta);
        int reduceData(int delta, int index, bool& doClear);
        void updateDate(int delta);
        void insertData4D(Point newData, int pos, int ind);
        int getDataSize() const;
        bool isGaussian();
        Point getMean();
        double** getVar();
        std::vector<Point> getSample();
        void setMean(Point m);
        void setVar(double** v);
        void addSample(Point value);
        int getPosInsert();
        int xMin();
        int xMax();
        int yMin();
        int yMax();
        void updateX(double newX);
        void updateY(double newY);
        void shiftMinX(double delta);
        void shiftMaxX(double delta);
        Point getSample(int i);
        int getSampleSize();
        Point getNormData(int i);
        Point operator[](int i) const;
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H

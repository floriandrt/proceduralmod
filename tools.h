#ifndef TOOLS_H
#define TOOLS_H

#include "data.h"
#include "point.h"

class Tools{
    public :
        Tools() {};
        // static Point* dotMat2D(Point a, Point b);
        static double* average(double **data, int dim, int size);
        static Point averageMulDim(Data& d);
        // static Point* average2(Data const& d);
        static double* variance(double **data, double mean[], int dim, int size);
        static double** varianceMulDim(Data& d, Point& mean);
        // static Point* variance2(Data const& d, Point *mean);
        static double gaussian(double x, double mean, double variance);
        // static Point gaussian2D(Point x, Point mean, Point* variance);
        static double generateGaussian(double mean, double variance);
        static Point generateMulDim(Point mean, double **variance);
        static Point rejet(Point min, Point max, Point mean, double** variance);
};

#endif //TOOLS_H

#include "tools.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace std;

// Point sqrt2D(Point a){
// 	return Point(sqrt(a[0]), sqrt(a[1]));
// }


// Point* Tools::dotMat2D(Point a, Point b){
// 	Point *res = new Point[2];
// 	res[0].setX(a[0]*b[0]);
// 	res[0].setY(a[0]*b[1]);
// 	res[1].setX(a[1]*b[0]);
// 	res[1].setY(a[1]*b[1]);
// 	return res;
// }

Point mult(double** const A, Point x){
    Point res(x.getSize());
    for(int i = 0; i<x.getSize(); i++){
        res[i] = 0;
        for(int j = 0; j<x.getSize(); j++){
            res[i] += A[i][j]*x[j];
        }
    }
    return res;
}

//méthode de calcul d'une racine carrée d'une matrice
double** cholesky(double** A, int size){
    double** L = new double*[size];
    double sum;
    double prod = 0;
    for(int i = 0; i<size; i++){
        sum = 0;
        L[i] = new double[size];
        for(int j = 0; j<size; j++){
            if(i>j){
                prod = 0;
                for(int k = 0; k<j; k++){
                    prod += L[j][k]*L[i][k];
                }
                L[i][j] = A[j][i]-prod;
                L[i][j] /= L[j][j];
                sum += pow(L[i][j],2);
            }else if(i == j){
                L[i][i] = sqrt(A[i][i]-sum);
            }else{
                L[i][j] = 0;
            }
        }
    }
    return L;
}

double* Tools::average(double **data, int dim, int size){
    double *result = new double[dim];
    for(int i = 0; i<dim; i++){
        result[i] = 0;
        for(int j = 0; j<size; j++){
            result[i] += data[i][j];
        }
        result[i] /= size;
    }
    return result;
}

Point Tools::averageMulDim(Data& d){
    int dim = d[0].getSize();
    Point res(dim);
    for(int i = 0; i<dim; i++){
        res[i] = 0;
        for(int j = 0; j<d.getDataSize(); j++){
            res[i] += d[j][i];
        }
        res[i] /= d.getDataSize();
    }
    return res;
}

// Point* Tools::average2(Data const& d){
// 	Point *result = new Point[d.getGroupSize()];
// 	for(int j = 0; j<d.getGroupSize(); j++){
// 		result[j] = 0;
// 		for(int i = 0; i<d.getDataSize(); i++){
// 			result[j] += d.getData(i,j);
// 		}
// 		result[j] /= d.getDataSize();
// 	}
// 	return result;
// }

double* Tools::variance(double **data, double mean[], int dim, int size){
    double *result = new double[dim];
    for(int i = 0; i<dim; i++){
        result[i] = 0;
        for(int j = 0; j<size; j++){
            result[i] += (data[i][j]*data[i][j])-(mean[i]*mean[i]);
        }
        result[i] /= size;
        result[i] = sqrt(result[i]);
    }
    return result;
}

double** Tools::varianceMulDim(Data& d, Point mean){
    int dim = mean.getSize();
    double** res = new double*[dim];
    for(int i = 0; i<dim; i++){
        res[i] = new double[dim];
        for(int j = 0; j<dim; j++){
            for(int k = 0; k<d.getDataSize(); k++){
                res[i][j] += (d[k][i]-mean[i])*(d[k][j]-mean[j]);
            }
            res[i][j] /= d.getDataSize();
        }
    }
    return res;
}

// Point* Tools::variance2(Data const& d, Point *mean){
// 	Point *result = new Point[2];
// 	Point *temp = new Point[2];
// 	for(int i = 0; i<d.getGroupSize(); i++){
// 		for(int j = 0; j<d.getDataSize(); j++){
// 			temp = dotMat2D(d.getData(j,i)-mean[i],d.getData(j,i)-mean[i]);
// 			result[0] += temp[0];
// 			result[1] += temp[1];
// 		}
// 	}
// 	result[0] /= d.getDataSize();
// 	result[1] /= d.getDataSize();
// 	result[0] = sqrt2D(result[0]);
// 	result[1] = sqrt2D(result[1]);
// 	return result;
// }

//double det(Point* a){
//    return a[0][0]*a[1][1]-(a[0][1]*a[1][0]);
//}

//a doit être de taille 2
// Point* inv2D(Point* a){
// 	if(det(a) == 0){
// 		cerr << "Non inversible matrix" << endl;
// 		return NULL;
// 	}
// 	Point *res = new Point[2];
// 	res[0].setX(a[1][1]);
// 	res[0].setY(-a[0][1]);
// 	res[1].setX(-a[1][0]);
// 	res[1].setY(a[0][0]);
// 	return res;
// }

double Tools::gaussian(double x, double mean, double variance){
    return (1/(sqrt(2*M_PI)*variance))*exp(-0.5*pow((x-mean)/variance,2));
}

// Point Tools::gaussian2D(Point x, Point mean, Point* variance){
// 	Point xB(x-mean);
// 	Point *invVar = inv2D(variance);
// 	(1/(2*M_PI*sqrt(det(variance))))*exp(-0.5*(invVar[0][0]*xB[0]*xB[0]+invVar[1][1]*xB[1]*xB[1]+(invVar[0][1]+invVar[1][0])*xB[0]*xB[1]));
// }

double Tools::generateGaussian(double mean, double variance){
    double temp = sqrt(-2*log((double)rand()/RAND_MAX))*cos(2*M_PI*(double)rand()/RAND_MAX);
    return temp*variance+mean;
}

Point Tools::generateMulDim(Point mean, double **variance){
    int dim = mean.getSize();
    double **A = cholesky(variance,dim);
    //x suit une loi N(0,I)
    Point x(dim);
    //y suit une loi N(mean,variance)
    Point y(dim);
    for(int i = 0; i<dim; i++){
        x[i] = Tools::generateGaussian(0,1);
    }
    y = mult(A,x);
    y += mean;
    return y;
}

Point Tools::generateY(Point mean, double** variance, double x1, double x2){
    Point y = generateMulDim(mean, variance);
    y[0] = x1;
    y[2] = x2;
    return y;
}

Point Tools::rejet(Point min, Point max, Point mean, double** variance, int limit){
    Point test = generateMulDim(mean, variance);
    for(int i = 0; i<limit; i++){
        if(test > min && test < max){
            return test;
        }
    }
    return Point();
}

bool Tools::isGaussian(Point x, Point mean, double** var){
    Point min(x);
    Point max(x);
    min -= 30;
    max += 30;
    Point res = rejet(min, max, mean, var, 100000);

    return !(res.getSize() == 0);
}

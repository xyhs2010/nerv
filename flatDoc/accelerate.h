/*
 * accelerate.h
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#ifndef ACCELERATE_H_
#define ACCELERATE_H_



#ifdef __cplusplus
extern "C" {
#endif
#include <lapacke.h>
#include <cblas.h>
#ifdef __cplusplus
}
#endif

//#include "appleclapack.h"

typedef struct {
	double *data;
	int cols;
	int rows;
	bool col_major;
} Acmat;

Acmat *acmatLikeMat(Acmat *origin);
Acmat *changeMajor(Acmat *pmat);
void traverseMat(Acmat *mat, void (*func)(Acmat *, int, int));
void destroyMat(Acmat *mat);

inline double valueAt(Acmat *mat, int ic, int ir) {
	if (mat->col_major) {
		return mat->data[ic * mat->rows + ir];
	} else {
		return mat->data[ir * mat->cols + ic];
	}
}

inline void setvalue(double value, Acmat *mat, int ic, int ir) {
	if (mat->col_major) {
		mat->data[ic * mat->rows + ir] = value;
	} else {
		mat->data[ir * mat->cols + ic] = value;
	}
}


double acmean(double *values, double *weights, int num);
double acvariance(double *values, double *weights, int num);

double acmaxIndex(double *values, int num) ;
double acminIndex(double *values, int num) ;


#endif /* ACCELERATE_H_ */

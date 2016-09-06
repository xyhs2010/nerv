/*
 * accelerate.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#include "accelerate.h"
#include <assert.h>

void tranversMat(Acmat *mat, void (*func)(Acmat *, int, int)) {
	for (int index = 0; index < mat->cols * mat->rows; index++) {
		int ic, ir;
		if (mat->col_major) {
			ic = index / mat->rows;
			ir = index % mat->rows;
		} else {
			ir = index / mat->cols;
			ic = index % mat->cols;
		}
		(*func)(mat, ic, ir);
	}
}

double valueAt(Acmat *mat, int ic, int ir) {
	if (mat->col_major) {
		return mat->data[ic * mat->rows + ir];
	} else {
		return mat->data[ir * mat->cols + ic];
	}
}

void destroyMat(Acmat *mat) {
	free(mat->data);
}

double acmean(double *values, double *weights, int num) {
	if (num <= 0) {
		return 0;
	}
	assert(values != NULL);

	double sum = 0, whole = 0;
	if (weights == NULL) {
		for (int i = 0; i < num; i++) {
			sum += values[i];
			whole += 1;
		}
	} else {
		for (int i = 0; i < num; i++) {
			sum += weights[i] * values[i];
			whole += weights[i];
		}
	}
	if (whole == 0)
		return 0;
	return sum / whole;
}

double acvariance(double *values, double *weights, int num) {
	if (num <= 0) {
		return 0;
	}
	assert(values != NULL);

	double mean = acmean(values, weights, num);
	for (int i = 0; i < num; i++)
		values[i] = values[i] * values[i];
	double sqmean = acmean(values, weights, num);
	return sqmean - (mean * mean);
}


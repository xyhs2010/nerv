/*
 * accelerate.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#include "accelerate.h"
#include <assert.h>

void traverseMat(Acmat *mat, void (*func)(Acmat *, int, int)) {
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

double acmaxIndex(double *values, int num) {
	if (num <= 0) {
		return 0;
	}
	assert(values != NULL);

	double max = values[0], index = 0;

	for (int i = 0; i < num; i++) {
		if (values[i] > max) {
			index = i;
			max = values[i];
		}
	}
	return index;
}

double acminIndex(double *values, int num) {
	if (num <= 0) {
		return 0;
	}
	assert(values != NULL);

	double min = values[0], index = 0;

	for (int i = 0; i < num; i++) {
		if (values[i] < min) {
			index = i;
			min = values[i];
		}
	}
	return index;
}


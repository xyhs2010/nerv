/*
 * accelerate.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#include "accelerate.h"

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

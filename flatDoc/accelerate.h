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

void tranversMat(Acmat *mat, void (*func)(Acmat *, int, int));

double valueAt(Acmat *mat, int ic, int ir);


#endif /* ACCELERATE_H_ */
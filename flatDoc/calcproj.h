/*
 * calcproj.h
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#ifndef CALCPROJ_H_
#define CALCPROJ_H_


#include "accelerate.h"

typedef struct {
	Acmat *mat;
	int startc;
	int endc;
	int startr;
	int endr;
	int centerc;
	int centerr;
	int radius;
	double fstang;
	double secang;
	double belief;
} Acblock;

typedef struct {
	Acblock *blocks;
	int cols;
	int rows;
	bool col_major;
} Acblockarray;

Acblockarray createBlocks(Acmat *mat, Acblock **pblocks);
void destroyBlockArray(Acblockarray *array);



#endif /* CALCPROJ_H_ */

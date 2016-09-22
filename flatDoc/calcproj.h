/*
 * calcproj.h
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#ifndef CALCPROJ_H_
#define CALCPROJ_H_


#include "accelerate.h"

#define PROJ_GRIDS_NUM (9)

typedef struct {
	Acmat *mat;
	int startc;
	int endc;
	int startr;
	int endr;
	int centerc;
	int centerr;
	double radius;

	double minWeight;
	double maxWeight;
	double minAngle;
	double maxAngle;
	bool useful;
} Acblock;

typedef struct {
	Acblock *blocks;
	int cols;
	int rows;
	bool col_major;
} Acblockarray;

Acblockarray createBlocks(Acmat *mat);
void destroyBlockArray(Acblockarray *array);

double projStdAtAngle(double angle, Acblock *block);

void projStdsAtAngles(const double *angles, double *stds, int num,  Acblock *block);

void obtainNeibourAcblocks(Acblockarray *array, int index, int *neibourIndexs);

#endif /* CALCPROJ_H_ */

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
#define ANGLE_NUM (26)
#define MAX_PAGE (10)
#define FIT_ORDER (4)

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
	Acmat *mat;
	bool h_major;
} Acblockarray;

Acblockarray createBlocks(Acmat *mat);
void destroyBlockArray(Acblockarray *array);

double projStdAtAngle(double angle, Acblock *block);

void projStdsAtAngles(const double *angles, double *stds, int num,  Acblock *block);

void obtainNeibourAcblocks(Acblockarray *array, int index, int *neibourIndexs);

void blocksFilter(Acblockarray *parray);

int blocksSeg(Acblockarray *parray, int (*segments)[2]);

void makeVec(double x, double y, double *vec);

void makeSigVec(double x, double y, double *vec);

void polyfit(Acblockarray *parray, double *zs);

void rectMat(Acblockarray *parray, Acmat *pdesmat, double *zs);

void traverseMatLocal(Acmat *pmat, Acmat *pout, int rad, void (*callback)(void *, double));

#endif /* CALCPROJ_H_ */

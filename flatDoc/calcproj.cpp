/*
 * calcproj.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#include "calcproj.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

Acblockarray createBlocks(Acmat *mat) {
	int maxL = mat->cols > mat->rows ? mat->cols : mat->rows;
	int radius = maxL / 35;
	if (radius < 8)
		radius = 8;
	int blockwid = 2 * radius + 1;
	int numc = mat->cols / blockwid;
	int numr = mat->rows / blockwid;
	Acblockarray blockarray;
	blockarray.mat = mat;
	blockarray.col_major = true;
	blockarray.cols = numc;
	blockarray.rows = numr;
	if (numc == 0 || numr == 0) {
		blockarray.blocks = NULL;
		return blockarray;
	}
	Acblock *blocks = (Acblock *)malloc(numc * numr * sizeof(Acblock));
	int index = 0;
	for (int ic = 0; ic < numc; ic++) {
		for (int ir = 0; ir < numr; ir++) {
			blocks[index].mat = mat;
			blocks[index].radius = radius;
			blocks[index].startc = ic * blockwid;
			blocks[index].startr = ir * blockwid;
			blocks[index].centerc = blocks[index].startc + blocks[index].radius;
			blocks[index].centerr = blocks[index].startr + blocks[index].radius;
			if (ic == numc - 1) {
				blocks[index].endc = mat->cols;
			} else {
				blocks[index].endc = blocks[index].startc + blockwid;
			}
			if (ir == numr - 1) {
				blocks[index].endr = mat->rows;
			} else {
				blocks[index].endr = blocks[index].startr + blockwid;
			}
			index++;
		}
	}
	blockarray.blocks = blocks;
	return blockarray;
}

void destroyBlockArray(Acblockarray *array) {
	free(array->blocks);
}

double projStdAtAngle(double angle, Acblock *block) {
	double accum[PROJ_GRIDS_NUM] = {0};
	double entire[PROJ_GRIDS_NUM] = {0};
	double grid = (2 * block->radius + 1) / (PROJ_GRIDS_NUM);
	double x, y, proj;
	for (int ic = block->startc; ic < block->endc; ic++) {
		for (int ir = block->startr; ir < block->endr; ir++) {
			x = ic - block->centerc;
			y = ir - block->centerr;
			if (x * x + y * y <= block->radius * block->radius) {
				proj = y * cos(angle) - x * sin(angle);
				int gridindex = (proj + block->radius) / grid;
				if (gridindex < 0 || gridindex >= PROJ_GRIDS_NUM) {
					printf("grid error");
				}
				entire[gridindex] += 1;
				accum[gridindex] += valueAt(block->mat, ic, ir);
			}
		}
	}
	for (int i = 0; i < PROJ_GRIDS_NUM; i++) {
		if (entire[i] > 0) {
			accum[i] /= entire[i];
		}
	}
	return sqrt(acvariance(accum, entire, PROJ_GRIDS_NUM));
}

void projStdsAtAngles(const double *angles, double *stds, int num,  Acblock *block) {
	double ** accums = (double **)malloc(num * sizeof(double *));
	for (int i = 0; i < num; i++) {
		accums[i] = (double *)malloc(PROJ_GRIDS_NUM * sizeof(double));
		for (int j = 0; j < PROJ_GRIDS_NUM; j++)
			accums[i][j] = 0;
	}
	double ** entires = (double **)malloc(num * sizeof(double *));
	for (int i = 0; i < num; i++) {
		entires[i] = (double *)malloc(PROJ_GRIDS_NUM * sizeof(double));
		for (int j = 0; j < PROJ_GRIDS_NUM; j++)
			entires[i][j] = 0;
	}
	double grid = (2 * block->radius + 1) / (PROJ_GRIDS_NUM);
	double x, y, proj;
	for (int ic = block->startc; ic < block->endc; ic++) {
		for (int ir = block->startr; ir < block->endr; ir++) {
			x = ic - block->centerc;
			y = ir - block->centerr;
			if (x * x + y * y <= block->radius * block->radius) {
				for (int i = 0; i < num; i++) {
					double angle = angles[i];
					proj = y * cos(angle) - x * sin(angle);
					int gridindex = (proj + block->radius) / grid;
					if (gridindex < 0 || gridindex >= PROJ_GRIDS_NUM) {
						printf("grid error");
					}
					entires[i][gridindex] += 1;
					accums[i][gridindex] += valueAt(block->mat, ic, ir);
				}
			}
		}
	}
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < PROJ_GRIDS_NUM; j++) {
			if (entires[i][j] > 0) {
				accums[i][j] /= entires[i][j];
			}
		}
		stds[i] = sqrt(acvariance(accums[i], entires[i], PROJ_GRIDS_NUM));
		free(accums[i]);
		free(entires[i]);
	}
	free(accums);
	free(entires);
}

void obtainNeibourAcblocks(Acblockarray *array, int index, int *neibourIndexs) {
	int ic, ir;
	for (int i = 0; i < 4; i++)
		neibourIndexs[i] = -1;

	if (array->col_major) {
		ic = index / array->rows;
		ir = index % array->rows;
	} else {
		ic = index % array->cols;
		ir = index / array->cols;
	}

	if (array->col_major) {
		if (ic > 0)
			// left
			neibourIndexs[3] = index - array->rows;
		if (ic < array->cols - 1)
			// right
			neibourIndexs[1] = index + array->rows;
		if (ir > 0)
			// up
			neibourIndexs[0] = index - 1;
		if (ir < array->rows - 1)
			// down
			neibourIndexs[2] = index + 1;
	} else {
		if (ic > 0)
			// left
			neibourIndexs[3] = index - 1;
		if (ic < array->cols - 1)
			// right
			neibourIndexs[1] = index + 1;
		if (ir > 0)
			// up
			neibourIndexs[0] = index - array->cols;
		if (ir < array->rows - 1)
			// down
			neibourIndexs[2] = index + array->cols;
	}
}

void intextfilter(Acblock *pblock) {
	int intextLocal = 10;
	bool intext[4];
	int orc, orr;
	for (int oind = 0; oind < 4; oind++) {
		orc = oind % 2 == 1 ? 1 : -1;
		orr = oind / 2 == 1 ? 1 : -1;
		int ic, ir;
		bool ointext = false;
		for (int dc = 0; dc <= intextLocal; dc++) {
			ic = pblock->centerc + orc * dc;
			if (ic < 0 || ic >= pblock->mat->cols)
				continue;
			for (int dr = 0; dr <= intextLocal; dr++) {
				ir = pblock->centerr + orr * dr;
				if (ir < 0 || ir >= pblock->mat->rows)
					continue;
				if (valueAt(pblock->mat, ic, ir) < 138) {
					ointext = true;
					break;
				}
			}
			if (ointext == true)
				break;
		}
		intext[oind] = ointext;
	}
	if (!((intext[0] & intext[3]) | (intext[1] & intext[2]))) {
		pblock->useful = false;
	}
}

void blocksFilter(Acblockarray *parray) {
	double angles[ANGLE_NUM];
	double stds[ANGLE_NUM];
	for (int i = 0; i < ANGLE_NUM; i++) {
		angles[i] = (M_PI * i) / ANGLE_NUM;
	}

	int mini, maxi;
	double angle;
	for (int i = 0; i < parray->cols * parray->rows; i++) {
		Acblock *pblock = parray->blocks + i;
		projStdsAtAngles(angles, stds, ANGLE_NUM, pblock);
		maxi = acmaxIndex(stds, ANGLE_NUM);
		mini = acminIndex(stds, ANGLE_NUM);
		pblock->maxAngle = angles[maxi];
		pblock->maxWeight = stds[maxi];
		pblock->minAngle = angles[mini];
		pblock->minWeight = stds[mini];
		pblock->useful = true;

//		if (stds[maxi] - stds[mini] < 5 ||
//				stds[maxi] / stds[mini] < 2) {
//			pblock->useful = false;
//		}
	}

	for (int i = 0; i < parray->cols * parray->rows; i++)
		intextfilter(parray->blocks + i);

//	for (int i = 0; i < parray->cols * parray->rows; i++) {
//		Acblock *pblock = parray->blocks + i;
//		int neibourIndexs[4];
//		int wrongSum = 0;
//		double neibAngle;
//		obtainNeibourAcblocks(parray, i, neibourIndexs);
//		for (int j = 0; j < 4; j++) {
//			if (neibourIndexs[j] < 0 ||
//					!parray->blocks[neibourIndexs[j]].useful) {
//				continue;
//			}
//			neibAngle = parray->blocks[neibourIndexs[j]].maxAngle;
//			if (abs(neibAngle - pblock->maxAngle) > M_PI/4)
//				wrongSum++;
//		}
//		if (wrongSum > 1) {
//			pblock->useful = false;
//		}
//	}

	Acblock *vpblock[1000], *hpblock[1000];
	int vi = 0, hi = 0;
	for (int i = 0; i < parray->cols * parray->rows; i++) {
		Acblock *pblock = parray->blocks + i;
		if (pblock->maxAngle < M_PI / 6 || pblock->maxAngle > M_PI * 5 / 6)
			hpblock[hi++] = pblock;
		else if (pblock->maxAngle > M_PI / 3 && pblock->maxAngle < M_PI * 2 / 3)
			vpblock[vi++] = pblock;
	}
	if (hi > vi) {
		parray->h_major = true;
		for (int i = 0; i < vi; i++)
			vpblock[i]->useful = false;
	} else {
		parray->h_major = false;
		for (int i = 0; i < hi; i++)
			hpblock[i]->useful = false;
	}

}

int blocksSeg(Acblockarray *parray, int (*segments)[2]) {
	int segnum = 0;
	int segL = parray->h_major ? parray->cols : parray->rows;
	double *sum = (double *)malloc(segL * sizeof(double));
	int *num = (int *)malloc(segL * sizeof(int));
	for (int i = 0; i < segL; i++) {
		sum[i] = 0;
		num[i] = 0;
	}
	int ir, ic;
	Acblock *pblock;
	for (int i = 0; i < parray->cols * parray->rows; i++) {
		pblock = parray->blocks + i;
		if (parray->h_major) {
			pblock->maxAngle += M_PI / 2;
			if (pblock->maxAngle > M_PI) {
				pblock->maxAngle -= M_PI;
			}
		}
		if (pblock->useful) {
			if (parray->col_major) {
				ic = i / parray->rows;
				ir = i % parray->rows;
			} else {
				ic = i % parray->cols;
				ir = i / parray->cols;
			}
			if (parray->h_major) {
				sum[ic] += pblock->maxAngle;
				num[ic] += 1;
			} else {
				sum[ir] += pblock->maxAngle;
				num[ir] += 1;
			}
		}
	}

	for (int i = 0; i < segL; i++) {
		if (num[i] > 0)
			sum[i] /= num[i];
		printf("%d  sum: %f, num: %d\n ", i, sum[i], num[i]);
	}

	int lastb, j; double diff;
	int blockwid = parray->blocks[0].endc - parray->blocks[0].startc;
	int lastSeg = 1;
	for (int i = 2; i < segL - 1; i++) {
		if (num[i] >= 3 && num[i + 1] >=3) {
			j = i;
			lastb = i;
			while (--j > lastSeg) {
				if (num[j] > 3 && num[j - 1] > 3) {
					lastb = j;
					break;
				}
			}
			if (lastb != i) {
				diff = sum[i] - sum[lastb];
				if (fabs(diff) > M_PI/6) {
					if (diff * (sum[i+1] - sum[i]) < 0 && diff * (sum[lastb] - sum[lastb-1]) < 0) {
						segments[segnum][0] = (lastb + 0.5) * blockwid;
						segments[segnum][1] = (i + 0.5) * blockwid;
						segnum++;
						lastSeg = i;
					}
				}
			}
		}
	}

	free(sum);
	free(num);
	return segnum;
}

inline void localIter(Acmat *pmat, int ic, int ir, int rad, void *handler, void (*callback)(void *, double)) {
	for (int i = ic - rad; i < ic + rad; i++) {
		if (i < 0 || i >= pmat->cols)
			continue;
		for (int j = ir - rad; j < ir + rad; j++) {
			if (j < 0 || j >= pmat->rows)
				continue;
			callback(handler, valueAt(pmat, i, j));
		}
	}
}

void traverseMatLocal(Acmat *pmat, Acmat *pout, int rad, void (*callback)(void *, double)) {
	if (pout == NULL) {
		pout = acmatLikeMat(pmat);
	}
	if (pout->data == NULL)
		pout->data = (double *)malloc(pout->rows * pout->cols * sizeof(double));

	double newval;
	for (int ic = 0; ic < pout->cols; ic++)
		for (int ir = 0; ir < pout->rows; ir++) {
			newval = valueAt(pmat, ic, ir);
			localIter(pmat, ic, ir, rad, &newval, callback);
			setvalue(newval, pout, ic, ir);
		}
}

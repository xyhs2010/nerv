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

Acblockarray createBlocks(Acmat *mat) {
	int maxL = mat->cols > mat->rows ? mat->cols : mat->rows;
	int radius = maxL / 28;
	if (radius < 8)
		radius = 8;
	int blockwid = 2 * radius + 1;
	int numc = mat->cols / blockwid;
	int numr = mat->rows / blockwid;
	Acblockarray blockarray;
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
			blocks[index].belief = -1;
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
				proj = x * cos(angle) + y * sin(angle);
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

/*
 * calcproj.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: xyhs
 */

#include "calcproj.h"

Acblockarray createBlocks(Acmat *mat, Acblock **pblocks) {
	int maxL = mat->cols > mat->rows ? mat->cols : mat->rows;
	int radius = maxL / 28;
	if (radius < 4)
		radius = 4;
	int blockwid = 2 * radius + 1;
	int numc = mat->cols / blockwid;
	int numr = mat->rows / blockwid;
	if (numc == 0 || numr == 0) {
		return 0;
	}
	Acblock *blocks = (Acblock *)malloc(numc * numr * sizeof(Acblock));
	*pblocks = blocks;
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
	Acblockarray blockarray;
	blockarray.blocks = blocks;
	blockarray.col_major = true;
	blockarray.cols = numc;
	blockarray.rows = numr;
	return blockarray;
}

void destroyBlockArray(Acblockarray *array) {
	free(array->blocks);
}

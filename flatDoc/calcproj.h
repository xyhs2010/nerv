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

int createBlocks(Acmat *mat, Acblock **pblocks);



#endif /* CALCPROJ_H_ */

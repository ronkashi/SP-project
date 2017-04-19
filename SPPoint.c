/*
 * SPPoint.c
 *
 *  Created on: Dec 25, 2016
 *      Author: gavrielc
 */
#include "SPPoint.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

int SortCoor;
struct sp_point_t {
	int dim;
	int index;
	double* data;
};

SPPoint* spPointCreate(double* data, int dim, int index) {
	SPPoint* point;
	double *point_array;
	int i;
	if (data == NULL || dim <= 0 || index < 0) {
		return NULL;
	}
	point = (SPPoint*) malloc(sizeof(*point));
	if (point == NULL) {
		return NULL;
	}
	point_array = (double*) malloc(dim * sizeof(double));
	if (point_array == NULL) {
		free(point);
		return NULL;
	}
	for (i = 0; i < dim; i++) {
		point_array[i] = data[i];
	}
	point->index = index;
	point->dim = dim;
	point->data = point_array;
	return point;
}

SPPoint* spPointCopy(SPPoint* source) {
	assert(source != NULL);
	return spPointCreate(source->data, source->dim, source->index);
}

void spPointDestroy(SPPoint* point) {
	if (point != NULL) {
		free(point->data);
		free(point);
	}
	return;
}

int spPointGetDimension(SPPoint* point) {
	assert(point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint* point) {
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint* point, int axis) {
	assert(point != NULL);
	assert(axis < point->dim);
	return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q) {
	assert(p != NULL && q != NULL);
	assert(p->dim == q->dim);
	double dist = 0;
	int i;
	for (i = 0; i < p->dim; i++) {
		dist += (p->data[i] - q->data[i]) * (p->data[i] - q->data[i]);
	}
	return dist;
}
SPPoint* spPointAddDimention(SPPoint* p, double val) {
	if (p == NULL) {
		return NULL;
	}

	int dim;
	int index;
	double* data;
	int i;

	dim = spPointGetDimension(p);
	index = spPointGetIndex(p);
	data = (double*) calloc(dim + 1, sizeof(*data));
	if (NULL == data) {
		spPointDestroy(p);
		return NULL;
	}
	for (i = 0; i < dim; i++) {
		data[i] = spPointGetAxisCoor(p, i);
	}
	data[dim] = val;
	spPointDestroy(p);
	SPPoint* RT_POINT =  spPointCreate(data, dim +1 , index);
	free(data);
	return RT_POINT;
}


int* spPointSortByCoor(SPPoint** arr, int size, int coor) {

	int* indexes;
	int i;
	indexes = (int*) calloc(size, sizeof(indexes));
	if (NULL == indexes) {
		return NULL;
	}
	SortCoor = coor;
	qsort(arr, size, sizeof(SPPoint*),cmpfunc);
	for (i = 0; i < size; i++) {
		indexes[i] = (int)floor(spPointGetAxisCoor(arr[i], arr[i]->dim - 1));
	}
	return indexes;
}

int cmpfunc(const void * a, const void * b) {
	if(0 < spPointGetAxisCoor(*(SPPoint**)a, SortCoor) - spPointGetAxisCoor(*(SPPoint**)b, SortCoor)){
		return 1;
	}
	return -1;
}

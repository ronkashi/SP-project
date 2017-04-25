#include "SPKDArray.h"
#include <malloc.h>

////Decelerations:
//#define allocation_error()
//typedef enum SP_WHAT_TO_WHAT_TO_FREE_t {
//	SP_KDARRAY_MAT, SP_COPY_ARR, SP_LEFT_NODE, SP_RIGHT_NODE,
//}SP_WHAT_TO_WHAT_TO_FREE;

struct sp_kdArray_t {
	SPPoint** copy_arr;
	int size;
	int dim;
	int** mat;
};

bool spAllLinesInSameLength(SPPoint** arr, int size) {
	int i, d;
	if (NULL == arr || size < 0) {
		return false; //invalid input
	}
	d = spPointGetDimension(arr[0]);
	for (i = 0; i < size; i++) {
		if (d != spPointGetDimension(arr[i])) {
			return false;
		}
	}
	return true;
}

void spKdArrayDestroy(SPKDArray* kd_array) {
	int i;
	if (NULL != kd_array) {
		if (NULL != kd_array->copy_arr) {
			for (i = 0; i < kd_array->size; i++) {
				spPointDestroy(kd_array->copy_arr[i]);
			}
			free(kd_array->copy_arr);
		}
		if ((NULL != kd_array->mat)) {
			for (i = 0; i < kd_array->dim; i++) {
				free(kd_array->mat[i]);
			}
			free(kd_array->mat);
		}
	}
	free(kd_array);
	return;
}

SPKDArray* Init(SPPoint** arr, int size) {
	int i = 0;
	int d;
	SPKDArray* kd_array = NULL;
	if (!spAllLinesInSameLength(arr, size)) {
		return NULL; //invalid input
	}
	kd_array = (SPKDArray*) calloc(1, sizeof(SPKDArray));
	if (NULL == kd_array) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	d = spPointGetDimension(arr[0]);
	kd_array->size = 0;
	kd_array->dim = 0;

	kd_array->copy_arr = (SPPoint**) calloc(size, sizeof(SPPoint*));
	if (NULL == kd_array->copy_arr) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spKdArrayDestroy(kd_array);
		return NULL;
	}

	for (i = 0; i < size; i++) {
		kd_array->copy_arr[i] = spPointCopy(arr[i]);
		if (!kd_array->copy_arr[i]) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			spKdArrayDestroy(kd_array);
			return NULL;
		}
		kd_array->copy_arr[i] = spPointAddDimention(kd_array->copy_arr[i], i);
		if (!kd_array->copy_arr[i]) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			spKdArrayDestroy(kd_array);
			return NULL;;
		}
		kd_array->size++;
	}

	kd_array->mat = (int**) calloc(d, sizeof(int*));
	if (!kd_array->mat) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spKdArrayDestroy(kd_array);
		return NULL;
	}

	for (i = 0; i < d; i++) {
		kd_array->mat[i] = (int*) spPointSortByCoor(kd_array->copy_arr, size,
				i);
		if (NULL == kd_array->mat[i]) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			spKdArrayDestroy(kd_array);
			return NULL;
		}
		kd_array->dim++;
	}

	for (i = 0; i < kd_array->size; i++) {
		spPointDestroy(kd_array->copy_arr[i]);
		kd_array->copy_arr[i] = spPointCopy(arr[i]);
		if (NULL == kd_array->copy_arr[i]) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			spKdArrayDestroy(kd_array);
			return NULL;
		}
	}

	return kd_array;
}


bool Split(SPKDArray* kdArr, int coor, SPKDArray* kdLeft, SPKDArray* kdRight) {
	if (NULL == kdArr) {
		return false;
	}
	int i;
	int j;
	int right_index;
	int left_index;
	int round_up_med =(kdArr->size % 2 == 0) ? (kdArr->size / 2) : (kdArr->size / 2 + 1);

	kdRight->dim = 0;
	kdRight->size = kdArr->size - round_up_med;
	kdRight->copy_arr = (SPPoint**) calloc(kdRight->size,sizeof(SPPoint*));
	if (!kdRight->copy_arr) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(kdRight);
		return false;
	}

	kdRight->mat = (int**) calloc(kdArr->dim, sizeof(int*));
	if (!kdRight->mat) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spKdArrayDestroy(kdRight);
		return false;
	}

	kdRight->dim = 0;
	for (i = 0; i < kdArr->dim; i++) {
		kdRight->mat[i] = (int*) calloc(kdRight->size, sizeof(int));
		if (NULL == kdRight->mat[i]) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			spKdArrayDestroy(kdRight);
			return false;
		}
		kdRight->dim++;
	}
//for left side
	kdLeft->dim = 0;
	kdLeft->size = round_up_med;
	kdLeft->copy_arr = (SPPoint**) calloc(kdLeft->size,sizeof(SPPoint*));
	if (!kdLeft->copy_arr) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spKdArrayDestroy(kdRight);
		free(kdLeft);
		return false;
	}

	kdLeft->mat = (int**) calloc(kdArr->dim, sizeof(int*));
	if (!kdLeft->mat) {
		spKdArrayDestroy(kdRight);
		spKdArrayDestroy(kdLeft);
		return false;
	}

	kdLeft->dim = 0;
	for (i = 0; i < kdArr->dim; i++) {
		kdLeft->mat[i] = (int*) calloc(kdLeft->size, sizeof(int));
		if (NULL == kdLeft->mat[i]) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			spKdArrayDestroy(kdRight);
			spKdArrayDestroy(kdLeft);
			return false;
		}
		kdLeft->dim++;
	}
//////end of allocations///

	int* reverseMap;
	reverseMap = (int*) calloc(kdArr->size, sizeof(int));
	for (i = 0; i < kdArr->dim; i++) {
		right_index = 0;
		left_index = 0;
		for (j = 0; j < kdArr->size; j++) {
			reverseMap[kdArr->mat[i][j]] = j;
		}
		for (j = 0; j < kdArr->size; j++) {
			if (reverseMap[j] < round_up_med) {
				kdLeft->mat[i][reverseMap[j]] =left_index;
				left_index++;
			} else {
				kdRight->mat[i][right_index] =right_index;
				right_index++;
			}
		}
	}

	for (i = 0; i < kdLeft->size; i++) {
		kdLeft->copy_arr[i] = spPointCopy(kdArr->copy_arr[kdArr->mat[coor][i]]);
		if (NULL == kdLeft->copy_arr[i]) {
			spKdArrayDestroy(kdLeft);
			spKdArrayDestroy(kdRight);

		}
	}

	for (i = 0; i < kdRight->size; i++) {
		kdRight->copy_arr[i] = spPointCopy(
				kdArr->copy_arr[kdArr->mat[coor][i + round_up_med]]);
		if (kdRight->copy_arr[i] == NULL) {
			spKdArrayDestroy(kdLeft);
			spKdArrayDestroy(kdRight);
		}
	}
	return true;
}

int getKdArraySize(SPKDArray* arr) {
	if (NULL == arr) {
		return -1;
	}
	return arr->size;
}

int getKdArrayDim(SPKDArray* arr) {
	if (NULL == arr) {
		return -1;
	}
	return arr->dim;
}
int** getKdArrayMat(SPKDArray* arr) {
	if (NULL == arr) {
		return NULL;
	}
	return arr->mat;
}

SPPoint** getKdArrayCopyArr(SPKDArray* arr) {
	if (NULL == arr) {
		return NULL;
	}
	return arr->copy_arr;
}

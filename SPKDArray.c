#include "SPKDArray.h"
#include <malloc.h>

//Decelerations:
#define allocation_error()
typedef enum SP_WHAT_TO_WHAT_TO_FREE_t {
	SP_KDARRAY_MAT, SP_COPY_ARR, SP_LEFT_NODE, SP_RIGHT_NODE,
}SP_WHAT_TO_WHAT_TO_FREE;

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
		return NULL; //MEM failture
	}
	d = spPointGetDimension(arr[0]);
	kd_array->size = 0;
	kd_array->dim = 0;

	kd_array->copy_arr = (SPPoint**) calloc(size, sizeof(SPPoint*));
	if (NULL == kd_array->copy_arr) {
		spKdArrayDestroy(kd_array);
		return NULL;
	}

	for (i = 0; i < size; i++) {
		kd_array->copy_arr[i] = spPointCopy(arr[i]);
		if (!kd_array->copy_arr[i]) {
			spKdArrayDestroy(kd_array);
			return NULL;
		}
		kd_array->copy_arr[i] = spPointAddDimention(kd_array->copy_arr[i], i);
		if (!kd_array->copy_arr[i]) {
			spKdArrayDestroy(kd_array);
			return NULL;;
		}
		kd_array->size++;
	}

	kd_array->mat = (int**) calloc(d, sizeof(int*));
	if (!kd_array->mat) {
		spKdArrayDestroy(kd_array);
		return NULL;
	}

	for (i = 0; i < d; i++) {
		kd_array->mat[i] = (int*) spPointSortByCoor(kd_array->copy_arr, size,
				i);
		if (NULL == kd_array->mat[i]) {
			//TODO mallocation fail
			spKdArrayDestroy(kd_array);
			return NULL;
		}
		kd_array->dim++;
	}

	for (i = 0; i < kd_array->size; i++) {
		spPointDestroy(kd_array->copy_arr[i]);
		kd_array->copy_arr[i] = spPointCopy(arr[i]);
		if (NULL == kd_array->copy_arr[i]) {
			//TODO mallocation fail
			spKdArrayDestroy(kd_array);
			return NULL;
		}
	}

	return kd_array;
}


//first arg kd_array_left_side
//second arg kd_array_right_side
bool Split(SPKDArray* kdArr, int coor, SPKDArray* kdLeft, SPKDArray* kdRight) {
//TODO
	/*
	 * 1.parse the way of spliting & which coor to split by
	 * 2. in O(n) split to above median and below
	 * 3.build the matrix sorted
	 *
	 */


	if (NULL == kdArr) {
		//invalid arg
		return false; //TODO
	}
	int i;
	int j;
	int right_index;
	int left_index;
	int round_up_med =(kdArr->size % 2 == 0) ? (kdArr->size / 2) : (kdArr->size / 2 + 1);
	//int meddian = kdArr->mat[coor][round_up_med];

//for right side
//	kdRight = (SPKDArray*) calloc(1, sizeof(SPKDArray));
//	if (!kdRight) {
//		//MEM alloc
//		return false; //TODO
//	}
	kdRight->dim = 0;
	kdRight->size = kdArr->size - round_up_med;
	kdRight->copy_arr = (SPPoint**) calloc(kdRight->size,sizeof(SPPoint*));
	if (!kdRight->copy_arr) {
		//MEM alloc
		free(kdRight);
		return false; //TODO
	}

	kdRight->mat = (int**) calloc(kdArr->dim, sizeof(int*));
	if (!kdRight->mat) {
		spKdArrayDestroy(kdRight);
		return false;
	}

	kdRight->dim = 0;
	for (i = 0; i < kdArr->dim; i++) {
		kdRight->mat[i] = (int*) calloc(kdRight->size, sizeof(int));
		if (NULL == kdRight->mat[i]) {
			//TODO mallocation fail
			spKdArrayDestroy(kdRight); //TODO alloc FAil
			return false;
		}
		kdRight->dim++;
	}
//for left side
//	kdLeft = (SPKDArray*) calloc(1, sizeof(SPKDArray));
//	if (!kdLeft) {
//		spKdArrayDestroy(kdRight);
//		//MEM alloc
//		return false; //TODO
//	}
	kdLeft->dim = 0;
	kdLeft->size = round_up_med;
	kdLeft->copy_arr = (SPPoint**) calloc(kdLeft->size,sizeof(SPPoint*));
	if (!kdLeft->copy_arr) {
		//MEM alloc
		spKdArrayDestroy(kdRight);
		free(kdLeft);
		return false; //TODO
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
			//TODO mallocation fail
			spKdArrayDestroy(kdRight);
			spKdArrayDestroy(kdLeft);
			return false; //TODO alloc FAil
		}
		kdLeft->dim++;
	}
//////end of allocations///

	int* reverseMap;
	reverseMap = (int*) calloc(kdArr->size, sizeof(int));
//	for (i = 0; i < kdArr->size; i++) {
//		reverseMap[kdArr->mat[coor][i]] = i;
//	}

//	printf("\n");
//	for (i = 0; i < kdArr->size; i++) {
//		printf("%d   ",reverseMap[i]);
//	}
//	printf("\n");


	for (i = 0; i < kdArr->dim; i++) {
		right_index = 0;
		left_index = 0;///TODO problem
		for (j = 0; j < kdArr->size; j++) {
			reverseMap[kdArr->mat[i][j]] = j;
		}
		for (j = 0; j < kdArr->size; j++) {
			if (reverseMap[j] < round_up_med) {
				kdLeft->mat[i][reverseMap[j]] =left_index; //kdArr->mat[i][j];
				left_index++;
			} else {
				kdRight->mat[i][right_index] =right_index; //kdArr->size - kdArr->mat[i][j];
				right_index++;
			}

//			{
//				kdLeft->mat[i][left_index] = reverseMap[kdArr->mat[i][j]];
//				left_index++;
//			} else {
//				kdRight->mat[i][right_index] = reverseMap[kdArr->mat[i][j]]
//						- round_up_med;
//
//				right_index++;
//			}
		}
	}

//	if (kdRight->size != right_index){
//		printf("kdRight->size : %d\n",kdRight->size);
//		printf("right_index : %d\n",right_index);
//		printf("not OK!!!\n");
//		fflush(NULL);
//	}
//
//	if (kdLeft->size != left_index){
//		printf("kdLeft->size : %d\n",kdLeft->size);
//		printf("left_index : %d\n",left_index);
//		printf("not OK!!!\n");
//		fflush(NULL);
//	}


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

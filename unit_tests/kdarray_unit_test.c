#include <stdio.h>
#include <malloc.h>
#include "../SPPoint.h"
#include "SPKDArray.h"
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..


#define POINT_NUM 5
#define DIM 2

int printKDArray(SPKDArray* arr) {
	int j;
	for (j = 0; j < getKdArraySize(arr); ++j) {
		printf("%c\t", spPointGetIndex(getKdArrayCopyArr(arr)[j]));
	}
	printf("\n");
	for (int i = 0; i < spPointGetDimension(getKdArrayCopyArr(arr)[0]); i++) {
		for (int j = 0; j < getKdArraySize(arr); ++j) {
			printf("%d\t", getKdArrayMat(arr)[i][j]);
		}
		printf("\n");
	}
	return 1;
}
static bool basicInitKdTest() {
	double data[POINT_NUM][DIM] = { { 1, 2 }, { 123, 70 }, { 2, 7 }, { 9, 11 },
			{ 3, 4 } };

	double expected[15] = {97,0,0,98,2,4,99,4,2,100,3,3,101,1,1};
	int i;
	int j;
	SPPoint** p_arr = (SPPoint**) malloc(POINT_NUM * sizeof(*p_arr));
	for (i = 0; i < POINT_NUM; ++i) {
		p_arr[i] = spPointCreate(data[i], DIM, 97 + i);
	}
	SPKDArray* kdarr = Init(p_arr, POINT_NUM);
	printKDArray(kdarr);

	for (i = 0; i < POINT_NUM; ++i) {
		for(j=0;j<DIM +1 ; i++){
			ASSERT_TRUE( getKdArrayMat(kdarr)[i][j] = expected[i*(DIM+1)+j]);
		}
		spPointDestroy(p_arr[i]);
	}
	for (i = 0; i < POINT_NUM; ++i) {
		spPointDestroy(p_arr[i]);
	}
	spKdArrayDestroy(kdarr);
	free(p_arr);
	return true;
}

static bool basicSplitKdTest() {
	double data[POINT_NUM][DIM] = { { 1, 2 }, { 123, 70 }, { 2, 7 }, { 9, 11 },
			{ 3, 4 } };

	SPPoint** p_arr = (SPPoint**) malloc(POINT_NUM * sizeof(*p_arr));
	for (int i = 0; i < POINT_NUM; ++i) {
		p_arr[i] = spPointCreate(data[i], DIM, 97 + i);
	}
	SPKDArray* kdarr = Init(p_arr, POINT_NUM);
	printKDArray(kdarr);
	SPKDArray* left = NULL;
	SPKDArray* right = NULL;
	left = (SPKDArray*) calloc(1, sizeof(left));
	right = (SPKDArray*) calloc(1, sizeof(right));
	Split(kdarr, 0, left, right);
	printf("\n");
	printKDArray(left);
	printf("\n");
	printKDArray(right);

	spKdArrayDestroy(right);
	spKdArrayDestroy(left);
	for (i = 0; i < POINT_NUM; ++i) {
		spPointDestroy(p_arr[i]);
	}
	free(p_arr);
	return true;
}

int main(int argc, char const *argv[]) {
	RUN_TEST(basicInitKdTest);
	RUN_TEST(basicSplitKdTest);
	printf("Completed!\n");
	return 0;
}

//int main(int argc, char **argv) {
//	int i;
//	int* indexes;
//	SPPoint** p_arr;
//	int index = 100;
//	double data[POINT_NUM][DIM] = { { 1, 4, 0 }, { 3, 2, 1 } };
//
//	p_arr = malloc(sizeof(SPPoint*) * 2);
//
//	for (i = 0; i < POINT_NUM; i++)
//		p_arr[i] = spPointCreate(data[i], DIM, index + i);
//
//	indexes = spPointSortByCoor(p_arr, POINT_NUM, 1);
//
//	for (i = 0; i < POINT_NUM; i++)
//		printf("item no. %d is: %d\n", i, indexes[i]);
//
//	for (i = 0; i < POINT_NUM; ++i) {
//		printf("point no. %d dim is: %d\n", i, spPointGetDimension(p_arr[i]));
//		spPointDestroy(p_arr[i]);
//	}
//	free(p_arr);
//	free(indexes);
//
//	return 0;
//
//}

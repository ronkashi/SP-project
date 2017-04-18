//#include <stdio.h>
//#include <malloc.h>
//#include "SPPoint.h"
//#include "SPKDArray.h"
//
//#define POINT_NUM 5
//#define DIM 2
//int printKDArray(SPKDArray* arr) {
//
//	int j;
//	for(j = 0; j <getKdArraySize(arr); ++j) {
//		printf("%c\t", spPointGetIndex(getKdArrayCopyArr(arr)[j]));
//	}
//
//	printf("\n");
//	for (int i = 0; i < spPointGetDimension(getKdArrayCopyArr(arr)[0]); i++) {
//		for (int j = 0; getKdArraySize(arr); ++j) {
//			printf("%d\t", getKdArrayMat(arr)[i][j]);
//		}
//		printf("\n");
//	}
//
//	return 1;
//}
//
//int main(int argc, char const *argv[]) {
//
//	double data[POINT_NUM][DIM] = { { 1, 2 }, { 123, 70 }, { 2, 7 }, { 9, 11 },
//			{ 3, 4 } };
//
//	SPPoint** p_arr = (SPPoint**) malloc(POINT_NUM * sizeof(*p_arr));
//	for (int i = 0; i < POINT_NUM; ++i) {
//		p_arr[i] = spPointCreate(data[i], DIM, 97 + i);
//	}
//	SPKDArray* kdarr = Init(p_arr, POINT_NUM);
//	printKDArray(kdarr);
//	SPKDArray* left = NULL;
//	SPKDArray* right = NULL;
//	Split(kdarr, 0,left,right);
//	printf("\n");
//	printKDArray(left);
//	printf("\n");
//	printKDArray(right);
//
//	printf("hello\n");
//	return 0;
//}
//

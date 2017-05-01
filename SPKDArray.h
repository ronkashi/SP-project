
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include <stdbool.h>
#include <stdio.h>


typedef struct sp_kdArray_t SPKDArray;

/*
 * Initializes the kd-array with the data given by arr.
 * The complexity of this operation is d*n*log(n).
 */
SPKDArray* Init(SPPoint** arr, int size);

/*
 * Returns two kd-arrays (kdLeft, kdRight) such that the first
 * n//2+1 points with respect to the coordinate coor are in kdLeft , and the rest of the points are in
 * kdRight.
 * The complexity of this operation is d*n.
 *
 * 1.parse the way of spliting & which coor to split by
 * 2. in O(n) split to above median and below
 * 3.build the matrix sorted
 *
 * 3 arg kd_array_left_side
 * 4 arg kd_array_right_side
 */
bool Split(SPKDArray* kdArr, int coor,SPKDArray* kdLeft, SPKDArray* kdRight);

//destroy func for kd array
void spKdArrayDestroy(SPKDArray* kd_array);

/*
 * getters
 */
int getKdArraySize(SPKDArray* arr);
int getKdArrayDim(SPKDArray* arr);
int** getKdArrayMat(SPKDArray* arr);
SPPoint** getKdArrayCopyArr(SPKDArray* arr);

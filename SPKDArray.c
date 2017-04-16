#include "SPKDArray.h"
#include <malloc.h>

//Decelerations:
#define spPCADimention 128 //TODO take from config file
struct sp_kdArray_t {
	SPPoint* copy_arr;
	int size;
	int dim;
	int** mat;
};

//typedef struct tuple_t{
//	int index
//	double
//};

struct sp_kdTreeNode_t {
	int Dim; //The splitting dimension
	int Val; //The median value of the splitting dimension
	KDTreeNode* Left; //Left = Pointer to the left subtree
	KDTreeNode* Right; //Pointer to the right subtree
	SPPoint* Data; //Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
};

typedef enum dir {
	Left, Right
} direction;

KDTreeNode Init(SPPoint** arr, int size) {
	int i = 0;
	SPPoint** copy_arr = NULL;
	SPKDArray kd_array;
	copy_arr = (SPPoint**) calloc(size, sizeof(*copy_arr));
	if (copy_arr == NULL) {
		return NULL; //TODO
	}
	for (i = 0; i < size; i++) {
		copy_arr = spPointCopy(arr[i]);
		if (!copy_arr) {
			return NULL; //TODO
		}
		copy_arr[i] = spPointAddDimention(copy_arr[i], i);
		if (!copy_arr) {
			return NULL; //TODO;
		}
	}

	//TODO sort by every coor and insert to SPKDArray.mat
	kd_array.mat = calloc(spPCADimention,sizeof(*(kd_array.mat)));
	for (i=0;i<spPCADimention;i++){
		kd_array.mat[i]= (int*) spPointSortByCoor(copy_arr,size,i);
	}

//		int values_in_the_sorted_cor[2]={0};
//
//	values_in_the_sorted_cor[1] = (SPPoint**)calloc(size,sizeof(*(values_in_the_sorted_cor[1])));
//
//	int** A = (int**) calloc(spPCADimention,sizeof(*A));
//	for(i=0;i<size;i++){
//		A[i] = (int*) calloc(spPCADimention,sizeof(*(A[i])));
//	}
//	//TODO MEM allocation
//
//	for (i=0;i<size;i++){
//		values_in_the_sorted_cor[0][i]=i;
//		values_in_the_sorted_cor[1][i]=spPointGetAxisCoor(arr[i],i);
//	}
//	for (i=0;i<size;i++){
//		A[i]=qsort(values_in_the_sorted_cor,size,sizeof(*values_in_the_sorted_cor),cmpfunc);
//	}

return NULL;
}

bool Split(SPKDArray kdArr, int coor, SPKDArray kdLeft, SPKDArray kdRight) {
	//TODO
	/*
	 * 1.parse the way of spliting & which coor to split by
	 * 2. in O(n) split to above median and below
	 * 3.build the matrix sorted
	 *
	 */
return false;
}

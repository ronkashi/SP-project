#include <stdio.h>
#include <malloc.h>
#include "../SPPoint.h"
//#include "../SPKDArray.h"
#include "../SPKDTree.h"
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..


#define POINT_NUM 5
#define DIM 2

int printKDArray(SPKDArray* arr) {
	int i,j;
	for (j = 0; j < getKdArraySize(arr); ++j) {
		printf("%c\t", spPointGetIndex(getKdArrayCopyArr(arr)[j]));
	}
	printf("\n");
	for (i = 0; i < spPointGetDimension(getKdArrayCopyArr(arr)[0]); i++) {
		for (j = 0; j < getKdArraySize(arr); ++j) {
			printf("%d\t", getKdArrayMat(arr)[i][j]);
		}
		printf("\n");
	}
	return 1;
}
static bool basicInitKdTest() {
	double data[POINT_NUM][DIM] = { { 1, 2 }, { 123, 70 }, { 2, 7 }, { 9, 11 },
			{ 3, 4 } };

	double expected[10] = {0,2,4,3,1,0,4,2,3,1};
	int i;
	int j;
	SPPoint** p_arr = (SPPoint**) malloc(POINT_NUM * sizeof(*p_arr));
	for (i = 0; i < POINT_NUM; ++i) {
		p_arr[i] = spPointCreate(data[i], DIM, 97 + i);
	}
	SPKDArray* kdarr = Init(p_arr, POINT_NUM);
	printKDArray(kdarr);
	for (i = 0; i < DIM; i++) {
		for(j=0;j<POINT_NUM ; j++){
			ASSERT_TRUE( getKdArrayMat(kdarr)[i][j] == expected[i*(POINT_NUM)+j]);
		}
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

	int i;
	SPPoint** p_arr = (SPPoint**) malloc(POINT_NUM * sizeof(*p_arr));
	for (i = 0; i < POINT_NUM; ++i) {
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

int printSPPoint (SPPoint* p)
{
	printf("******************** Start printf kdarr ********************\n");
	int i = 0;
	if( p == NULL)
		return -1;
	printf("point Dim: %d\n",spPointGetDimension(p));
	printf("point index :%d\n", spPointGetIndex(p));
	for (i = 1; i < spPointGetDimension(p) + 1; i++)
		printf("coordinate no: %d value is: %f\n", i, spPointGetAxisCoor(p, i));

	printf("******************** End printf kdarr ********************\n");

	return 0;
}

void printKDTree( kdTreeNode* root, int depth)
{
	// // to get side
	// if (side == 0){
	// printf("********************** Start Node at side %s depth %d **********************", "left", depth);
	// printf("********************** Start Node at side %s depth %d **********************", "right", depth);
	// }
	// to check if leaf or not
	if(getKdTreeNodeData(root) == NULL) /* not leaf */{
		printf("dim:%d\n", getKdTreeNodeDim(root));
		printf("val:%f\n", getKdTreeNodeVal(root));
		depth++;
		printKDTree(getKdTreeNodeLeftChild(root),depth);
		printKDTree(getKdTreeNodeRightChild(root),depth);
	}
	else
		printSPPoint(getKdTreeNodeData(root));

	// // to get side
	// if (side == 0){
	// printf("********************** End Node at side %s depth %d **********************", "left", depth);
	// printf("********************** End Node at side %s depth %d **********************", "right", depth);
	// }
}

static bool KDTreeSpeardTest()
{
	int i;
	double data[POINT_NUM][DIM] = { { 1, 2 }, { 123, 70 }, { 2, 7 }, { 9, 11 },
			{ 3, 4 } };
	/*********************** Created KDArray ***********************/
	SPPoint** p_arr = (SPPoint**)malloc(POINT_NUM*sizeof(SPPoint**));
	for ( i = 0; i < POINT_NUM; ++i){
		p_arr[i] = spPointCreate(data[i],DIM,97+i);
	};
	SPKDArray* kdarr = Init(p_arr,POINT_NUM);
	/***************************************************************/
	kdTreeNode* root = NULL;
	spKdTreeInit(kdarr, root, MAX_SPREAD, 0);

	printKDTree(root,0);
	spKdTreeDestroy(root);
	return true;
}

int main() {
	RUN_TEST(basicInitKdTest);
	RUN_TEST(basicSplitKdTest);
	RUN_TEST(KDTreeSpeardTest);
	printf("Completed!\n");
	return 0;
}
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "unit_test_util.h"
#include "../KDTree.h"

#define INIT_ERROR "KDTreeInit"
#define POINT_NUM 2 
#define DIM 2

int main(int argc, char const *argv[])
{
	

	return 0;
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

void printKDTree( KDTreeNode* root, int depth, int side)
{
	// to get side
	if (side == 0)
	printf("********************** Start Node at side %s depth %d **********************", "left", depth)
	printf("********************** Start Node at side %s depth %d **********************", "right", depth)
	
	// to check if leaf or not
	if(getKDTreeData(root) == NULL) /* not leaf */{
		printf("dim:%d\n", getKdTreeNodeDim(root));
		printf("val:%d\n", getKdTreeNodeVal(root));
		printKDTree(getKdTreeNodeLeftChild(root));
		printKDTree(getKdTreeNodeRightChild(root));
	}
	else
		printSPPoint(getKDTreeData(root));

	// to get side
	if (side == 0)
	printf("********************** End Node at side %s depth %d **********************", "left", depth)
	printf("********************** End Node at side %s depth %d **********************", "right", depth)
	
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
	KDArray* kdarr = Init(p_arr,POINT_NUM);
	/***************************************************************/
	KDTreeNode* root;
	KDTreeInit(kdarr, root, MAX_SPREAD, 0)

	printKDTree(root);
	KDTreeNodeDestroy(root);

}

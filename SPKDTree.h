/*
 * spKdTreeNode.h
 *
 */
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPPoint.h"
#include <malloc.h>
#include <stdio.h>
#include <time.h>

typedef struct kdTreeNode_t kdTreeNode;

typedef enum splitting_criteria_t{
	MAX_SPREAD, // Define the spread of the i_th dimension to be the difference
				//between the maximum and minimum
				//of the i_th coordinate of all points.
				//In the example given in the previous section the spread of
				//the x-coordinate is 123 - 1 = 122 and for the y-coordinate is 70-2 =68.
				//Split the kd-array according the dimension with the highest spread
				//(if there are several candidates choose the lowest dimension)
	RANDOM, // choose a random dimension
	INCREMENTAL //- if the splitting dimension of the upper level was i,
				//then the splitting dimension at this level is (i+1)%d
} SPLIT_CRITERIA;

int getCoorToSplitBy(SPKDArray* arr,SPLIT_CRITERIA op, int level);

int getMaxCoorOfSpread(SPKDArray* arr);

int spKdTreeInit(SPKDArray* arr,kdTreeNode* root,SPLIT_CRITERIA op,int level);

int kNearestNeighbors(kdTreeNode* curr ,SPBPQueue* bpq, SPPoint* P);

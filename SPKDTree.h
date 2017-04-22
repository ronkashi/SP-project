/*
 * spKdTreeNode.h
 *
 */
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct kdTreeNode_t {
	int Dim; //The splitting dimension
	double Val; //The median value of the splitting dimension
	kdTreeNode* Left; //Left = Pointer to the left subtree
	kdTreeNode* Right; //Pointer to the right subtree
	SPPoint* Data; //Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
} kdTreeNode;

int getCoorToSplitBy(SPKDArray* arr,SPLIT_CRITERIA op, int level);

int getMaxCoorOfSpread(SPKDArray* arr);

int spKdTreeInit(SPKDArray* arr,kdTreeNode* root,SPLIT_CRITERIA op,int level);

int kNearestNeighbors(kdTreeNode* curr ,SPBPQueue* bpq, SPPoint* P);

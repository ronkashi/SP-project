/*
 * spKdTreeNode.h
 *
 */
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPLogger.h"

#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct kdTreeNode_t {
	int Dim; //The splitting dimension
	double Val; //The median value of the splitting dimension
	struct kdTreeNode_t* Left; //Left = Pointer to the left subtree
	struct kdTreeNode_t* Right; //Pointer to the right subtree
	SPPoint* Data; //Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
} kdTreeNode;

int getCoorToSplitBy(SPKDArray* arr,SPLIT_CRITERIA op, int level);

int getMaxCoorOfSpread(SPKDArray* arr);

int spKdTreeInit(SPKDArray* arr,kdTreeNode* root,SPLIT_CRITERIA op,int level);

int kNearestNeighbors(kdTreeNode* curr ,SPBPQueue* bpq, SPPoint* P);

int spKdTreeDestroy(kdTreeNode* root);

////getters//////

int getKdTreeNodeDim(kdTreeNode* root);

double getKdTreeNodeVal(kdTreeNode* root);

SPPoint* getKdTreeNodeData(kdTreeNode* root);

kdTreeNode* getKdTreeNodeLeftChild(kdTreeNode* root);

kdTreeNode* getKdTreeNodeRightChild(kdTreeNode* root);
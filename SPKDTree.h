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

typedef struct kdTreeNode_t kdTreeNode;

int getCoorToSplitBy(SPKDArray* arr,SPLIT_CRITERIA op, int level);

int getMaxCoorOfSpread(SPKDArray* arr);

int spKdTreeInit(SPKDArray* arr,kdTreeNode* root,SPLIT_CRITERIA op,int level);

int kNearestNeighbors(kdTreeNode* curr ,SPBPQueue* bpq, SPPoint* P);

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

/*
*return by which coor to split by
* arr - the array we should split
* op - the splitting method
* level - if the op is RANDOM or MAX_SPREAD its dont care
*       - for INCREMENTAL at the first level should be 0
*/
int getCoorToSplitBy(SPKDArray* arr,SPLIT_CRITERIA op, int level);

/*
*returns the coor wuth the max spread at arr
*/
int getMaxCoorOfSpread(SPKDArray* arr);

/*
*init the tree according to SPLIT_CRITERIA
*/
int spKdTreeInit(SPKDArray* arr,kdTreeNode* root,SPLIT_CRITERIA op,int level);

/*
*build a BPQ with the K-nearest neighboors of a point P
*/
int kNearestNeighbors(kdTreeNode* curr ,SPBPQueue* bpq, SPPoint* P);

//destroy kd tree
int spKdTreeDestroy(kdTreeNode* root);

////getters//////
/*
*get the dimmention that this node is split by
*/
int getKdTreeNodeDim(kdTreeNode* root);

/*
*get the median value of the splitting dimension
*/
double getKdTreeNodeVal(kdTreeNode* root);

/*
* get pointer to a point if the current node is a leaf
* otherwise this field value is NULL
*/
SPPoint* getKdTreeNodeData(kdTreeNode* root);

/*
* get pointer to the left subtree
* if the left subtree doesnt excists return NULL
*/
kdTreeNode* getKdTreeNodeLeftChild(kdTreeNode* root);

/*
* get pointer to the right subtree
* if the right subtree doesnt excists return NULL
*/
kdTreeNode* getKdTreeNodeRightChild(kdTreeNode* root);
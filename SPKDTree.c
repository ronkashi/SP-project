#include "SPKDTree.h"

#define spKNN 3 //TODO ask from config

int getCoorToSplitBy(SPKDArray* arr, SPLIT_CRITERIA op, int level) {
	switch (op) {
	case MAX_SPREAD:
		return getMaxCoorOfSpread(arr);
		break;
	case RANDOM:
		srand(time(NULL));
		return rand() % getKdArrayDim(arr);
		break;
	case INCREMENTAL:
		return level;
		break;
	default:
		return -1;
		break;
	}
	return -1;
}

int getMaxCoorOfSpread(SPKDArray* arr) {
	int** mat = getKdArrayMat(arr);
	SPPoint** copy_arr = getKdArrayCopyArr(arr);
	int size = getKdArraySize(arr);
	int d = getKdArrayDim(arr);
	int max_spread = 0;
	int max_spread_index = -1;
	int i = 0;
	// MAX_SPREAD:
	for (i = 0; i < d; i++) {
		if (max_spread
				< spPointGetAxisCoor(copy_arr[mat[i][size - 1]], i)
						- spPointGetAxisCoor(copy_arr[mat[i][0]], i)) {
			max_spread_index = i;
			max_spread = spPointGetAxisCoor(copy_arr[mat[i][size - 1]], i)
					- spPointGetAxisCoor(copy_arr[mat[i][0]], i);
		}
	}
	return max_spread_index;
}

int spKdTreeInit(SPKDArray* arr, kdTreeNode* root, SPLIT_CRITERIA op, int level) {
	if (NULL == arr) {
		return -1;
	}

	if (getKdArraySize(arr) == 1) {
		root->Data = getKdArrayCopyArr(arr)[0];
		root->Dim = -1;
		root->Left = NULL;
		root->Right = NULL;
		root->Val = -1;
		return 0;
	}

	int round_up_med =
			(getKdArraySize(arr) % 2 == 0) ?
					(getKdArraySize(arr) / 2) : (getKdArraySize(arr) / 2 + 1);

	SPKDArray* kdLeft = NULL;
	SPKDArray* kdRight = NULL;
	root->Data = NULL;
	root->Dim = getCoorToSplitBy(arr, op, level);
	root->Val = spPointGetAxisCoor(
			getKdArrayCopyArr(arr)[getKdArrayMat(arr)[root->Dim][round_up_med]],
			root->Dim);
	kdLeft = (SPKDArray*) calloc(1, sizeof(kdLeft));
	if(NULL == kdLeft){
		return -1;//TODO mem alloc fail
	}
	kdRight = (SPKDArray*) calloc(1, sizeof(kdRight));
	if(NULL == kdRight){
		free(kdLeft);
		return -1;//TODO mem alloc fail
	}
	if (false == Split(arr, root->Dim, kdLeft, kdRight)) {
		return -1;
	}
	level++;
	root->Left = (kdTreeNode*) malloc(sizeof(kdTreeNode));
	if (NULL == root->Left) {
		//TODO allocation fail
		printf("tree malloc fail\n");
		return -1;
	}
	root->Right = (kdTreeNode*) malloc(sizeof(kdTreeNode));
	if (NULL == root->Right) {
		//TODO allocation fail
		free(root->Left);
		printf("tree malloc fail\n");
		return -1;
	}
	spKdTreeInit(kdLeft, root->Left, op, level);
	spKdTreeInit(kdRight, root->Right, op, level);
	return 0;
}
bool isLeaf(kdTreeNode* node) {
	if (!node) {
		return false;
	}
	if (NULL != node->Data) {
		return true;
	}
	return false;
}
int kNearestNeighborsRecFunc(kdTreeNode* curr, SPBPQueue* bpq, SPPoint* P) {
	if (isLeaf(curr)) {
		if (spBPQueueMaxValue(bpq) > spPointL2SquaredDistance(curr->Data, P)) {
			spBPQueueEnqueue(bpq, spPointGetIndex(curr->Data),
					spPointL2SquaredDistance(curr->Data, P));
			return 0;
		}
	}
	if (spPointGetAxisCoor(P, curr->Dim) <= curr->Val) {
		kNearestNeighborsRecFunc(curr->Left, bpq, P);
		if ((!spBPQueueIsFull(bpq))
				|| (spBPQueueMaxValue(bpq)
						> abs(curr->Val - spPointGetAxisCoor(P, curr->Dim)))) {
			kNearestNeighborsRecFunc(curr->Right, bpq, P);
		}
	} else {
		kNearestNeighborsRecFunc(curr->Right, bpq, P);
		if ((!spBPQueueIsFull(bpq))
				|| (spBPQueueMaxValue(bpq)
						> abs(curr->Val - spPointGetAxisCoor(P, curr->Dim)))) {
			kNearestNeighborsRecFunc(curr->Left, bpq, P);
		}
	}
	return 0;

}

int kNearestNeighbors(kdTreeNode* curr, SPBPQueue* bpq, SPPoint* P) {
	if (NULL == curr || NULL == P) {
		return -1;
	}
	bpq = spBPQueueCreate(spKNN);
	kNearestNeighborsRecFunc(curr, bpq, P);

	return 0;
}


#include "SPPoint.h"
#include <stdbool.h>


typedef struct sp_kdArray_t SPKDArray;
typedef struct sp_kdTreeNode_t KDTreeNode;


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
 */
bool Split(SPKDArray kdArr, int coor,SPKDArray kdLeft, SPKDArray kdRight);

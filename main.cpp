

#include "main_aux.h"


#define ALLOC_ERROR_MSG "Allocation error"

int main(int argc, char* argv[]) {
    SPConfig config;
    SP_CONFIG_MSG msg;
    SPPoint** flatDatabase;
    SPPoint** queryFeatures;
    SPKDArray* kdArray;
    kdTreeNode* kdTree = NULL;
    SPBPQueue* queue = NULL, *imgQueue = NULL;
    BPQueueElement* queueElement;
    int *featureHits, allFeatures=0, numQueryFeatures = 1, kClosest, numOfImgs;
    char path[1024];

    config = createConfig(argc, argv);
    if(!config) return -1;

    if(!initializeLogger(config)) return -1;

    sp::ImageProc ip = sp::ImageProc(config);

    flatDatabase = (SPPoint**) malloc(allFeatures * sizeof(SPPoint*));
    if(!flatDatabase) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        spConfigDestroy(config);
        spLoggerDestroy();
		return -1;
    }
    allFeatures = processFeatures(config, flatDatabase, ip);
    if(allFeatures <= 0) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        spConfigDestroy(config);
        spLoggerDestroy();
        free(flatDatabase);
		return -1;
    }

    //initialize data structures

    kdArray = Init(flatDatabase, allFeatures);
    for(int i = 0; i<allFeatures; i++) {
        spPointDestroy(flatDatabase[i]);
    }
    free(flatDatabase);
    printf("Array created\n");
    kdTree = (kdTreeNode*) malloc(sizeof(*kdTree));

    if(spKdTreeInit(kdArray, kdTree, spConfigGetKDTreeSplitMethod(config, &msg), 0) < 0) {
        //TODO free mem and exit if initializing tree fails
    }
    printf("Array size: %d\n", getKdArraySize(kdArray));
    spKdArrayDestroy(kdArray);

    queueElement = (BPQueueElement*) malloc(sizeof(BPQueueElement));
    queue = spBPQueueCreate(spConfigGetKNN(config, &msg));

    numOfImgs = spConfigGetNumOfImages(config, &msg);

    while(spEnterQueryImg(path)) { //TODO implement bool getImageFromPath(char* path, ...)
        queryFeatures = ip.getImageFeatures(path, numOfImgs, &numQueryFeatures);
        featureHits = (int*) calloc(numOfImgs, sizeof(int));
        for(int i = 0; i<numQueryFeatures; i++) {
            kNearestNeighbors(kdTree, queue, queryFeatures[i]);
            for(int j=0; j<spBPQueueSize(queue); j++) {
                spBPQueuePeek(queue, queueElement);
                featureHits[queueElement->index]++;
                spBPQueueDequeue(queue);
            }
        }
        kClosest = spConfigGetNumOfSimilarImgs(config, &msg);
        imgQueue = spBPQueueCreate(kClosest);
        if(NULL == queue){
            // TODO
        }
        for (int i = 0; i < numOfImgs; i++) {
            spBPQueueEnqueue(imgQueue, i, (double) ((kClosest * numQueryFeatures) - featureHits[i]));
        }
        free(featureHits);

        if(spConfigMinimalGui(config, &msg)) {
            for (int j = 0; j < kClosest; j++) {
                spBPQueuePeek(imgQueue, queueElement);
                spConfigGetImagePath(path, config, queueElement->index);
                ip.showImage(path);
                spBPQueueDequeue(imgQueue);
            }
        } else {
            printf("Best candidates for - %s - are:\n", path);
            for (int j = 0; j < kClosest; j++) {
                spBPQueuePeek(imgQueue, queueElement);
                spConfigGetImagePath(path, config, queueElement->index);
                printf("%s\n", path);
                spBPQueueDequeue(imgQueue);
            }
        }
        spBPQueueDestroy(imgQueue);
        for(int i = 0; i<numQueryFeatures; i++) {
            spPointDestroy(queryFeatures[i]);
        }
        free(queryFeatures);
    }

    // TODO freeAllResourcesAndExit()
    // TODO kdTreeDestroy();
    spBPQueueDestroy(queue);
    free(queueElement);
    spConfigDestroy(config);
    printf("Exiting...\n");
    return 0;
}

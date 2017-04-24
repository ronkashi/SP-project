#include <cstdio>
#include <cstring>

extern "C"{
    #include "SPConfig.h"
    #include "SPLogger.h"
    #include "SPPoint.h"
    #include "SPKDArray.h"
    #include "SPKDTree.h"
}

#include "SPImageProc.h"
#include "main_aux.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    SPConfig config;
    SP_CONFIG_MSG msg;
    SPPoint*** featuresDatabase;
    SPPoint** flatDatabase;
    SPPoint** queryFeatures;
    SPKDArray* kdArray;
    kdTreeNode* kdTree = NULL;
    SPBPQueue* queue = NULL, *imgQueue = NULL;
    BPQueueElement* queueElement;
    int *nFeatures, *featureHits;
    double* data;
    int numOfImgs, dimension, fd, allFeatures=0, numQueryFeatures = 1, kClosest;
    char path[1024];

    config = createConfig(argc, argv, &msg);

    dimension = spConfigGetPCADim(config, &msg);
    numOfImgs = spConfigGetNumOfImages(config, &msg);
    nFeatures = (int*) malloc(numOfImgs * sizeof(*nFeatures));
    featuresDatabase = (SPPoint***) malloc(numOfImgs * sizeof(*featuresDatabase));

    sp::ImageProc ip = sp::ImageProc(config);

    if (spConfigIsExtractionMode(config, &msg)) {
        // Extract features from images
        for(int i = 0; i < numOfImgs; i++) {
            msg = spConfigGetImagePath(path, config, i);
            featuresDatabase[i] = ip.getImageFeatures(path, i, nFeatures+i);

            //save to file
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR | O_CREAT, 0755);
            write(fd, nFeatures+i, sizeof(int));
            for(int j = 0; j<nFeatures[i]; j++) {
                write(fd, spPointGetData(featuresDatabase[i][j]), dimension * sizeof(double));
            }
            close(fd);
        }
    } else {

        // Extract features from file
        data = (double*) malloc(dimension * sizeof(double));
        for(int i = 0; i<numOfImgs; i++) {
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR);
            read(fd, nFeatures+i, sizeof(int));
            featuresDatabase[i] = (SPPoint**) malloc(nFeatures[i] * sizeof(SPPoint*));
                        
            for(int j = 0; j<nFeatures[i]; j++) {
                read(fd, data, dimension * sizeof(double));
                featuresDatabase[i][j] = spPointCreate(data, dimension, i);
            }
            close(fd);
        }
        free(data);
        printf("Extracted from file\n");
    }

    // flattening point array so a kd tree can be created
    for(int i=0; i<numOfImgs; i++) {
        allFeatures += nFeatures[i];
    }
    flatDatabase = (SPPoint**) malloc(allFeatures * sizeof(SPPoint*));
    int k = 0;
    for(int i=0; i<numOfImgs; i++) {
        for(int j=0; j<nFeatures[i]; j++) {
            flatDatabase[k++] = featuresDatabase[i][j];
        }
        free(featuresDatabase[i]);
    }
    free(nFeatures);
    free(featuresDatabase);

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

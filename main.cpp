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
    SPPoint** flatDatabase, queryFeatures;
    SPKDArray* kdArray;
    kdTreeNode* kdTree = NULL;
    SPBPQueue* queue = NULL;
    BPQueueElement* queueElement;
    int* nFeatures, featureHits;
    double* data;
    int numOfImgs, dimension, fd, allFeatures=0, numQueryFeatures = 0, kClosest;
    char path[1024];

    if(argc == 1) {

        config = spConfigCreate("spcbir.config", &msg);
        if(msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf("The default configuration file spcbir.config couldn't be open\n");
            return -1;
        }
    } else if(argc == 3 && (strcmp(argv[1], "-c") == 0)) {

        config = spConfigCreate(argv[2], &msg);
        if(msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf("The configuration file %s couldn't be open\n", argv[2]);
            return -1;
        }
    } else {
        printf("Invalid command line: use -c <config_filename>\n");
        return -1;
    }

    if(msg != SP_CONFIG_SUCCESS) {
        printf("some other message"); //TODO
        return -1;
    }

    printf("config created\n");

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

    if(spKdTreeInit(kdArray, kdTree, spConfigGetKDTreeSplitMethod(config, &msg), 0) < 0) {
        //TODO free mem and exit if initializing tree fails
    }
    printf("Array size: %d\n", getKdArraySize(kdArray));
    spKdArrayDestroy(kdArray);

    queueElement = (BPQueueElement*) malloc(sizeof(BPQueueElement));

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
            spBPQueueDestroy(queue);
        }
        kClosest = spConfigGetNumOfSimilarImgs(config, &msg);
        queue = spBPQueueCreate(kClosest);
        if(NULL == queue){
            // TODO
        }
        for (int i = 0; i < numOfImgs; i++) {
            spBPQueueEnqueue(queue, i, (double) ((kClosest * numQeuryFeatures) - featureHits[i]));
        }
        free(featureHits);

        if(spConfigMinimalGui(config, &msg)) {
            for (int j = 0; j < kClosest; j++) {
                spBPQueuePeek(queue, queueElement);
                spConfigGetImagePath(path, config, queueElement->index);
                ip.showImage(path);
                spBPQueueDequeue(localQueue);
            }
        } else {
            printf("Best candidates for - %s - are:\n", path);
            for (int j = 0; j < kClosest; j++) {
                spBPQueuePeek(queue, queueElement);
                spConfigGetImagePath(path, config, queueElement->index);
                printf("%s\n", path);
                spBPQueueDequeue(localQueue);
            }
        }
        spBPQueueDestroy(queue);
        for(int i = 0; i<numQueryFeatures; i++) {
            spPointDestroy(queryFeatures[i]);
        }
        free(queryFeatures);
    }

    // TODO freeAllResourcesAndExit()
    // TODO kdTreeDestroy();
    free(queueElement);
    spConfigDestroy(config);
    printf("Exiting...\n");
    return 0;
}

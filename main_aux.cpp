#include "main_aux.h"

#define DEFAULT_CONFIG "spcbir.config"
#define DEFAULT_CONFIG_ERROR "The default configuration file spcbir.config couldn't be open\n"
#define CONFIG_ERROR "The configuration file %s couldn't be open\n"
#define INVALID_COMMAND "Invalid command line: use -c <config_filename>\n"
#define MAX_LENGTH 1024
#define ALLOC_ERROR_MSG "Allocation error"
#define LOG_FILE_ERROR "Cannot open logger file\n"
#define WRITE_TO_FILE_ERROR "features could not be saved to file"
#define OPEN_FEATS_ERROR "Features file could not be opened"
#define READ_FEATS_ERROR "Unable to read features from file"

SPConfig createConfig(int argc, char* argv[]) {
    SP_CONFIG_MSG msg;
	SPConfig config;
	if(argc == 1) {
        config = spConfigCreate(DEFAULT_CONFIG, &msg);
        if(msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf(DEFAULT_CONFIG_ERROR);
        }
    } else if(argc == 3 && (strcmp(argv[1], "-c") == 0)) {
        config = spConfigCreate(argv[2], &msg);
        if(msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf(CONFIG_ERROR, argv[2]);
        }
    } else {
        printf(INVALID_COMMAND);
        return NULL;
    }
	return config;
}

bool initializeLogger(SPConfig config) {
    char path[MAX_LENGTH];
    SP_LOGGER_MSG msg;
    SP_CONFIG_MSG configMsg;
    SP_LOGGER_LEVEL level = (SP_LOGGER_LEVEL) spConfigGetLoggerLevel(config, &configMsg);
    if(level < 0) return false;
    spConfigGetLoggerFilename(path, config);
    msg = spLoggerCreate((strncmp(path, "stdout", MAX_LENGTH) == 0) ? NULL : path, level);
    if(msg == SP_LOGGER_OUT_OF_MEMORY) {
        printf(ALLOC_ERROR_MSG);
        return false;
    } else if(msg == SP_LOGGER_CANNOT_OPEN_FILE) {
        printf(LOG_FILE_ERROR);
        return false;
    }
    return true;
}

void freeFeatures(int i, int *nFeatures,  SPPoint*** featuresDatabase) {
    for(int j = 0; j<i; j++) {
        for(int k = 0; k<nFeatures[j]; k++) {
            spPointDestroy(featuresDatabase[j][k]);
        }
        free(featuresDatabase[j]);
    }
    free(featuresDatabase);
    free(nFeatures);
}

SPPoint** processFeatures(SPConfig config, int* totalFeatures, sp::ImageProc ip) {
    SP_CONFIG_MSG msg;
    char path[MAX_LENGTH];
    double *data;
    int numOfImgs, dimension, fd, *nFeatures, allFeatures = 0;
    SPPoint ***featuresDatabase, **flatDatabase;

    dimension = spConfigGetPCADim(config, &msg);
    numOfImgs = spConfigGetNumOfImages(config, &msg);
    nFeatures = (int*) calloc(numOfImgs, sizeof(*nFeatures));
    if (!nFeatures) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
    featuresDatabase = (SPPoint***) malloc(numOfImgs * sizeof(*featuresDatabase));
    if (!featuresDatabase) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        free(nFeatures);
		return NULL;
	}
    if (spConfigIsExtractionMode(config, &msg)) {
        // Extract features from images
        for(int i = 0; i < numOfImgs; i++) {
            msg = spConfigGetImagePath(path, config, i);
            featuresDatabase[i] = ip.getImageFeatures(path, i, nFeatures+i);
            if(!featuresDatabase[i]) {
                freeFeatures(i, nFeatures, featuresDatabase);
                return NULL;
            }
            //save to file
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR | O_CREAT, 0755);
            if(fd < 0) {
                spLoggerPrintWarning(WRITE_TO_FILE_ERROR, __FILE__, __func__, __LINE__);
            } else {
                write(fd, nFeatures+i, sizeof(int));
                for(int j = 0; j<nFeatures[i]; j++) {
                    write(fd, spPointGetData(featuresDatabase[i][j]), dimension * sizeof(double));
                }
                close(fd);
            }
        }
    } else {

        // Extract features from file
        data = (double*) malloc(dimension * sizeof(double));
        if (!data) {
            spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
            free(nFeatures);
            free(featuresDatabase);
            return NULL;
        }
        for(int i = 0; i<numOfImgs; i++) {
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR);
            if(fd < 0) {
                spLoggerPrintError(OPEN_FEATS_ERROR, __FILE__, __func__, __LINE__);
                freeFeatures(i, nFeatures, featuresDatabase);
                return NULL;
            }
            read(fd, nFeatures+i, sizeof(int));
            if(nFeatures[i] <= 0) {
                spLoggerPrintError(READ_FEATS_ERROR, __FILE__, __func__, __LINE__);
                freeFeatures(i, nFeatures, featuresDatabase);
                return NULL;
            }
            featuresDatabase[i] = (SPPoint**) malloc(nFeatures[i] * sizeof(SPPoint*));
            if(!featuresDatabase[i]) {
                spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
                freeFeatures(i, nFeatures, featuresDatabase);
                return NULL;
            }
            for(int j = 0; j<nFeatures[i]; j++) {
                read(fd, data, dimension * sizeof(double));
                featuresDatabase[i][j] = spPointCreate(data, dimension, i);
            }
            close(fd);
        }
        free(data);
    }

    // flattening point array so a kd tree can be created
    for(int i=0; i<numOfImgs; i++) {
        allFeatures += nFeatures[i];
    }
    flatDatabase = (SPPoint**) malloc(allFeatures * sizeof(SPPoint*));
    if(!flatDatabase) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        freeFeatures(numOfImgs, nFeatures, featuresDatabase);
        return NULL;
    }
    int k = 0;
    for(int i=0; i<numOfImgs; i++) {
        for(int j=0; j<nFeatures[i]; j++) {
            flatDatabase[k++] = featuresDatabase[i][j];
        }
        free(featuresDatabase[i]);
    }
    free(nFeatures);
    free(featuresDatabase);
    *totalFeatures = allFeatures;
    return flatDatabase;
}

kdTreeNode* initDataStructs(SPPoint** features, int numFeatures, SPConfig config) {
    SP_CONFIG_MSG msg;
    SPKDArray* kdArray = Init(features, numFeatures);
    for(int i = 0; i<numFeatures; i++) {
        spPointDestroy(features[i]);
    }
    free(features);
    if(!kdArray) return NULL;

    kdTreeNode* kdTree = (kdTreeNode*) malloc(sizeof(*kdTree));
    if(!kdTree) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        spKdArrayDestroy(kdArray);
        return NULL;
    }

    if(spKdTreeInit(kdArray, kdTree, spConfigGetKDTreeSplitMethod(config, &msg), 0) < 0) {
        spKdArrayDestroy(kdArray);
        return NULL;
    }
    spKdArrayDestroy(kdArray);
    return kdTree;
}

bool spEnterQueryImg(char* queryPath) {
	if (NULL == queryPath) {
		return false;
	}
	printf("Please enter an image path:\n");
	scanf("%s*c", queryPath);
	if (queryPath[0] == '<' && queryPath[1] == '>') {
		return false;
	}
	return true;
}

void searchByImage(SPConfig config, kdTreeNode* kdTree, sp::ImageProc ip) {
    char path[MAX_LENGTH];
    BPQueueElement* queueElement;
    SPBPQueue *featuresQueue, *imgQueue;
    int numOfImgs, kClosest, *featureHits, numQueryFeatures = 0;
    SP_CONFIG_MSG msg;
    SPPoint** queryFeatures;

    queueElement = (BPQueueElement*) malloc(sizeof(BPQueueElement));
    if(!queueElement) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        return;
    }
    featuresQueue = spBPQueueCreate(spConfigGetKNN(config, &msg));
    if(!featuresQueue) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        free(queueElement);
        return;
    }
    kClosest = spConfigGetNumOfSimilarImgs(config, &msg);
    imgQueue = spBPQueueCreate(kClosest);
    if(!imgQueue) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        free(queueElement);
        spBPQueueDestroy(featuresQueue);
        return;
    }
    numOfImgs = spConfigGetNumOfImages(config, &msg);

    while(spEnterQueryImg(path)) {
        queryFeatures = ip.getImageFeatures(path, numOfImgs, &numQueryFeatures);
        if(!queryFeatures) break;
        featureHits = (int*) calloc(numOfImgs, sizeof(int));
        if(!featureHits) {
            spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
            break;
        }
        for(int i = 0; i<numQueryFeatures; i++) {
            kNearestNeighbors(kdTree, featuresQueue, queryFeatures[i]);
            for(int j=0; j<spBPQueueSize(featuresQueue); j++) {
                spBPQueuePeek(featuresQueue, queueElement);
                featureHits[queueElement->index]++;
                spBPQueueDequeue(featuresQueue);
            }
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
        
        for(int i = 0; i<numQueryFeatures; i++) {
            spPointDestroy(queryFeatures[i]);
        }
        free(queryFeatures);
    }

    spBPQueueDestroy(imgQueue);
    spBPQueueDestroy(featuresQueue);
    free(queueElement);
}
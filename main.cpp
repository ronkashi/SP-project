

#include "main_aux.h"


#define ALLOC_ERROR_MSG "Allocation error"

int main(int argc, char* argv[]) {
    SPConfig config;
    SPPoint** flatDatabase;
    kdTreeNode* kdTree = NULL;
    int allFeatures=0;
    sp::ImageProc* ip;

    //Create config
    config = createConfig(argc, argv);
    if(!config) return -1;

    //Initialize logger
    if(!initializeLogger(config)) {
        spConfigDestroy(config);
        return -1;
    }

    //Initialize ImageProc inside a try catch block so we can safely exit if an Exception is thrown
    ip = (sp::ImageProc*) malloc(sizeof(*ip));
    if(!ip) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        spConfigDestroy(config);
    }
    try {
        *ip = sp::ImageProc(config);
    } catch(...) {
        free(ip);
        spLoggerDestroy();
        spConfigDestroy(config);
        return -1;
    }

    //Get all features from images or .feats files (depending on config)
    flatDatabase = processFeatures(config, &allFeatures, *ip);
    if(!flatDatabase) {
        spConfigDestroy(config);
        spLoggerDestroy();
		return -1;
    }

    //Initialize KD Tree data structure
    kdTree = initDataStructs(flatDatabase, allFeatures, config);
    if(!kdTree) {
        spConfigDestroy(config);
        spLoggerDestroy();
		return -1;
    }

    //Enter Query-Search-Result loop
    searchByImage(config, kdTree, *ip);

    //Free all remaining resources
    spKdTreeDestroy(kdTree);
    free(ip);
    spConfigDestroy(config);
    spLoggerDestroy();
    printf("Exiting...\n");
    return 0;
}

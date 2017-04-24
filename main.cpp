

#include "main_aux.h"


#define ALLOC_ERROR_MSG "Allocation error"

int main(int argc, char* argv[]) {
    SPConfig config;
    SPPoint** flatDatabase;
    kdTreeNode* kdTree = NULL;
    int allFeatures=0;
    sp::ImageProc* ip;

    config = createConfig(argc, argv);
    if(!config) return -1;

    if(!initializeLogger(config)) {
        spConfigDestroy(config);
        return -1;
    }

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
    

    flatDatabase = processFeatures(config, &allFeatures, *ip);
    if(!flatDatabase) {
        spConfigDestroy(config);
        spLoggerDestroy();
		return -1;
    }

    //initialize data structures
    kdTree = initDataStructs(flatDatabase, allFeatures, config);
    if(!kdTree) {
        spConfigDestroy(config);
        spLoggerDestroy();
		return -1;
    }

    searchByImage(config, kdTree, *ip);

    //TODO destroy tree
    free(ip);
    spConfigDestroy(config);
    spLoggerDestroy();
    printf("Exiting...\n");
    return 0;
}

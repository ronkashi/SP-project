

#include "main_aux.h"


#define ALLOC_ERROR_MSG "Allocation error"

int main(int argc, char* argv[]) {
    SPConfig config;
    SP_CONFIG_MSG msg;
    SPPoint** flatDatabase;
    kdTreeNode* kdTree = NULL;
    int allFeatures=0;

    config = createConfig(argc, argv);
    if(!config) return -1;

    if(!initializeLogger(config)) return -1;

    sp::ImageProc ip = sp::ImageProc(config);

    flatDatabase = processFeatures(config, &allFeatures, ip);
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

    searchByImage(config, kdTree);

    //TODO destroy tree
    spConfigDestroy(config);
    spLoggerDestroy();
    printf("Exiting...\n");
    return 0;
}

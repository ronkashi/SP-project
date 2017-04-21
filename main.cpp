#include <cstdio>
#include <cstring>

extern "C"{
    #include "SPConfig.h"
    #include "SPLogger.h"
    #include "SPPoint.h"
}

#include "SPImageProc.h"

int main(int argc, char* argv[]) {
    SPConfig config;
    SP_CONFIG_MSG msg;
    SPPoint*** featuresDatabase;
    int* nFeatures; 
    int numOfImgs;
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
    } else {
        printf("config created\n");
        //extract features
    }
    printf("Hello world!\n");
    numOfImgs = spConfigGetNumOfImages(config, &msg);
    printf("Num of images: %d\n", numOfImgs);
    sp::ImageProc ip = sp::ImageProc(config);
    nFeatures = (int*) malloc(numOfImgs * sizeof(*nFeatures));
    featuresDatabase = (SPPoint***) malloc(numOfImgs * sizeof(*featuresDatabase));
    for(int i = 0; i < numOfImgs; i++) {
        msg = spConfigGetImagePath(path, config, i);
        featuresDatabase[i] = ip.getImageFeatures(path, i, nFeatures+i);
    }
    ip.showImage(path);
    spConfigDestroy(config);
    free(nFeatures);
    free(featuresDatabase);
    return 0;
}
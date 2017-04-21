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
    int numOfImgs, totalFeatures = 0, dimension, fd;
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
    typedef struct { 
        int dim;
        int index;
        double data[dimension];
    } StaticPoint;

    numOfImgs = spConfigGetNumOfImages(config, &msg);
    nFeatures = (int*) malloc(numOfImgs * sizeof(*nFeatures));
    featuresDatabase = (SPPoint***) malloc(numOfImgs * sizeof(*featuresDatabase));

    if (spConfigIsExtractionMode(config, &msg)) {
        // Extract features from images
        sp::ImageProc ip = sp::ImageProc(config);
        for(int i = 0; i < numOfImgs; i++) {
            msg = spConfigGetImagePath(path, config, i);
            featuresDatabase[i] = ip.getImageFeatures(path, i, nFeatures+i);

            //save to file
            StaticPoint statPointArr[nFeatures[i]];
            for(int j = 0; j<nFeatures[i]; j++) {
                statPointArr[j].dim = dimension;
                statPointArr[j].index = i;
                memcpy(statPointArr[j].data, featuresDatabase[i][j]->data, dimension * sizeof(double));
            }
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR | O_CREAT, 0755);
            write(fd, nFeatures+i, sizeof(int));
            write(fd, &statPointArr, sizeof(statPointArr));
            close(fd);
        }
    } else {

        // Extract features from file
        for(int i = 0; i<numOfImgs; i++) {
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR);
            read(fd, nFeatures+i, sizeof(int));
            StaticPoint statPointArr[nFeatures[i]];
            read(fd, &statPointArr, sizeof(statPointArr));
            close(fd);
            featuresDatabase[i] = (SPPoint**) malloc(nFeatures[i] * sizeof(SPPoint*));
            for(int j = 0; j<dimension; j++) {
                featuresDatabase[i][j] = spPointCreate(&statPointArr[j].data, dimension, i);
            }
        }
    }

    // TODO initialize data structures

    while(getImageFromPath(path)) { //TODO implement bool getImageFromPath(char* path, ...)

        //TODO Extract features from image

        //TODO get nearest neighbours for each feature and save hits to array

        //TODO show results
        if(spConfigMinimalGui(config, &msg)) {
            //TODO showImagesWithGui()
        } else {
            //TODO print image results
        }
    }

    // TODO freeAllResourcesAndExit()
    
    spConfigDestroy(config);
    free(nFeatures);
    free(featuresDatabase);
    return 0;
}
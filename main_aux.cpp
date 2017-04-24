#include "main_aux.h"
#include "SPImageProc.h"

#include <cstdio>
#include <cstring>

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
    SP_CONFIG_MSG* msg;
	SPConfig config;
	if(argc == 1) {
        config = spConfigCreate(DEFAULT_CONFIG, msg);
        if(*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf(DEFAULT_CONFIG_ERROR);
        }
    } else if(argc == 3 && (strcmp(argv[1], "-c") == 0)) {
        config = spConfigCreate(argv[2], msg);
        if(*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf(CONFIG_ERROR, argv[2]);
        }
    } else {
        printf(INVALID_COMMAND);
        return NULL;
    }
	return config;
}

bool initializeLogger(SPConfig config) {
    char* path[MAX_LENGTH];
    SP_LOGGER_MSG msg;
    SP_CONFIG_MSG configMsg;
    int level = spConfigGetLoggerLevel(config, &configMsg);
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

int processFeatures(SPConfig config, SPPoint** flatDatabase, sp::ImageProc ip) {
    SP_CONFIG_MSG msg;
    char path[MAX_LENGTH];
    double *data;
    int numOfImgs, dimension, fd, *nFeatures, allFeatures;
    SPPoint ***featuresDatabase;

    dimension = spConfigGetPCADim(config, &msg);
    numOfImgs = spConfigGetNumOfImages(config, &msg);
    nFeatures = (int*) calloc(numOfImgs, sizeof(*nFeatures));
    if (!nFeatures) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return -1;
	}
    featuresDatabase = (SPPoint***) malloc(numOfImgs * sizeof(*featuresDatabase));
    if (!featuresDatabase) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        free(nFeatures);
		return -1;
	}
    if (spConfigIsExtractionMode(config, &msg)) {
        // Extract features from images
        for(int i = 0; i < numOfImgs; i++) {
            msg = spConfigGetImagePath(path, config, i);
            featuresDatabase[i] = ip.getImageFeatures(path, i, nFeatures+i);
            if(!featuresDatabase[i]) {
                freeFeatures(i, nFeatures, featuresDatabase);
                return -1;
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
            return -1;
        }
        for(int i = 0; i<numOfImgs; i++) {
            msg = spConfigGetFeatsPath(path, config, i);
            fd = open(path, O_RDWR);
            if(fd < 0) {
                spLoggerPrintError(OPEN_FEATS_ERROR, __FILE__, __func__, __LINE__);
                freeFeatures(i, nFeatures, featuresDatabase);
                return -1;
            }
            read(fd, nFeatures+i, sizeof(int));
            if(nFeatures[i] <= 0) {
                spLoggerPrintError(READ_FEATS_ERROR, __FILE__, __func__, __LINE__);
                freeFeatures(i, nFeatures, featuresDatabase);
                return -1;
            }
            featuresDatabase[i] = (SPPoint**) malloc(nFeatures[i] * sizeof(SPPoint*));
            if(!featuresDatabase[i]) {
                spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
                freeFeatures(i, nFeatures, featuresDatabase);
                return -1;
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
    int k = 0;
    for(int i=0; i<numOfImgs; i++) {
        for(int j=0; j<nFeatures[i]; j++) {
            *flatDatabase[k++] = featuresDatabase[i][j];
        }
        free(featuresDatabase[i]);
    }
    free(nFeatures);
    free(featuresDatabase);
    return allFeatures;
}
#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_


extern "C" {
    #include "SPConfig.h"
    #include "SPLogger.h"
    #include "SPPoint.h"
    #include "SPKDArray.h"
    #include "SPKDTree.h"
}

#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "SPImageProc.h"

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


SPConfig createConfig(int argc, char* argv[]);

bool initializeLogger(SPConfig config);

SPPoint** processFeatures(SPConfig config, int* allFeatures, sp::ImageProc ip);

kdTreeNode* initDataStructs(SPPoint** features, int numFeatures, SPConfig config);

bool spEnterQueryImg(char* queryPath);

void searchByImage(SPConfig config, kdTreeNode* kdTree, sp::ImageProc ip);

#endif

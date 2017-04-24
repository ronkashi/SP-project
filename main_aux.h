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

SPConfig createConfig(int argc, char* argv[]);

bool initializeLogger(SPConfig config);

SPPoint** processFeatures(SPConfig config, int* allFeatures, sp::ImageProc ip);

kdTreeNode* initDataStructs(SPPoint** features, int numFeatures);


bool spEnterQueryImg(char* queryPath);

#endif
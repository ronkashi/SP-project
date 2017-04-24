#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_


extern "C" {
    #include "SPPoint.h"
    #include "SPConfig.h"
}

SPConfig createConfig(int argc, char* argv[]);

bool initializeLogger(SPConfig config);

int processFeatures(SPConfig config, SPPoint** flatDatabase, sp::ImageProc ip);

bool spEnterQueryImg(char* queryPath);

#endif
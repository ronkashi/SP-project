#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_


extern "C" {
    #include "SPPoint.h"
    #include "SPConfig.h"
}

SPConfig createConfig(int argc, char* argv[], SP_CONFIG_MSG* msg);

bool initializeLogger(SPConfig config);

bool spEnterQueryImg(char* queryPath);

#endif
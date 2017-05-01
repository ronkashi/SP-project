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
#define LOG_FILE_ERROR "Cannot open logger file\n"
#define WRITE_TO_FILE_ERROR "features could not be saved to file"
#define OPEN_FEATS_ERROR "Features file could not be opened"
#define READ_FEATS_ERROR "Unable to read features from file"

/**
 * Receives the command line arguments and calls spConfigCreate with the appropriate parameters
 * the result is returned.
 */
SPConfig createConfig(int argc, char* argv[]);

/**
 * Receives the config and calls spLoggerCreate() with the appropriate parameters. 
 * Returns true if the initialization is successful. Returns false and prints error if the initialization fails.
 */
bool initializeLogger(SPConfig config);

/**
 * Receives the config, a pointer to an int where the total number of features for all images is stored
 * and the ImageProc object. 
 * If spExtractionMode is true the features are extracted using the ImageProc method,
 * otherwise, the features are read from the .feats files.
 * The returned array contains all of the features as SPPoints
 */
SPPoint** processFeatures(SPConfig config, int* allFeatures, sp::ImageProc ip);

/**
 * Receives an array with all of the features, the number of features and the config.
 * Creates a KDArray from the features, then creates a tree from the array and returns the tree root
 */
kdTreeNode* initDataStructs(SPPoint** features, int numFeatures, SPConfig config);

/**
 * Gets the image path input from user. Returns false if input=="<>", returns true otherwise
 */
bool spEnterQueryImg(char* queryPath);

/**
 * Receives the config struct, a KDTree with all of the features and the ImageProc object. Enters a loop where 
 * for each iteration an input is received from the user. If the input is not a terminating string, features are extracted for the image path that was input
 * and the nearest images to those features are displayed to the user.
 */
void searchByImage(SPConfig config, kdTreeNode* kdTree, sp::ImageProc ip);

#endif

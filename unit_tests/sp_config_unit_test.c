#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unit_test_util.h"
#include "../SPConfig.h"
#define MAX_LENGTH 1024


//Config is not defined
static bool basicConfigTest() {
    SPConfig config = NULL;
    char path[4] = {0};
	ASSERT_TRUE(spConfigGetImagePath(path, config, 0) == SP_CONFIG_INVALID_ARGUMENT);
	return true;
}

//config create with valid config file creates config with success message
static bool basicInitConfigTest() {
	const char* configFile = "test.config";
    SP_CONFIG_MSG msg;
    SPConfig config = spConfigCreate(configFile, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_FALSE(config == NULL);
	spConfigDestroy(config);
	return true;
}

//test all default values
static bool configDefaultValuesTest() {
	const char* configFile = "testDefault.config";
    char path[MAX_LENGTH];
    SP_CONFIG_MSG msg;
    SPConfig config = spConfigCreate(configFile, &msg);
	ASSERT_TRUE(spConfigGetPCADim(config, &msg) == 20);
    msg = spConfigGetPCAPath(path, config);
	ASSERT_TRUE(strncmp(path, "pca.yml", MAX_LENGTH));
    ASSERT_TRUE(spConfigGetNumOfFeatures(config, &msg) == 100);
    ASSERT_TRUE(spConfigIsExtractionMode(config, &msg));
    ASSERT_FALSE(spConfigMinimalGui(config, &msg));
    ASSERT_TRUE(spConfigGetNumOfSimilarImgs(config, &msg) == 1);
    ASSERT_TRUE(spConfigGetKNN(config, &msg) == 1);
    ASSERT_TRUE(spConfigGetKDTreeSplitMethod(config, &msg) == MAX_SPREAD);
    ASSERT_TRUE(spConfigGetLoggerLevel(config, &msg) == 3);
    msg = spConfigGetLoggerFilename(path, config);
	ASSERT_TRUE(strncmp(path, "stdout", MAX_LENGTH) == 0);
	spConfigDestroy(config);
	return true;
}

//test that values are initialized according to test config file
static bool configValuesTest() {
	const char* configFile = "test.config";
    char path[MAX_LENGTH];
    SP_CONFIG_MSG msg;
    SPConfig config = spConfigCreate(configFile, &msg);
	ASSERT_TRUE(spConfigGetPCADim(config, &msg) == 20);
    msg = spConfigGetPCAPath(path, config);
	ASSERT_TRUE(strncmp(path, "abcd.efg", MAX_LENGTH));
    ASSERT_TRUE(spConfigGetNumOfFeatures(config, &msg) == 88);
    ASSERT_FALSE(spConfigIsExtractionMode(config, &msg));
    ASSERT_FALSE(spConfigMinimalGui(config, &msg));
    ASSERT_TRUE(spConfigGetNumOfSimilarImgs(config, &msg) == 5);
    ASSERT_TRUE(spConfigGetKNN(config, &msg) == 7);
    ASSERT_TRUE(spConfigGetKDTreeSplitMethod(config, &msg) == MAX_SPREAD);
    ASSERT_TRUE(spConfigGetLoggerLevel(config, &msg) == 2);
    msg = spConfigGetLoggerFilename(path, config);
	ASSERT_TRUE(strncmp(path, "logger.com", MAX_LENGTH) == 0);
	spConfigDestroy(config);
	return true;
}

int main() {
	RUN_TEST(basicConfigTest);
	RUN_TEST(basicInitConfigTest);
    RUN_TEST(configDefaultValuesTest);
	RUN_TEST(configValuesTest);
	printf("Completed!\n");
	return 0;
}


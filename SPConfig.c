#include "SPConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LENGTH 1024

struct sp_config_t {
    char spImagesDirectory[MAX_LENGTH];
    char spImagesPrefix[MAX_LENGTH];
    char spImagesSuffix[MAX_LENGTH];
    int spNumOfImages;
    int spPCADimension;
    char spPCAFilename[MAX_LENGTH];
    int spNumOfFeatures;
    bool spExtractionMode;
    int spNumOfSimilarImages;
    CUT_METHOD spKDTreeSplitMethod;
    int spKNN;
    bool spMinimalGUI;
    int spLoggerLevel;
    char spLoggerFilename[MAX_LENGTH];
};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
    FILE* fd;
    SPConfig config;
    int line = 0;
    char lineBuffer[MAX_LENGTH];
    char *token1, *token2, *trimString;

    if(msg == NULL) {
        printf("Invalid argument");
        return NULL;
    }
    if(filename == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }
    fd = fopen(filename, "r");
    if(fd == NULL) {
        *msg = SP_CONFIG_CANNOT_OPEN_FILE;
        return NULL;
    }
    config = (SPConfig) malloc(sizeof(*config));
    if(config == NULL) {
        *msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }

    setConfigDefaults(config);

    while(fgets(lineBuffer, MAX_LENGTH, fd) != NULL) {
        line++;
        trimString = trimWhitespace(lineBuffer);
        if(*trimString != '#' && *trimString != 0) {
            token1 = strtok(trimString, "=");
            token2 = strtok(NULL, "=");
            if(token2 == NULL) {
                *msg = SP_CONFIG_INVALID_LINE;
                printf("File: %s\nLine: %d\nMessage: Invalid configuration line\n", filename, line);
                free(config);
                return NULL;
            }
            *msg = updateConfig(config, trimWhitespace(token1), trimWhitespace(token2), filename, line);
            if(*msg != SP_CONFIG_SUCCESS) {
                if(*msg != SP_CONFIG_INVALID_LINE) printf("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n", filename, line);
                free(config);
                return NULL;
            }
        }
    }
    if(*(config->spImagesDirectory) == '\0') {
        *msg = SP_CONFIG_MISSING_DIR;
        printMissingParameter(filename, "spImagesDirectory", line);
        free(config);
        return NULL;
    }
    if(*(config->spImagesPrefix )== '\0') {
        *msg = SP_CONFIG_MISSING_PREFIX;
        printMissingParameter(filename, "spImagesPrefix", line);
        free(config);
        return NULL;
    }
    if(*(config->spImagesSuffix) == '\0') {
        *msg = SP_CONFIG_MISSING_SUFFIX;
        printMissingParameter(filename, "spImagesSuffix", line);
        free(config);
        return NULL;
    }
    if(config->spNumOfImages == 0) {
        *msg = SP_CONFIG_MISSING_NUM_IMAGES;
        printMissingParameter(filename, "spNumOfImages", line);
        free(config);
        return NULL;
    }
    return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return NULL;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return NULL;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return -1;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return -1;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return -1;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index) {
    if(imagePath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }
    if(index >= config->spNumOfImages) {
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }
    snprintf(imagePath, MAX_LENGTH, "%s%s%d%s", config->spImagesDirectory, config->spImagesPrefix, index, config->spImagesSuffix);
    return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
    if(pcaPath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }
    snprintf(pcaPath, MAX_LENGTH, "%s%s", config->spImagesDirectory, config->spPCAFilename);
    return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config) {
    if(config != NULL) {
        free(config);
    }
}

int spConfigGetNumOfSimilarImgs(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return -1;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfSimilarImages;
}

int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return -1;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spKNN;
}

int spConfigGetLoggerLevel(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return -1;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spLoggerLevel;
}

SP_CONFIG_MSG spConfigGetLoggerFilename(char* loggerFilename, const SPConfig config) {
    if(loggerFilename == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }
    snprintf(loggerFilename, MAX_LENGTH, "%s", config->spLoggerFilename);
    return SP_CONFIG_SUCCESS;
}

CUT_METHOD spConfigGetKDTreeSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg) {
    if(msg == NULL) {
        printf("Invalid argument");
        return RANDOM;
    }
    if(config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return RANDOM;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spKDTreeSplitMethod;
}

void setConfigDefaults(SPConfig config) {
    *config = (struct sp_config_t) {.spPCADimension = 20, .spNumOfFeatures = 100, true, 1, MAX_SPREAD, 1, false, 3};
    snprintf(config->spPCAFilename, MAX_LENGTH, "pca.yml");
    snprintf(config->spLoggerFilename, MAX_LENGTH, "stdout");
}

SP_CONFIG_MSG updateConfig(const SPConfig config, char* variableName, char* value, const char* filename, int line) {
    if(strcmp(variableName, "spImagesDirectory") == 0) {
        if(containsSpace(value)) return SP_CONFIG_INVALID_STRING;
        snprintf(config->spImagesDirectory, MAX_LENGTH, "%s", value);
    } else if(strcmp(variableName, "spImagesPrefix") == 0) {
        if(containsSpace(value)) return SP_CONFIG_INVALID_STRING;
        snprintf(config->spImagesPrefix, MAX_LENGTH, "%s", value);
    } else if(strcmp(variableName, "spImagesSuffix") == 0) {
        if(strcmp(value, ".jpg") == 0 || strcmp(value, ".png") == 0 || strcmp(value, ".bmp") == 0 || strcmp(value, ".gif") == 0) {
            snprintf(config->spImagesSuffix, MAX_LENGTH, "%s", value);
        } else {
            return SP_CONFIG_INVALID_STRING;
        }
    } else if(strcmp(variableName, "spNumOfImages") == 0) {
        int num = getNumber(value);
        if(num <= 0) return SP_CONFIG_INVALID_INTEGER;
        config->spNumOfImages = num;
    } else if(strcmp(variableName, "spPCADimension") == 0) {
        int num = getNumber(value);
        if(num < 10 || num > 28) return SP_CONFIG_INVALID_INTEGER;
        config->spPCADimension = num;
    } else if(strcmp(variableName, "spPCAFilename") == 0) {
        if(containsSpace(value)) return SP_CONFIG_INVALID_STRING;
        snprintf(config->spPCAFilename, MAX_LENGTH, "%s", value);
    } else if(strcmp(variableName, "spNumOfFeatures") == 0) {
        int num = getNumber(value);
        if(num <= 0) return SP_CONFIG_INVALID_INTEGER;
        config->spNumOfFeatures = num;
    } else if(strcmp(variableName, "spExtractionMode") == 0) {
        if(strcmp(value, "true") == 0) {
            config->spExtractionMode = true;
        } else if(strcmp(value, "false") == 0) {
            config->spExtractionMode = false;
        } else {
            return SP_CONFIG_INVALID_BOOLEAN;
        }
    } else if(strcmp(variableName, "spNumOfSimilarImages") == 0) {
        int num = getNumber(value);
        if(num <= 0) return SP_CONFIG_INVALID_INTEGER;
        config->spNumOfSimilarImages = num;
    } else if(strcmp(variableName, "spKDTreeSplitMethod") == 0) {
        if(strcmp(value, "RANDOM") == 0) {
            config->spKDTreeSplitMethod = RANDOM;
        } else if(strcmp(value, "MAX_SPREAD") == 0) {
            config->spKDTreeSplitMethod = MAX_SPREAD;
        } else if(strcmp(value, "INCREMENTAL") == 0) {
            config->spKDTreeSplitMethod = INCREMENTAL;
        } else {
            return SP_CONFIG_INVALID_ENUM;
        }
    } else if(strcmp(variableName, "spKNN") == 0) {
        int num = getNumber(value);
        if(num <= 0) return SP_CONFIG_INVALID_INTEGER;
        config->spKNN = num;
    } else if(strcmp(variableName, "spMinimalGUI") == 0) {
        if(strcmp(value, "true") == 0) {
            config->spMinimalGUI = true;
        } else if(strcmp(value, "false") == 0) {
            config->spMinimalGUI = false;
        } else {
            return SP_CONFIG_INVALID_BOOLEAN;
        }
    } else if(strcmp(variableName, "spLoggerLevel") == 0) {
        int num = getNumber(value);
        if(num <= 0 || num > 4) return SP_CONFIG_INVALID_INTEGER;
        config->spLoggerLevel = num;
    } else if(strcmp(variableName, "spLoggerFilename") == 0) {
        if(containsSpace(value)) return SP_CONFIG_INVALID_STRING;
        snprintf(config->spLoggerFilename, MAX_LENGTH, "%s", value);
    } else {
        printf("File: %s\nLine: %d\nMessage: Invalid configuration line\n", filename, line);
        return SP_CONFIG_INVALID_LINE;
    }
    return SP_CONFIG_SUCCESS;
}

char *trimWhitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--; 
    *(end+1) = 0;
    return str;
}

bool containsSpace(char *str) {
    while(*str != 0) {
        if(isspace((unsigned char)*str)) return true;
        str++;
    }
    return false;
}

int getNumber(char *str) {
    char *end;
    int num = strtol(str, &end, 10);
    return *end ? -1 : num;
}

void printMissingParameter(const char *filename, const char *parameter, int lines) {
    printf("File: %s\nLine: %d\nMessage: Parameter %s is not set\n", filename, lines, parameter);
}

// int main() {
//     SP_CONFIG_MSG msg;
//     char str[MAX_LENGTH];
//     SPConfig config = spConfigCreate("spcbir.config", &msg);
//     if(config == NULL) {
//         printf("Create returned NULL with message: %d\n", msg);
//     }
//     printf("Extraction mode: %s\n", spConfigIsExtractionMode(config, &msg) ? "true" : "false");
//     printf("Minimal GUI: %s\n", spConfigMinimalGui(config, &msg) ? "true" : "false");
//     printf("Number of images: %d\n", spConfigGetNumOfImages(config, &msg));
//     printf("Number of Features: %d\n", spConfigGetNumOfFeatures(config, &msg));
//     printf("PCA Dimension: %d\n", spConfigGetPCADim(config, &msg));
//     spConfigGetImagePath(str, config, 3);
//     printf("Full image path: %s\n", str);
//     spConfigGetPCAPath(str, config);
//     printf("Full PCA path: %s\n", str);
//     printf("Similar images: %d\n", spConfigGetNumOfSimilarImgs(config, &msg));
//     printf("KNN: %d\n", spConfigGetKNN(config, &msg));
//     printf("Logger level: %d\n", spConfigGetLoggerLevel(config, &msg));
//     spConfigGetLoggerFilename(str, config);
//     printf("Logger filename: %s\n", str);
//     printf("Split method: %s\n", spConfigGetKDTreeSplitMethod(config, &msg) == 0 ? "RANDOM" : spConfigGetKDTreeSplitMethod(config, &msg) ? "MAX_SPREAD" : "INCREMENTAL");
//     return 0;
// }
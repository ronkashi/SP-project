#include "main_aux.h"

#include <cstdio>
#include <cstring>

#define DEFAULT_CONFIG "spcbir.config"
#define DEFAULT_CONFIG_ERROR "The default configuration file spcbir.config couldn't be open\n"
#define CONFIG_ERROR "The configuration file %s couldn't be open\n"
#define INVALID_COMMAND "Invalid command line: use -c <config_filename>\n"
#define CONFIG_ALLOC_ERROR "Config memmory allocation failure\n"

SPConfig createConfig(int argc, char* argv[], SP_CONFIG_MSG* msg) {
	SPConfig config;
	if(argc == 1) {
        config = spConfigCreate(DEFAULT_CONFIG, msg);
        if(*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf(DEFAULT_CONFIG_ERROR);
            return NULL;
        }
    } else if(argc == 3 && (strcmp(argv[1], "-c") == 0)) {
        config = spConfigCreate(argv[2], msg);
        if(*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
            printf(CONFIG_ERROR, argv[2]);
            return NULL;
        }
    } else {
        printf(INVALID_COMMAND);
        return NULL;
    }

    if(*msg != SP_CONFIG_SUCCESS) {
        printf(CONFIG_ALLOC_ERROR);
        return NULL;
    }
	return config;
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
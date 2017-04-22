#include "main_aux.h"

#include <stdio.h>


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
/*
 * shadowHelper.c
 *
 *  Created on: 17-Sep-2019
 *      Author: subhashpatel
 */


#include "shadowHelper.h"

#include <stdlib.h>
#include <string.h>

#include "cjson/cJSON.h"

void updateNumShadowParam(char *str, char *key, float value)
{
	cJSON *shadow;
	cJSON *state;
	cJSON *reported;

	shadow = cJSON_CreateObject();
	state = cJSON_CreateObject();
	reported = cJSON_CreateObject();

	cJSON_AddNumberToObject(reported, key, value);

	cJSON_AddItemToObject(state, "reported", reported);
	cJSON_AddItemToObject(shadow, "state", state);

	strcpy(str, cJSON_Print(shadow));


	cJSON_free(shadow);
	cJSON_free(state);
	cJSON_free(reported);
}

void updateStrShadowParam(char *str, char *key, char * value)
{
	cJSON *shadow;
	cJSON *state;
	cJSON *reported;

	shadow = cJSON_CreateObject();
	state = cJSON_CreateObject();
	reported = cJSON_CreateObject();

	cJSON_AddStringToObject(reported, key, value);

	cJSON_AddItemToObject(state, "reported", reported);
	cJSON_AddItemToObject(shadow, "state", state);

	strcpy(str, cJSON_Print(shadow));

	cJSON_free(shadow);
	cJSON_free(state);
	cJSON_free(reported);
}



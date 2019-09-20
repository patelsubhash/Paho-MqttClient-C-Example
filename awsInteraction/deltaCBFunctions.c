/*
 * deltaCBFunctions.c
 *
 *  Created on: 20-Sep-2019
 *      Author: shiv
 */


#include "deltaCBFunctions.h"
#include "cjson/cJSON.h"

#include <stdio.h>
#include <string.h>

void cbTemperature(cJSON *px)
{
	printf("Delta >> Temperature callback \n");
	printf("Desired Temperature = %f\n", px->valuedouble);
}

void cbLighBrightness(cJSON *px)
{
	printf("Delta >> LightBrightness callback \n");
	printf("Desired lightBrightness = %f\n", px->valuedouble);
}

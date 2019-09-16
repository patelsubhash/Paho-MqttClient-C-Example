/*
 * actionCBFunctions.c
 *
 *  Created on: 13-Sep-2019
 *      Author: shiv
 */


#include "actionCBFunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void actionONTopic01(void *pArg)
{
	printf("Action received on Topic01\n");
	char *pcIncomming;
	pcIncomming = (char *)pArg;

	char *msg;
	msg = (char *)malloc(strlen(pcIncomming)+1);
	memset(msg, 0, strlen(pcIncomming)+1);
	memcpy(msg, pcIncomming, strlen(pcIncomming));
	printf("Message = [%s]\n", msg);

	free(msg);
}
void actionONTopic02(void *pArg)
{
	printf("Action received on Topic02\n");
	char *pcIncomming;
	pcIncomming = (char *)pArg;

	char *msg;
	msg = (char *)malloc(strlen(pcIncomming)+1);
	memset(msg, 0, strlen(pcIncomming)+1);
	memcpy(msg, pcIncomming, strlen(pcIncomming));
	printf("Message = [%s]\n", msg);

	free(msg);
}
void actionONTopic03(void *pArg)
{
	printf("Action received on Topic03\n");
	char *pcIncomming;
	pcIncomming = (char *)pArg;

	char *msg;
	msg = (char *)malloc(strlen(pcIncomming)+1);
	memset(msg, 0, strlen(pcIncomming)+1);
	memcpy(msg, pcIncomming, strlen(pcIncomming));
	printf("Message = [%s]\n", msg);

	free(msg);

}


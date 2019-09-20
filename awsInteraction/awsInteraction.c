

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cjson/cJSON.h"
#include "MQTTClient.h"

#include "shadowHelper.h"
#include "shadowInteract.h"
#include "deltaCBFunctions.h"

#define THING 		"THING_NAME"

#define ADDRESS		"ssl://xxxxxxxxxxxxxx-ats.iot.us-east-1.amazonaws.com:8883"

#define ROOTCA		"rootCA.crt"
#define PRIVATEKEY	"privkey.pem"
#define CLIENTCERT	"cert.pem"

#define CLIENTID    "Device-Shiv03"


void shadowReadCB (char *shadow);

int main(void)
{
	printf("Example for interacting with AWS-Shadow\n");

	xAWSThing myThing = INIT_XAWSTHING;
	xDeltaCB OnDelta[] = {
			{"Temperature", cbTemperature},
			{"lightBrightness", cbLighBrightness},
			{NULL, NULL}
	};


	myThing.listDeltaCB = OnDelta;
	initThing(&myThing, THING, ADDRESS, ROOTCA, PRIVATEKEY, CLIENTCERT, CLIENTID);
	connectToThing(&myThing);

	getShadow(&myThing, shadowReadCB);

	char shadowDoc[1024];
	int Temperature = 10;

	while(1)
	{
		if(isConnectedToBroker(&myThing))
		{
			updateNumShadowParam(shadowDoc, "Temperature", Temperature%35);
			printf("Shadow: %s\n", shadowDoc);
			updateShadow(&myThing, shadowDoc);
			Temperature++;
			sleep(30);
		}
	}


	return 0;
}

void shadowReadCB (char *shadow)
{
	printf("Shadow read callback is called \n");
	printf("================================\n");
	printf("%s\n", shadow);
	printf("================================\n");
}

/*
 * shadowInteract.c
 *
 *  Created on: 18-Sep-2019
 *      Author: subhashpatel
 */

#include "shadowInteract.h"

#include "MQTTClient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Local functions
void connlost(void *context, char *cause);
void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);


//Implementation of local functions
void connlost(void *context, char *cause)
{
	xAWSThing *xIN;
	xIN = (xAWSThing *)context;

	printf("%s >> Connection lost : Reason : [%s]\r\n", xIN->thingName, cause);
	printf("%s >> Retrying connection \n", xIN->thingName);
	connectToThing(xIN);
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{

}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{

	xAWSThing *xIN;
	xIN = (void *)context;

	char *msg;
	msg = (char *)malloc(message->payloadlen+1);
	memset(msg, 0, message->payloadlen);
	memcpy(msg, message->payload, message->payloadlen);

	char awsTopic[256];

	//AWS Topic check

	sprintf(awsTopic, "$aws/things/%s/shadow/update/delta", xIN->thingName);
	if(strcmp(awsTopic, topicName) == 0)
	{
		printf("Delta message is received\n");
		cJSON *root;
		cJSON *state;
		root = cJSON_Parse(msg);
		if(root)
		{
			state = cJSON_GetObjectItem(root, "state");
			for(int i = 0; ; i++)
			{
				if(xIN->listDeltaCB[i].key == NULL)
				{
					break;
				}
				if((xIN->listDeltaCB[i]).deltaCB != NULL)
				{
					if(cJSON_HasObjectItem(state, xIN->listDeltaCB[i].key))
					{
						(&(xIN->listDeltaCB[i]))->deltaCB(cJSON_GetObjectItem(state, xIN->listDeltaCB[i].key));
					}
				}

			}
		}
		cJSON_free(state);
		cJSON_free(root);
		free(msg);
		return 1;
	}

	sprintf(awsTopic, "$aws/things/%s/shadow/get/accepted", xIN->thingName);
	if(strcmp(awsTopic, topicName) == 0)
	{
		printf("Shadow is received\n");
		xIN->pfShadowReadCB(msg);
		free(msg);
		return 1;
	}


	printf("Message is arrived\n" );
	free(msg);
	return 1;
}

int initThing(xAWSThing *px, char *Name, char *url, char *rootCaFile, char *privateKey, char *clientCert, char *clientName)
{
	px->thingName = Name;

	px->url = url;

	px->xMqttSSL.verify = 1;
	px->xMqttSSL.trustStore = rootCaFile;
	px->xMqttSSL.keyStore = clientCert;
	px->xMqttSSL.privateKey = privateKey;
	px->xMqttSSL.enableServerCertAuth = 1;
	px->xMqttSSL.struct_version = 1;
	px->xMqttSSL.sslVersion = 3;

	px->xMqttConOpt.ssl = &px->xMqttSSL;
	px->xMqttConOpt.cleansession = 1;
	px->xMqttConOpt.keepAliveInterval = 3;


	int rc;
	rc = MQTTClient_create(&px->xMqttClient, url, clientName, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	if(rc != MQTTCLIENT_SUCCESS)
		return rc;

	//Register callback functions
	px->pfOnConnLost = connlost;
	px->pfOnMsgDelivered = delivered;
	px->pfOnMsgArrived = msgarrvd;

	MQTTClient_setCallbacks(px->xMqttClient, (void *)px , px->pfOnConnLost, px->pfOnMsgArrived, px->pfOnMsgDelivered);

	return 1;
}
int connectToThing(xAWSThing *px)
{
	int rc;
	char shadowTopic[256];
	do
	{
		printf("Connecting to broker \n");
		rc = MQTTClient_connect(px->xMqttClient, &px->xMqttConOpt);
		if(rc != MQTTCLIENT_SUCCESS)
		{
			printf("Failed to connect with connect failure reason = %d \n", rc);
			sleep(1);
		}
		else
		{
			printf("Connected Successfully \n");
		}
	}while(rc != MQTTCLIENT_SUCCESS);

	//Subscribe to shadow-get
	sprintf(shadowTopic, "$aws/things/%s/shadow/get/accepted", px->thingName);
	rc = MQTTClient_subscribe(px->xMqttClient, shadowTopic, 0);
	if(rc != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_disconnect(px->xMqttClient, 1);
		return rc;
	}

	//Subscribe to delta
	sprintf(shadowTopic, "$aws/things/%s/shadow/update/delta", px->thingName);
	rc = MQTTClient_subscribe(px->xMqttClient, shadowTopic, 0);
	if(rc != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_disconnect(px->xMqttClient, 1);
		return rc;
	}
	return MQTTCLIENT_SUCCESS;
}

int isConnectedToBroker(xAWSThing *px)
{
	if(MQTTClient_isConnected(px->xMqttClient))
	{
		return 1;
	}
	else
		return 0;
}

int updateShadow(xAWSThing *px, char *pcShadowDoc)
{
	char shadowTopic[256];
	sprintf(shadowTopic, "$aws/things/%s/shadow/update", px->thingName);
	return MQTTClient_publish(px->xMqttClient, shadowTopic, strlen(pcShadowDoc), pcShadowDoc, 0, 0, NULL);
}

int getShadow(xAWSThing *px, void (*cb) (char *))
{
	int rc;
	char shadowTopic[256];
	sprintf(shadowTopic, "$aws/things/%s/shadow/get", px->thingName);

	rc = MQTTClient_publish(px->xMqttClient, shadowTopic, 0, 0, 0, 0, NULL);
	if(rc != MQTTCLIENT_SUCCESS)
		return -1;
	px->pfShadowReadCB = cb;
	return MQTTCLIENT_SUCCESS;
}

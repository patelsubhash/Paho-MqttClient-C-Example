/*
 * shadowInteract.h
 *
 *  Created on: 18-Sep-2019
 *      Author: subhashpatel
 */

#ifndef SHADOWINTERACT_H_
#define SHADOWINTERACT_H_

#include "MQTTClient.h"

#include "cjson/cJSON.h"

#define INIT_XAWSTHING {NULL, NULL, NULL, MQTTClient_connectOptions_initializer, MQTTClient_SSLOptions_initializer}

typedef struct __xDeltaCB__
{
	char *key;
	void (*deltaCB)(cJSON *);
}xDeltaCB;
typedef struct __xThing__
{
	char *thingName;

	char *url;

	MQTTClient xMqttClient;
	MQTTClient_connectOptions xMqttConOpt;
	MQTTClient_SSLOptions xMqttSSL;

	void (*pfOnConnLost) (void *context, char *cause);
	void (*pfOnMsgDelivered)(void *context, MQTTClient_deliveryToken dt);
	int (*pfOnMsgArrived)(void *context, char *topicName, int topicLen, MQTTClient_message *message);

	void (*pfShadowReadCB)(char *);

	xDeltaCB *listDeltaCB;
}xAWSThing;

int initThing(xAWSThing *px, char *Name, char *url, char *rootCaFile, char *privateKey, char *clientCert, char *clientName);
int connectToThing(xAWSThing *px);
int isConnectedToBroker(xAWSThing *px);
int updateShadow(xAWSThing *px, char *pcShadowDoc);
int getShadow(xAWSThing *px, void (*cb) (char *));

#endif /* SHADOWINTERACT_H_ */

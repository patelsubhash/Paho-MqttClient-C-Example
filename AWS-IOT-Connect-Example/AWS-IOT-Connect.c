
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "MQTTClient.h"
#include "actionCBFunctions.h"

#define ADDRESS		 "ssl://xxxxxxxxxxxxxxx.iot.us-east-1.amazonaws.com:8883" //put here the thing end-point

#define ROOTCA		"rootCA.crt"		//Root certificate of AWS
#define PRIVATEKEY	"privkey.pem"   	//Private key downloaded during the creation of thing
#define CLIENTCERT	"cert.pem"      	//Client certificate downloaded during the creation of thing
#define CLIENTID    	"AWS-IOT-Connect-01"

#define QOS         	0
#define TIMEOUT     	10000L

int continueWait = 1;

//Structure for topic vs cb
typedef struct __xAction__
{
	char *topic;
	void (*cb) (void *pCBP);
}xActionONMsg;

//structure for MQTT information
typedef struct __xMQTTConnetionData__
{
	MQTTClient *xMqttClient;
	MQTTClient_connectOptions *xMqttConnOpt;
	char **listOfTopicsToSubscribe;
	char *msg;
	xActionONMsg *pActionTopicMaping;
}xMQTTConnectionData;

//Callback Function prototypes
void connlost(void *context, char *cause);
void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);


//Other useful functions
int connectToServer(xMQTTConnectionData *pV);

//On connection lost
void connlost(void *context, char *cause)
{
	printf("Connection lost : Reason : [%s]\r\n", cause);
	continueWait = 0;
	xMQTTConnectionData * xIncomming;
	xIncomming = (xMQTTConnectionData *)context;
	printf("MSG : %s\n", xIncomming->msg);
	printf("Retrying connection ... \n");
	connectToServer(xIncomming);
}

//On message delivered
void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("On publish callback \n");
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	char *msg;
	xMQTTConnectionData *pxIncomming;

	pxIncomming = (xMQTTConnectionData *)context;

	msg = (char *)malloc(message->payloadlen+1);
	memset(msg, 0, message->payloadlen+1);
	memcpy(msg, message->payload, message->payloadlen);

	printf("[%s] has been arrived on topic : [%s]\n", msg, topicName );


	xActionONMsg *pc = pxIncomming->pActionTopicMaping;

	while(pc->topic != NULL)
	{
		if(pc->cb != NULL)
		{
			if(strcmp(pc->topic, topicName) == 0)
			{
				pc->cb(msg);
				break;
			}
		}
		pc++;
	}

	MQTTClient_free(topicName);
	MQTTClient_freeMessage(&message);
	free(msg);
	return 1;
}

int connectToServer(xMQTTConnectionData *pV)
{
	int rc;
	do
	{
		printf("Trying to establish connection to broker\n");
		rc = MQTTClient_connect(*(pV->xMqttClient), pV->xMqttConnOpt);
		if(rc != MQTTCLIENT_SUCCESS)
		{
			printf("Retrying connection in 1 second \n");
			printf("\n Reason = [%d]\n", rc);
			sleep(1);
		}
		else
		{
			printf("Connection is established \n");
		}

	}while(rc != MQTTCLIENT_SUCCESS);

	int i = 0;
	while(pV->listOfTopicsToSubscribe[i] != NULL)
	{
		rc = MQTTClient_subscribe(*(pV->xMqttClient), pV->listOfTopicsToSubscribe[i], 0);
		if(rc == MQTTCLIENT_SUCCESS)
		{
			printf("Subscribed to topic [%s]\n", pV->listOfTopicsToSubscribe[i]);
		}
		else
		{
			printf("Failed to subscribe to topic [%s]\n", pV->listOfTopicsToSubscribe[i]);
		}
		i++;
	}

	//rc = MQTTClient_subscribe(*(pV->xMqttClient), opts.topic, opts.qos)
	return MQTTCLIENT_SUCCESS;
}

int main()
{
	printf("This program will show the use of Paho Mqtt library with SSL connection\n");


	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;

	ssl_opts.verify = 1;
	ssl_opts.trustStore = ROOTCA;
	ssl_opts.privateKey = PRIVATEKEY;
	ssl_opts.keyStore = CLIENTCERT;
	conn_opts.ssl = &ssl_opts;

	char MSG[] = "Hello";

	char *subTopics[] = {"Topic01", "Topic02", "Topic03", NULL};

	xActionONMsg pxObjActionONMsg[] = {
			{subTopics[0], actionONTopic01},
			{subTopics[1], actionONTopic02},
			{subTopics[2], actionONTopic03},
			{NULL, NULL},
	};


	xMQTTConnectionData xObjMqttConnectionData;
	xObjMqttConnectionData.msg = MSG;
	xObjMqttConnectionData.xMqttClient = &client;
	xObjMqttConnectionData.xMqttConnOpt = &conn_opts;
	xObjMqttConnectionData.listOfTopicsToSubscribe = subTopics;
	xObjMqttConnectionData.pActionTopicMaping = pxObjActionONMsg;

	MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 10;
	conn_opts.cleansession = 1;
	//conn_opts.username = USER;
	//conn_opts.password = PASS;

	//MQTTClient_setCallbacks(client, (void *)&xObjMqttConnectionData , connlost, msgarrvd, delivered);
	MQTTClient_setCallbacks(client, (void *)&xObjMqttConnectionData , connlost, msgarrvd, NULL);

	while(connectToServer(&xObjMqttConnectionData) != MQTTCLIENT_SUCCESS);


	printf("Connected to the MQTT Broker\n");
	int msgcnt = 1;
	char publish_msg[256];
	while(1)
	{
		if(MQTTClient_isConnected(*(xObjMqttConnectionData.xMqttClient)))
		{
			sprintf(publish_msg, "Message = %d", msgcnt);
			MQTTClient_publish(*(xObjMqttConnectionData.xMqttClient), "Topic01", strlen(publish_msg), publish_msg, 0, 0, NULL);
			sleep(60);
			msgcnt++;
		}
	}

	printf("Leaving the program\n");
	return 0;
}

//Fichier : connection.h
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
//Status  : test

//#include "conversion.h"
#ifndef CONNETION_H
#define CONNECTION_H
#endif


// Header code here

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <syslog.h>

#include <json.h>
#include <MQTTClient.h>
#include "cJSON.h"
#include "type.h"
#include "dictionary.h"

#define MQTTCLIENT_PERSISTENCE 0
#define ADDRESS     "192.168.2.3:1883"
#define CLIENT_ID	  "Regul_elec"

#define XCOM_READ 1
#define XCOM_WRITE 2

#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L
;

void connlost(void *context, char *cause);

/*-----------------------MQTT handler --------------------------------------------------------*/
volatile MQTTClient_deliveryToken deliveredtoken;
void delivered(void *context, MQTTClient_deliveryToken dt);

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/*---------------------- Routine de lecture d'un parametre sur l'Xtender -----------------------*/
int Read(t_param* Parametre,MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/
/*-------------------- Routine de lectures des parametres necessaires pour l'algo -------------*/
void Read_p(MQTTClient *client);
/*----------------------------------------------------------------------------------------------*/

/*---------------------- Routine d'ecriture d'un parametre sur l'Xtender -----------------------*/
int Write(t_param* Parametre,MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/
/*--------------- Routine Ecriture, ecriture des parametres modifie dans l'algo ----------------*/
void Write_p(MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/

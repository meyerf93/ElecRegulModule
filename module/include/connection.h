//Fichier : connection.h
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
//Status  : test

//#include "conversion.h"
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


#include "type.h"
#include <json.h>
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "MQTTAsync.h"
#include "cJSON.h"

#define MQTTCLIENT_PERSISTENCE 0
#define ADDRESS     "192.168.2.3:1883"
#define CLIENTID_CHARGER	"Regul_elec_charger"
#define CLIENTID_BAT 			"Regul_elec_bat"

#define TOPIC  			"new.statements"
#define TOPIC_WRITE "device.command.set.value"

#define XCOM 				"xcom1"
#define XCOM_ID_CHARGER "xcom1/:101/:/config"
#define XCOM_ID_BAT "xcom1/:600/:/config"

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
/*---------------------- Routine de lecture d'un parametre sur l'Xtender -----------------------*/
int Read_bat(t_param* Parametre,MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/
/*-------------------- Routine de lectures des parametres necessaires pour l'algo -------------*/
void Read_p(MQTTClient *client,MQTTClient *client_bat);
/*----------------------------------------------------------------------------------------------*/

/*---------------------- Routine d'ecriture d'un parametre sur l'Xtender -----------------------*/
int Write(t_param* Parametre,MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/

/*---------------------- Routine d'ecriture d'un parametre sur l'Xtender -----------------------*/
int Write_bat(t_param* Parametre,MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/

/*--------------- Routine Ecriture, ecriture des parametres modifie dans l'algo ----------------*/
void Write_p(MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/


/*------------- Routine d'initialization de touts les parametres et de l'Xtender ---------------*/
int Init(MQTTClient* client);
/*----------------------------------------------------------------------------------------------*/

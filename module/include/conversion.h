//Fichier : conversion.h
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
//Status  : test

//#include "connection.h"
//#include "cJSON.h"

//#include <json.h>
//#include <MQTTClient.h>
#define BOOL_F 1
#define FLOAT_F 6
#define INT32_F 5
#define LONG_ENUM 10
#define SHORT_ENUM 11
#define ENUM 3
;
void send_json_obj(MQTTClient client,char topic[64], char data[64], char mdl[64], char id[64]);

int parse_energy_meters(char* payload);

void parse_studer_message(char* payload);

void parse_batt_message(char* payload);

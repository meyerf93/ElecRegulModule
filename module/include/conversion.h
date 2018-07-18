//Fichier : conversion.h
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
//Status  : test

#ifndef _CONVERSION_H
#define _CONVERSION_H

//#include "connection.h"
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

#include "cJSON.h"
#include <json.h>
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "MQTTAsync.h"

#define BOOL_F 1
#define FLOAT_F 6
#define INT32_F 5
#define LONG_ENUM 10
#define SHORT_ENUM 11
#define ENUM 3

#define SPACE "\t\r\n"


void strip(char *s,char * to_remove);


void send_json_obj(MQTTClient client,char topic[64], char data[64], char mdl[64], char id[64]);

int parse_energy_meters(char* payload);

void parse_studer_message(char* payload,cJSON* data);

void parse_batt_message(char* payload,cJSON* data);

#endif

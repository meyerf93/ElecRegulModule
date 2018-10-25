//Fichier : coversion.c:
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.2
//Date 	  : 28.08.2018
//Status  : test
#include "connection.h"
#include "conversion.h"
#include "cJSON.h"
#include "Write_read_v20.h"

#include <json.h>
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "MQTTAsync.h"
#include "type.h"
;

void strip(char *s,const char * to_remove)
{
    //char *p;
    //p = malloc(100*sizeof(char));
    char *p = s;
    int n;
    while (*s)
    {
        //printf(" in loop of strip");
        n = strcspn(s, to_remove);
        strncpy(p, s, n);
        p += n;
        s += n + strspn(s+n, to_remove);
    }
    //free(p);
    *p = 0;
}


// MQTTT communication with studer
void send_json_obj(MQTTClient client,char topic[64], char data[64], char id[64])
{
  printf("value receive in send_json : %s,%s,%s\n",topic,data,id);
  cJSON * root;
	root = cJSON_CreateObject();
  cJSON_AddStringToObject(root,"cmd",id);
  cJSON_AddStringToObject(root,"value",data);

  /*char temp_playload[100] ;
  if (!cJSON_PrintPreallocated(root,*temp_playload, 100, true)){
    printf("error when parsin file");
  }*/
	char* payload_json = calloc(100,1);
  payload_json = cJSON_Print(root);
  cJSON_Delete(root);
  //printf("message xcom to send json : %s\n",payload_json);
	strip(payload_json,SPACE);
	char *temp_ptr = strstr(payload_json,"\"[");
	//printf("%s\n",temp_ptr);
	strip(temp_ptr,"\"");

	MQTTClient_deliveryToken token;

	MQTTClient_message pubmsg = MQTTClient_message_initializer;
  pubmsg.payload = payload_json;
  pubmsg.payloadlen = strlen(payload_json);
  pubmsg.qos = QOS;
  pubmsg.retained = 0;

  printf("create the message to send\n");

	printf("json message send : %s\n",pubmsg.payload);
  MQTTClient_publishMessage(client, topic, &pubmsg, &token);
  printf("Waiting for up to %d seconds for publication of %s\n"
        "on topic %s for client with ClientID: %s\n",
        (int)(TIMEOUT/1000), payload_json, topic, id);
  MQTTClient_waitForCompletion(client, token, TIMEOUT);
  printf("message send`\n ");
  //MQTTClient_freeMessage(&pubmsg);
  MQTTClient_free(topic);
  printf("Message with delivery token %d delivered\n", token);
  free(payload_json);

}

//Parse the message from MQTT
int parse_energy_meters(char* payload){
  if (strstr(payload,PV_WEST) != NULL) {
		return 0;
	}
	else if (strstr(payload,PV_EAST) != NULL) {
		return 1;
	}
	else if (strstr(payload,PV_SOUTH) != NULL){
		return 2;
	}
	else if (strstr(payload,SECURE_LOAD)!= NULL) {
		return 3;
	}
	else if (strstr(payload,OVEN)!= NULL) {
		return 4;
	}
	else if (strstr(payload,COOKTOP) != NULL){
		return 5;
	}
	else if (strstr(payload,CAR_CHARGER) != NULL){
		return 6;
	}
	else if (strstr(payload,NORTH_RECEP)!= NULL) {
		return 7;
	}
	else if (strstr(payload,SOUTH_RECEP)!= NULL) {
		return 8;
	}
	else if (strstr(payload,SKIN_RECEP)!= NULL) {
		return 9;
	}
	else if (strstr(payload,BATTERY_IN)!= NULL) {
		return 10;
	}
	else if (strstr(payload,OPTI_KG) != NULL){
		return 11;
	}
	else if (strstr(payload,OPTI_PROD) != NULL){
		return 12;
	}
	else if (strstr(payload,OPTI_CONSO) != NULL){
		return 13;
	}
	else if (strstr(payload,OPTI_POWER) != NULL){
		return 14;
	}
  else return -1;
}

int parse_studer_message(char* payload,cJSON* data) {
  char* temp_json = calloc(100,1);
  temp_json = cJSON_Print(data);
  printf("data to search in the dictionnary : %s\n",temp_json);
  t_param *temp_parameter = dict_get(Parameter_dic,payload);
  if(temp_parameter != NULL){
    temp_parameter->Value=(float)data->valuedouble;
  }
  else {
    printf("error can't find : %s, in the dictionary˙\n",payload);
    return  -1;
  }
  return 0;
}

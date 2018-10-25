//Fichier : connection.c
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.2
//Date 	  : 28.08.2018
//Status  : test
#include "connection.h"
#include "conversion.h"
#include "cJSON.h"

#include <json.h>
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "MQTTAsync.h"
#include "Write_read_v20.h"
;
volatile MQTTClient_deliveryToken deliveredtoken;

void connlost(void *context, char *cause)
{
    UNUSED(context);
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

/*-----------------------MQTT handler --------------------------------------------------------*/
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    UNUSED(context);
    //printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  printf("message arrived on the topic : %s\n",topicName);
  printf("message is : %s\n",message->payload);
    /* EXEMPLE CODE MQTT*/
  cJSON * root;
  root = cJSON_CreateObject();
  cJSON *data;
  data = cJSON_CreateObject();

  UNUSED(context);
  UNUSED(topicLen);

  printf("init  the csjon object\n");

	int select_meters;

  root = cJSON_Parse(message->payload);
  printf("parse the maessage with payload\n");
  data = cJSON_GetObjectItemCaseSensitive(root, "data");
  printf("get the object data in root\n");

  /*char* temp_json = calloc(300,1);
  temp_json = cJSON_Print(root);
  printf("data to parse : %s\n",temp_json);

  char* temp_data = calloc(300,1);
  temp_data = cJSON_Print(data);
  printf("data to parse : %s\n",temp_data);*/

	select_meters = parse_energy_meters(topicName);
  printf("select the metesr\n");
	//extract data ------------------------------
	if(select_meters != -1){
		if (cJSON_IsNumber(data))
		{
  		meters[select_meters] = data->valuedouble;
		}
		Ppv = meters[0]+meters[1]+meters[2];
		Pl = meters[3]+meters[4]+meters[5]+meters[6]+meters[7]+meters[8]+meters[9]; //+ meters[10];
		Kg = meters[11];
		Ps_opti = meters[14];
	}
  printf("parse the energy meters\n");

	//parse packet for xcom
  if(parse_studer_message(topicName,data) == -1){
    printf("ERROR : can't parse the message");
  }
  printf("parse the parameters\n");
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);

  printf("parsing of the message is done\n");

   if(root != NULL) {
     //printf("root is not null, data : %s\n",cJSON_Print(data));
     //printf("root is not null, root : %s\n",cJSON_Print(root));
     cJSON_Delete(root);
   }
   if(data != NULL) {
     //printf("data is not null, data : %s\n",cJSON_Print(data));
     //printf("data is not null, root : %s\n",cJSON_Print(root));
     cJSON_Delete(data);
     //printf("data is not null, data after clean : %s\n",cJSON_Print(data));
     //printf("data is not null, root after clean : %s\n",cJSON_Print(root));
   }
	 return 1;
}


/*---------------------- Routine de lecture d'un parametre sur l'Xtender -----------------------*/
int Read(t_param* Parametre,MQTTClient* client)
{
  char data[64];
  sprintf(data,"[%d,%d,%d,%d,%d]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_READ);
  //printf("data : %s\n",data);
	send_json_obj(client,Parametre->Id_write,data,Parametre->Id_write);
	return 1;
}
/*----------------------------------------------------------------------------------------------*/

/*-------------------- Routine de lectures des parametres necessaires pour l'algo -------------*/
void Read_p(MQTTClient *client)
{
 	Read(&i_Battery_Voltage_Studer,client);
 	Read(&i_Input_voltage_AC_IN,client);
 	Read(&i_State_of_charge,client);
 	Read(&i_Battery_Charge_current,client);
 	Read(&i_Output_voltage_AC_OUT,client);
 	Read(&i_Output_power_AC_OUT,client);
  Read(&i_Battery_cycle_phase,client);
  Read(&i_Input_current_AC_IN,client);
 	Read(&i_Battery_Voltage,client);
	Read(&i_Battery_Current,client);
	Read(&i_soc_value_battery,client);
	Read(&i_State_of_Health,client);
	Read(&i_Battery_Voltage_Charge_limit,client);
	Read(&i_Battery_Voltage_Discharge_limit,client);
	Read(&i_Battery_Current_Charge_limit,client);
	Read(&i_Battery_Current_Discharge_limit,client);
	Read(&i_Battery_Current_Charge_recommanded,client);
	Read(&i_Battery_Current_Discharge_recommanded,client);
}
/*----------------------------------------------------------------------------------------------*/


/*---------------------- Routine d'ecriture d'un parametre sur l'Xtender -----------------------*/
int Write(t_param* Parametre,MQTTClient* client)
{
	char data[64];
	switch (Parametre->Format)
	{
		case BOOL_F:
		if(Parametre->Value == 1)
		{
			sprintf(data,"[%d,%d,%d,%d,%d,%s]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_WRITE,"true");
		}
		else
		{
			sprintf(data,"[%d,%d,%d,%d,%d,%s]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_WRITE,"false");
		}
		break;
		case FLOAT_F:
		sprintf(data,"[%d,%d,%d,%d,%d,%f]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_WRITE,Parametre->Value);
		break;
		case INT32_F:
		sprintf(data,"[%d,%d,%d,%d,%d,%d]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_WRITE,(int)Parametre->Value);
		break;
		case LONG_ENUM:
		sprintf(data,"[%d,%d,%d,%d,%d,%d]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_WRITE,(uint32_t)Parametre->Value);
		break;
		case ENUM:
		case SHORT_ENUM:
		sprintf(data,"[%d,%d,%d,%d,%d,%d]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_WRITE,(uint16_t)Parametre->Value);
		break;
		default:
		printf("ERROR : can't convert this format\n");
	}

	//printf("data : %s\n",data);
	send_json_obj(client,Parametre->Id_write,data,Parametre->Id_write);
	return 1;
}
/*----------------------------------------------------------------------------------------------*/

/*--------------- Routine Ecriture, ecriture des parametres modifie dans l'algo ----------------*/
void Write_p(MQTTClient* client)
{
	Write(&Batt_priority_source,client);
  Write(&Charger_allowed,client);
  Write(&Grid_Feeding_allowed,client);
  Write(&Inverter_Allowed,client);
  Write(&MAX_current_of_AC_IN,client);
  Write(&Max_Grid_Feeding_current,client);
  Write(&Battery_Charge_current_DC,client);
  Write(&Smart_boost_allowed,client);


  if(Force_floating.Value == 1)
	{
    	Write(&Force_floating,client);

    	Force_floating.Value = 0;
  }
  Write(&Floating_voltage,client);
}

/*----------------------------------------------------------------------------------------------*/

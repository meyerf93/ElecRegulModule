//Fichier : Main_test
//Projet  : ∑PS6 Pilotage-micro réseau - Solar Decathlon 2017
//But	  : Test des routine connect, deconnect read and write
//Auteurs : Capone M., Botta D, Meyer F, Couty Philippe.
//Version : 2.0
//Date 	  : 25.06.2018
//Status  : test

/*
COMMENTAIRES POUR GUILLAUME

Parametres:
1107: Limite de courant de charge AC-in
*/

#include <stdio.h>
#include <string.h>
#include <scom_property.h>
#include <stdbool.h>
#include <math.h>

#include <json.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <syslog.h>


#include "Write_read_v2.0.h"
#include "api_studer.h"
#include "connection.h"
#include "scom_data_link.h"
#include "scom_property.h"

#include <MQTTClient.h>
//#include <json-c/json.h>

#define source_addr 19216823
#define dest_addr 101
#define f_write 0x2
#define f_read 0x1
#define BOOL_F 1
#define FLOAT_F 6
#define INT32_F 5
#define LONG_ENUM 10
#define SHORT_ENUM 11
#define ENUM 3
#define true 1
#define false 0
#define int_temp_s 10
#define int_temp_us 0


#define MQTTCLIENT_PERSISTENCE 0
#define ADDRESS     "192.168.2.3:1883"
//#define ADDRESS		"128.179.181.106"
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

#include "cJSON.h"

char* address = "101";
int connection = IPV4;
int result = 0;
double meters[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0,0.0,0.0,0.0};


#define ARRAY_OF(x) (sizeof(x)/sizeof(x[0]))
#define UNUSED(x) (void)(x)

#define SPACE "\t\r\n"

static void strip(char *s,char * to_remove);

static int signal_catched = 0;

static void catch_signal (int signal)
{
	syslog (LOG_INFO, "signal=%d catched\n", signal);
	signal_catched++;
}

static void fork_process()
{
	pid_t pid = fork();
	switch (pid) {
	case  0: break; // child process has been created
	case -1: syslog (LOG_ERR, "ERROR while forking"); exit (1); break;
	default: exit(0);  // exit parent process with success
	}
}

static void strip(char *s,char * to_remove)
{
    char *p = s;
    int n;
    while (*s)
    {
        n = strcspn(s, to_remove);
        strncpy(p, s, n);
        p += n;
        s += n + strspn(s+n, to_remove);
    }
    *p = 0;
}
/*-----------------------MQTT handler --------------------------------------------------------*/
volatile MQTTClient_deliveryToken deliveredtoken;
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    UNUSED(context);
    //printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    /* EXEMPLE CODE MQTT*/
  char* payloadptr;
  cJSON * root;

  UNUSED(payloadptr);
  UNUSED(context);
  UNUSED(topicLen);


	bool I_paquet;
	int select_meters;
	 //select right mqtt packet ----------
	char payload[message->payloadlen+1];
	memcpy (payload, message->payload, message->payloadlen);
	payload[message->payloadlen] = 0;
	//printf("payload display : %s\n",payload);

  int offset = 0;
	char* dst= payload;
	do {
	    while (dst[offset] == '[') ++offset;
	    *dst = dst[offset];
	} while (*dst++);

 	offset = 0;
 	do {
 	   while (dst[offset] == ']') ++offset;
 	   *dst = dst[offset];
 	} while (*dst++);

//	printf("payload display : %s\n", payload);

	if (strstr(payload,PV_WEST) != NULL) {
		I_paquet = true;
		select_meters = 0;
	}
	else if (strstr(payload,PV_EAST) != NULL) {
		I_paquet = true;
		select_meters = 1;
	}
	else if (strstr(payload,PV_SOUTH) != NULL){
		I_paquet = true;
		select_meters = 2;
	}
	else if (strstr(payload,SECURE_LOAD)!= NULL) {
		I_paquet = true;
		select_meters = 3;
	}
	else if (strstr(payload,OVEN)!= NULL) {
		I_paquet = true;
		select_meters = 4;
	}
	else if (strstr(payload,COOKTOP) != NULL){
		I_paquet = true;
		select_meters = 5;
	}
	else if (strstr(payload,CAR_CHARGER) != NULL){
		I_paquet = true;
		select_meters = 6;
	}
	else if (strstr(payload,NORTH_RECEP)!= NULL) {
		I_paquet = true;
		select_meters = 7;
	}
	else if (strstr(payload,SOUTH_RECEP)!= NULL) {
		I_paquet = true;
		select_meters = 8;
	}
	else if (strstr(payload,SKIN_RECEP)!= NULL) {
		I_paquet = true;
		select_meters = 9;
	}
	else if (strstr(payload,BATTERY_IN)!= NULL) {
		I_paquet = true;
		select_meters = 10;
	}
	else if (strstr(payload,OPTI_KG) != NULL){
		I_paquet = true;
		select_meters = 11;
	}
	else if (strstr(payload,OPTI_PROD) != NULL){
		I_paquet = true;
		select_meters = 12;
	}
	else if (strstr(payload,OPTI_CONSO) != NULL){
		I_paquet = true;
		select_meters = 13;
	}
	else if (strstr(payload,OPTI_POWER) != NULL){
		I_paquet = true;
		select_meters = 14;
	}
	else {I_paquet = false;}
	//-------------------------------------------
	//extract data ------------------------------
	if(I_paquet){
		root = cJSON_Parse(payload);
		//printf("receive paylaod with mqtt : %s\n",payload);
		//printf("palyoad parsed : %s\n",cJSON_Print(root));

		cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
		if (cJSON_IsNumber(data))
		{
  		meters[select_meters] = data->valuedouble;
		}

		Ppv = meters[0]+meters[1]+meters[2];
		Pl = meters[3]+meters[4]+meters[5]+meters[6]+meters[7]+meters[8]+meters[9]; //+ meters[10];
		Kg = meters[11];
		Ps_opti = meters[14];

		//printf("receive data : %f,%f,%f\n",Ppv,Pl,Kg);
	}

	//parse packet for xcom
	if (strstr(payload,XCOM_ID_CHARGER) != NULL){
		root = cJSON_Parse(payload);
    //printf("receive paylaod with xcom : %s\n",payload);
    //printf("palyoad parsed : %s\n",cJSON_Print(root));

		cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
		if (strstr(payload,CHARGER_ALLOWED) != NULL){
			Charger_allowed.Value =(float)data->valueint;
		}
		else if (strstr(payload,BATTERY_CHARGE_CURRENT_DC) != NULL){
			Battery_Charge_current_DC.Value = (float)data->valuedouble;
		}
		else if (strstr(payload,FORCE_NEW_CYCLE) != NULL){
		Force_new_cycle.Value=(float)data->valueint;
		}
		else if (strstr(payload,USE_DYNAMIC_COMP) != NULL){
		Use_dynamic_comp.Value=(float)data->valueint;
		}
		else if (strstr(payload,FLOATING_VOLTAGE) != NULL){
			Floating_voltage.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,VOLTAGE_1_START_NEW_CYCLE) != NULL){
		Voltage_1_start_new_cycle.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,VOLTAGE_2_START_NEW_CYCLE) != NULL){
		Voltage_2_start_new_cycle.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,TIME_1_UNDER_VOLTAGE) != NULL){
		Time_1_under_voltage.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,TIME_2_UNDER_VOLTAGE) != NULL){
		Time_2_under_voltage.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,ABSORPTION_PHASE_ALLOWED) != NULL){
		Absorption_phase_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,ABSORPTION_VOLTAGE) != NULL){
		Absorption_voltage.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,ABSORPTION_DURATION) != NULL){
		Absorption_duration.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,END_ABSORPTION_CURRENT) != NULL){
		End_absorption_current.Value=(float)data->valueint;
		}
		else if (strstr(payload,CURRENT_TO_QUIT_ABSORPTION) != NULL){
		Current_to_quit_absorption.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,MAXIMAL_FREQ_ABSORPTION) != NULL){
		Maximal_freq_absorption.Value=(float)data->valueint;
		}
		else if (strstr(payload,DELAY_BEETWEN_ABSORPTION) != NULL){
		Delay_beetween_absorption.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,EQUALIZATION_PHASE_ALLOWED) != NULL){
		Equalization_phase_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,REDUCED_FLOATIN_ALLOWED) != NULL){
		Reduced_floating_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,PERIODIC_ABSORPTION_ALLOWED) != NULL){
		Periodic_absorption_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,INVERTER_ALLOWED) != NULL){
		Inverter_Allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,INVERTER_OUTPUT_VOLTAGE) != NULL){
		Inverter_Output_Voltage.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,INVERTER_FREQUENCY) != NULL){
		Inverter_Frequency.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,BATT_PRIORITY_SOURCE) != NULL){
		Batt_priority_source.Value=(float)data->valueint;
		}
		else if (strstr(payload,STANDBY_LEVEL) != NULL){
		Standby_level.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,STANDBY_NBR_PULSE) != NULL){
		Standby_nbr_pulse.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,TRANSFER_RELAY_ALLOED) != NULL){
		Transfer_relay_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,SMART_BOOS_ALLOWED) != NULL){
		Smart_boost_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,MAX_CURRENT_OF_AC_IN) != NULL){
		MAX_current_of_AC_IN.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,MULTI_INVERTERS_ALLOWED) != NULL){
		MAX_current_of_AC_IN.Value=(float)data->valueint;
		}
		else if (strstr(payload,INTEGRAL_MODE) != NULL){
		Integral_mode.Value=(float)data->valueint;
		}
		else if (strstr(payload,GRID_FEEDING_ALLOWED) != NULL){
		Grid_Feeding_allowed.Value=(float)data->valueint;
		}
		else if (strstr(payload,MAX_GRID_FEEDIN_CURRENT) != NULL){
		Max_Grid_Feeding_current.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,BATTERY_VOLTAGE_FORCED_FEEDING) != NULL){
		Battery_voltage_forced_feeding.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,START_TIME_FORCED_INJECTION) != NULL){
		Start_Time_forced_injection.Value=(float)data->valueint;
		}
		else if (strstr(payload,STOP_TIME_FORCED_INJECTION) != NULL){
		Stop_Time_forced_injection.Value=(float)data->valueint;
		}
		else if (strstr(payload,USE_DEFINED_PHASE_SHIFT_CURVE) != NULL){
		Use_defined_phase_shift_curve.Value=(float)data->valueint;
		}
		else if (strstr(payload,COS_PHI_P_0) != NULL){
		Cos_phi_P_0.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,COS_PHI_P_100) != NULL){
		Cos_phi_P_100.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,COS_PHI_AT_P_1613) != NULL){
		Cos_phi_at_P_1613.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,POWER_OF_SECOND_COS_PHI_POINT) != NULL){
		Power_of_second_cos_phi_point.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_BATTERY_VOLTAGE_STUDER) != NULL){
		i_Battery_Voltage_Studer.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_WANTED_BATTERY_CHARGE_CURRENT) != NULL){
		i_Wanted_battery_charge_current.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_BATTERY_CHARGE_CURRENT) != NULL){
		i_Battery_Charge_current.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_STATE_OF_CHARGE) != NULL){
		i_State_of_charge.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_BATTERY_CYCLE_PHASE) != NULL){
		i_Battery_cycle_phase.Value=(float)data->valueint;
		}
		else if (strstr(payload,I_INPUT_VOLTAGE_AC_IN) != NULL){
		i_Input_voltage_AC_IN.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_INPUT_CURRENT_AC_IN) != NULL){
		i_Input_current_AC_IN.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_INPUT_POWER_AC_IN) != NULL){
		i_Input_power_AC_IN.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_INPUT_CURRENT_LIMIT) != NULL){
		i_Input_current_limit.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_INPUT_CURRENT_LIMIT_REACHED) != NULL){
		i_Input_current_limit_reached.Value=(float)data->valueint;
		}
		else if (strstr(payload,I_BOOST_ACTIVE) != NULL){
		i_Boost_active.Value=(float)data->valueint;
		}
		else if (strstr(payload,I_OUTPUT_VOLTAGE_AC_OUT) != NULL){
		i_Output_voltage_AC_OUT.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_OUTPUT_CURRENT_AC_OUT) != NULL){
		i_Output_current_AC_OUT.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_OUTPUT_POWER_AC_OUT) != NULL){
		i_Output_power_AC_OUT.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_OPERATING_STATE) != NULL){
		i_Operating_state.Value=(float)data->valueint;
		}
		else if (strstr(payload,I_STATE_OF_INVERTER) != NULL){
		i_State_of_inverter.Value=(float)data->valueint;
		}
		else if (strstr(payload,I_DISCHARGE_BATTERY_POWER_DAY_AHEAD) != NULL){
		i_Discharge_Battery_Power_Day_ahead.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_DISCHARGE_BATTERY_POWER_TODAY) != NULL){
		i_Dishcarge_Battery_Power_today.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_ENERGY_AC_IN_DAY_A_HEAD) != NULL){
		i_Energy_AC_IN_Day_ahead.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_ENERGY_AC_IN_TODAY) != NULL){
		i_Energy_AC_IN_today.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_ENERGY_AC_OUT_DAY_A_HEAD) != NULL){
		i_Energy_AC_OUT_Day_ahead.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_ENERGY_AC_OUT_TODAY) != NULL){
		i_Energy_AC_OUT_today.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_AC_IN_FREQUENCY) != NULL){
		i_AC_IN_frequency.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_AC_OUT_FREQUENCY) != NULL){
		i_AC_OUT_frequency.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_INJECTION_CURRENT_AC_DESIRED) != NULL){
		i_Injection_current_AC_desired.Value=(float)data->valuedouble;
		}
		else if (strstr(payload,I_BATTERY_PRIORITY_ACTIVE) != NULL){
		i_Battery_priority_active.Value=(float)data->valueint;
		}
		else if (strstr(payload,I_BATTERY_PRIORITY_ACTIVE) != NULL){
		i_Forced_grid_feeding_active.Value=(float)data->valueint;
		}
		else if (strstr(payload,BAT_TEMP_COMP_COEF) != NULL){
			Bat_Temp_Com_Coef.Value=(float) data->valueint;
		}
	}
	else if (strstr(payload,XCOM_ID_BAT) != NULL){
			root = cJSON_Parse(payload);
			//printf("receive paylaod with xcom bat  : %s\n",payload);
			//printf("palyoad parsed : %s\n",cJSON_Print(root));

			cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
			if(strstr(payload,SOC_BACKUP) != NULL){
				Soc_Backup.Value=(float) data->valueint;
			}
			else if (strstr(payload,SOC_INJECT)!= NULL){
				Soc_Inject.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_VOLTAVGE)!= NULL){
				i_Battery_Voltage.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERYY_CURRENT)!= NULL){
				i_Battery_Current.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_SOC_VALUE_BATTERY)!= NULL){
				i_soc_value_battery.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_STATE_OF_HEALTH)!= NULL){
				i_State_of_Health.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_VOLTAGE_CHARGE_LIMIT)!= NULL){
				i_Battery_Voltage_Charge_limit.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_VOLTAGE_DISCHARGE_LIMIT)!= NULL){
				i_Battery_Voltage_Discharge_limit.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_CURRENT_CHARGE_LIMIT)!= NULL){
				i_Battery_Current_Charge_limit.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_CURRENT_DISCHARGE_LIMIT)!= NULL){
				i_Battery_Current_Discharge_limit.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_CURRENT_CHARGE_RECOMMANDED)!= NULL){
				i_Battery_Current_Charge_recommanded.Value=(float) data->valueint;
			}
			else if (strstr(payload,I_BATTERY_CURRENT_DISCHARGE_RECOMMANDED)!= NULL){
				i_Battery_Current_Discharge_recommanded.Value=(float) data->valueint;
			}
	}
	 MQTTClient_freeMessage(&message);
	 MQTTClient_free(topicName);
	return 1;
}

void connlost(void *context, char *cause)
{
    UNUSED(context);
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

// MQTTT communication with studer
void send_json_obj(MQTTClient client,char topic[64], char data[64], char mdl[64], char id[64])
{
    	//printf("value receive in send_json : %s,%s,%s,%s\n",topic,data,mdl,id);
  cJSON * root;
	root = cJSON_CreateObject();
  cJSON_AddStringToObject(root,"mdl",mdl);
  cJSON_AddStringToObject(root,"cmd",id);
  cJSON_AddStringToObject(root,"value",data);
	char* payload_json = cJSON_Print(root);
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

	//printf("json message send : %s\n",pubmsg.payload);
  MQTTClient_publishMessage(client, topic, &pubmsg, &token);
}


/*---------------------- Routine de connection à la cible via IPV4 ----------------------------- */
int Connection(const char* address,struct connection* socket_struct)
     	{
	if(connection == IPV4) result = initialize_socket_ipv4(address,socket_struct);
		if(result == -1) return result;
		return 1;
     	}
/*----------------------------------------------------------------------------------------------*/


/*---------------------- Routine de connection à la cible via IPV4 -----------------------------*/
void Deconnection(struct connection* socket_struct)
     {
      if(connection == IPV4) close_socket_ipv4(socket_struct);
     }
/*----------------------------------------------------------------------------------------------*/


/*---------------------- Routine de lecture d'un parametre sur l'Xtender -----------------------*/
int Read(t_param* Parametre,MQTTClient* client)
{
  char data[64];
  sprintf(data,"[%d,%d,%d,%d,%d]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_READ);
  //printf("data : %s\n",data);
	send_json_obj(client,TOPIC_WRITE,data,XCOM,XCOM_ID_CHARGER);
	return 1;
}
/*----------------------------------------------------------------------------------------------*/
/*---------------------- Routine de lecture d'un parametre sur l'Xtender -----------------------*/
int Read_bat(t_param* Parametre,MQTTClient* client)
{
  char data[64];
  sprintf(data,"[%d,%d,%d,%d,%d]",Parametre->Object_type,Parametre->User_ref,Parametre->Proprety_id,Parametre->Format,XCOM_READ);
  //printf("data : %s\n",data);
	send_json_obj(client,TOPIC_WRITE,data,XCOM,XCOM_ID_BAT);
	return 1;
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
	send_json_obj(client,TOPIC_WRITE,data,XCOM,XCOM_ID_CHARGER);
	return 1;
}
/*----------------------------------------------------------------------------------------------*/
/*---------------------- Routine d'ecriture d'un parametre sur l'Xtender -----------------------*/
int Write_bat(t_param* Parametre,MQTTClient* client)
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
	send_json_obj(client,TOPIC_WRITE,data,XCOM,XCOM_ID_BAT);
	return 1;
}
/*----------------------------------------------------------------------------------------------*/


/*------------- Routine d'initialization de touts les parametres et de l'Xtender ---------------*/
int Init(MQTTClient* client){
	Write(&Parameters_saved_in_flash_memory,client);
	Write(&Over_Voltage_Value_allowed,client);
	Write(&Voltage_After_Over_allowed,client);
	Write(&Under_Voltage_Value_allowed,client);
	Write(&Voltage_After_Under_allowed,client);
	Write(&Charger_allowed,client);
	Write(&Battery_Charge_current_DC,client);
	Write(&Use_dynamic_comp,client);
	Write(&Floating_voltage,client);
	Write(&Voltage_1_start_new_cycle,client);
	Write(&Voltage_2_start_new_cycle,client);
	Write(&Time_1_under_voltage,client);
	Write(&Time_2_under_voltage,client);
	Write(&Absorption_phase_allowed,client);
	Write(&Absorption_voltage,client);
	Write(&Absorption_duration,client);
	Write(&End_absorption_current,client);
	Write(&Current_to_quit_absorption,client);
	Write(&Maximal_freq_absorption,client);
	Write(&Delay_beetween_absorption,client);
	Write(&Equalization_phase_allowed,client);
	Write(&Reduced_floating_allowed,client);
	Write(&Periodic_absorption_allowed,client);
	Write(&Inverter_Allowed,client);
	Write(&Inverter_Output_Voltage,client);
	Write(&Inverter_Frequency,client);
	Write(&Batt_priority_source,client);
	Write(&Transfer_relay_allowed,client);
	Write(&Smart_boost_allowed,client);
	Write(&MAX_current_of_AC_IN,client);
	Write(&Multi_inverters_allowed,client);
	Write(&Integral_mode,client);
	Write(&Grid_Feeding_allowed,client);
	Write(&Battery_voltage_forced_feeding,client);
	Write(&Use_defined_phase_shift_curve,client);
	Write(&Maximal_freq_absorption,client);
	Write(&Delay_beetween_absorption,client);
	Write(&Equalization_phase_allowed,client);
	Write(&Reduced_floating_allowed,client);
	Write(&Periodic_absorption_allowed,client);
	Write(&Inverter_Allowed,client);
	return 1;
}
/*----------------------------------------------------------------------------------------------*/


/*-------------- Routine de la gestion de la batterie, charger, decharger, injecter ------------*/
// PCO : hypothèse  la batterie va recevoir une instruction de charger ou décharger à la puissance ps

// GESTION ACTIVE CONTROL, LA BATTERIE EST MASTER
// ON DOIT JOUER POUR LES SOC,
// Pour le pas de temps considéré correspondant au forecast , cela correspond à un nombre de kWh souhaité (consigne) sur une durée de 30'
// Initialement avec le soft opti , on recevait une valeur Ps <0 ou Ps>0
// kWh_disponible = 7057 state of heath% *10.8) *(soc_courant - SOCmin) La capacité intilale etant de 10.8 kWh
// SOC_final = SOC_courant + kWh_consigne/kWh_disponible
// delta_SOC=SOC_final-SOC_courant= kWh_consigne/kWh_disponible
// Le delta_SOC peut être calculé dans l'algorithme
// sa valeur max est SOCmax (inférieur au 100% de charge disponible)
// sa valeur min est SOCmin (supérieur) au 100% de décharge disponible)

// la gestion de la batterie fait la distinction Ps>plsec ou Ps<plsec
// plsec est mesuré de manière instatanée dans calcul_p_k Plsec = i_Output_power_AC_OUT.Value*1000;
// psmax_decharge = courant_decharge_limite IAdc (param 7068) x tension_dc (param 7000)
// ps = kWh/ temps_en_heure

// Question est-ce que smartboost toujours autorisé? aujourd'hui c'est initialisé TRUE dans le fichier Write_read.h

void Battery_management(float P_s,MQTTClient* client)
{
	// Unité de P_s en Watt
	//float Forcage_KWh_Charge_Decharge;
	//float Delta_SOC;
	//float Final_SOC;

	//Forcage_KWh_Charge_Decharge=P_s/1000*0.5 ; //Unité kWh 0.5 pour 30 min corriger avec le pas de temps de la boucle du programme e
	//MAX_current_of_AC_IN.Value=35.0; // 1107 deja dans le writeread.h

	//Read_bat(&i_soc_value_battery,client); // Param 7002
	//Read_bat(&i_State_of_Health, client); // Param 7057
	//Bat_Capacite_disponible = i_State_of_Health.Value* Bat_Capacite_nominale*(i_soc_value_battery.Value - SOCmin);
	//printf("Value for bat cap : %f, state of healt : %f, bat cap nomi : %f, soc value : %f, socmin %f\n",
	//Bat_Capacite_disponible, i_State_of_Health.Value, Bat_Capacite_nominale, i_soc_value_battery.Value, SOCmin);
	//Delta_SOC=Forcage_KWh_Charge_Decharge/Bat_Capacite_disponible*100;
	//printf("Delta soc : %f, Forcage kwh : %f,Bap cat dispo : %f\n",Delta_SOC, Forcage_KWh_Charge_Decharge, Bat_Capacite_disponible);
	//Final_SOC= i_soc_value_battery.Value + Delta_SOC;
	//printf("Final soc : %f, I soc value : %f, Delta soc : %f\n",Final_SOC,i_soc_value_battery.Value, Delta_SOC);

	//Calcul for next SOC
	// Bk et inj identique car PV coté AC _in en AC coupling et pas du coté batterie
	//Soc_Backup.Value = Final_SOC;
	//Soc_Inject.Value = Final_SOC;

	// ON CHARGE Ps>=0
	//if (Delta_SOC >= 0)
	if(Ps >= 0)
	{
		printf("CHARGE DE LA BATTERIE\n");
		printf("========== Battery en charge ==========\n");
		INJ=0;
		//Bloquer l'injection;
		Grid_Feeding_allowed.Value = true; //Param 1127;
		//Activer l'onduleur;
		Inverter_Allowed.Value = false; //Param 1124;
		//Activation du SmartBoost;
		Smart_boost_allowed.Value = true; //Param 1126;
		//Autoriser la charge;
		Charger_allowed.Value = true; //Param 1125;
		//tran transfert allowed
		Transfer_relay_allowed.Value = 1; //Param 1128
		//force un nouveau cycle
		Force_new_cycle.Value = 1;

		//MODE INSPECT CONTROL : Calcul de la courant max de charge;
		Battery_Charge_current_DC.Value = fabs(P_s) / i_Battery_Voltage_Studer.Value;
		printf("Battery current charge limit : %f\n",i_Battery_Current_Charge_limit.Value);
    if(Battery_Charge_current_DC.Value >= i_Battery_Current_Charge_limit.Value) Battery_Charge_current_DC.Value = i_Battery_Current_Charge_limit.Value;
		if(Battery_Charge_current_DC.Value >= 55) Battery_Charge_current_DC.Value = 55;
		printf("Battery charge current DC = %f\n", Battery_Charge_current_DC.Value);

		MAX_current_of_AC_IN.Value = 8000 / i_Input_voltage_AC_IN.Value;
		printf("Max current of ac in : %f\n", MAX_current_of_AC_IN.Value);
		if(MAX_current_of_AC_IN.Value >= 34.0) MAX_current_of_AC_IN.Value = 34.0; // 8.6 pour 2 kw
	}
  else // ON DECHARGE
	{
    	//Puissance des batteries suffisante pour alimenter le CS ?
    	charge_on = 0;
			Batt_priority_source.Value = false;
			if (fabs(P_s) > Plsec)
			{
      	printf("========== Injceter sur le reseaux ==========\n");
				printf("DECHARGE DE LA BATTERIE\n");
				//Bloquer la charge;
				Charger_allowed.Value = false;//Param 1125;
				//Activer l'onduleur;
				Inverter_Allowed.Value = true; //Param 1124;
				//Autoriser l'injection;
				Grid_Feeding_allowed.Value = true; //Param 1127;
				//Activation du SmartBoost;
				Smart_boost_allowed.Value = true; //Param 1126;
				//tran transfert allowed
				Transfer_relay_allowed.Value = 1; //Param 1128
				//force un nouveau cycle
				Force_new_cycle.Value = 1;


				//Régulation du ratio de puissance Pbatt vs Pres via Iac AC-IN;
				Max_Grid_Feeding_current.Value = fabs(Ps) / i_Input_voltage_AC_IN.Value;
				if(Max_Grid_Feeding_current.Value >= (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value)
			 	Max_Grid_Feeding_current.Value = (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value;													//value dynamic for discharge
				Printf("Max grid feeding : %f\n",Max_Grid_Feeding_current.Value);
				Pruntf("Battery current discharge limi : %f\n",i_Battery_Current_Discharge_limit.Value);
				if(Max_Grid_Feeding_current.Value >= 34.0) Max_Grid_Feeding_current.Value = 34.0; // 8.6 pour 2 kW

				//Temps d'injection;
				Start_Time_forced_injection.Value = Time_now+1; //L?injection débuterai dans 1 minute
				Stop_Time_forced_injection.Value = Start_Time_forced_injection.Value +1+INJ;
				//L'injection s'arrêtera après le nouveau cycle;
				INJ++;
			}
			else
			{
				printf("ALIMENTATION DES CHARGES SECURISEES");
				INJ=0;
	     	//printf("========== Puissance insuff pour injecter, alim le charge securisé ==========\n");
				//Bloquer la charge
				Charger_allowed.Value = false; //Param 1125;
				//Bloquer l'injection;
				Grid_Feeding_allowed.Value = false; //Param 1127;
				//Activer l'onduleur;
				Inverter_Allowed.Value = true; //Param 1124;
				//Activation du SmartBoost;
				Smart_boost_allowed.Value = true; //Param 1126;
				//Utilisation de la batterie comme source prioritaire;
				Batt_priority_source.Value = true; //Param 1296;

				// PCO a priori pas de sécu à mettre ici pour batterie
	    	MAX_current_of_AC_IN.Value = (Plsec-fabs(Ps))/i_Input_voltage_AC_IN.Value;
	    	if (MAX_current_of_AC_IN.Value >= 34.0) MAX_current_of_AC_IN.Value=34.0;
				Force_floating.Value = 1.0;
		}
	}
	/*if(Soc_Backup.Value > SOCmax) {
		Soc_Backup.Value = SOCmax;
		Soc_Inject.Value = SOCmax;
	}
	else if (Soc_Backup.Value < SOCmin) {
		Soc_Backup.Value = SOCmin;
		Soc_Inject.Value = SOCmin;
	}
	Write_bat(&Soc_Backup,client);
	Write_bat(&Soc_Inject,client);*/
}
/*----------------------------------------------------------------------------------------------*/


/*---------------------------- Routine de Calculs des coefficients ----------------------------*/
void Calculs_p_k(void)
{
	if (Time_now ==1440) Time_now = 0;
	//Ppv = 10000;       //Puissance des PV
	//Pl = 9000;        //Puissance des charges
	Epv = Ppv;//Ppv_1(int)Time_now]/60;       //Energie PV
	El = Pl;//Pl_1[(int)Time_now]/60;       //Energie des chatges
	Eb = Epv - El; //Energie bilan
	Ppv = Ppv;//Ppv_1[(int)Time_now];
	Pl = Pl;//Pl_1[(int)Time_now];
	Pb = (Ppv - Pl);  //Puissance bilan

	SOC = i_soc_value_battery.Value;
	printf("Soc value : %f\n",SOC);
	//SOC = 90;

	//Plsec = 500;     //Puissance des charges securisées
	Plsec = i_Output_power_AC_OUT.Value*1000;

	//Ps = Ps_opti*1000;        //Puissance demandée au stockage

	Ps = 0;
	Pr = 0;        //Puissance demandé au réseau
	//Ps = 0;

	Psmax = 2800;
	// PCO
  // psmax = courant_decharge_limite IAdc (param 7064) x tension_dc (param 7000)
  // a voir s'il faut pas remplacer par des valeur moyenne de 1 min, autre param disponibles
	//Puissance max de domande au stockage 55A x 50V, mais ne marche pas en mode Xcan active control puisqu'on sera au courant max recommandé

	// pas max decharge pas utilisé ci dessous
	// tester l'algo ...
	Psmax_discharge = i_Battery_Current_Discharge_limit.Value * i_Battery_Voltage_Discharge_limit.Value;
	Psmax_charge = i_Battery_Current_Charge_limit.Value * i_Battery_Voltage_Charge_limit.Value;


	Prmax = 8000; //Puissance max de demande au réseau ou du réseau slon donnée blue factory / Groupe e

 	Temps_routine = 2;
 	Time_now_i = Time_now / 60;

	SOCmin = 10;
	SOCmax = 95;
}
/*---------------------------------------------------------------------------------------------*/


/*----------------------- Algorithme d'optimisation energetique -------------------------------*/
void Algo(MQTTClient* client)
{
	Calculs_p_k();
	// PCO : a défaut d'une valeur Ps (stockage) calculée dans un algorithme d'optimisation, Ps =Pb (balance)
	Ps=Pb;
	printf("ps : %f, pb : %f\n",Ps,Pb);
	Battery_management(Ps,client);
//9:19;SOC = 43.000000;Eb = 449.309814;Pr = 0.000000;Ps = 449.309814;Pl = 5087.889648;Ppv = 3189.609863;Pb = 449.309814;Plsec = 2640.625000;Ibat = -11.000000;Ubat = 54.062500;Iac_in = 8.250000;Uac = 239.000000;
	// On produit?
	if (Eb >= 0)
	{
		//la batterie est elle presque pleine ?
		if (SOC >= 0.97*SOCmax)
		{
			//la batterie est trop pleine ?
			if (SOC >= SOCmax)
			{
      	printf("STATE = 4;");
      	STATE = 4;
				Ps = 0;
				Pr = Pb;
			}
			else
			{
				printf("STATE = 5;");
				STATE = 5;
				Ps =  Ps;
			}

		}
		else
		{	//la batterie n'est pas trop pas encore pleine
      printf("STATE = 3;");
      STATE = 3;
			Ps = Pb;
			Pr = 0;
		}
		Battery_management(Ps,client);
		// la puissance max demander au réseau est elle trop grande ?
		if (fabs(Pr) >= Prmax)
		{
			Pr = Prmax;
		}
	}
	else
	{ // non on ne produit pas
		// la batterie est elle presque vide ?
		if (1.03*SOCmin >= SOC)
		{
			// la batterie est elle vraiment vide ?
			if (SOCmin >= SOC)
			{
      	STATE = 2;
      	printf("STATE = 2;");
      	Ps = Psmax_charge;
				Pr = Pb;
			}
			else
			{
				Ps = Ps;
			}
		}
		else
		{
    	STATE = 1;
    	printf("STATE = 1;");
			Ps = Pb;
			Pr = 0;
			//Kr = Ppv / Pl;
		}
		//Ps = Ps * 3;
		Battery_management(Ps,client);
		if (fabs(Pr) >= Prmax)
		{
			//Kr = Prmax / Pr;
			Pr = - Prmax;
		}
	}
}
/*----------------------------------------------------------------------------------------------*/


/*--------------- Routine Ecriture, ecriture des parametres modifie dans l'algo ----------------*/
void Write_p(MQTTClient* client)/*scom_frame_t* frame,scom_property_t* property ,char* buffer,size_t buffer_length, struct connection* socket_struct, struct studer_data* data,char* ret_val)*/
{
	Write(&Batt_priority_source,client);
  Write(&Charger_allowed,client);
  Write(&Grid_Feeding_allowed,client);
  Write(&Inverter_Allowed,client);
  Write(&MAX_current_of_AC_IN,client);
  Write(&Max_Grid_Feeding_current,client);
  Write(&Battery_Charge_current_DC,client);
  Write(&Smart_boost_allowed,client);

 	if(INJ==1)
	{
 		Write(&Start_Time_forced_injection,client);
  }
  	Write(&Stop_Time_forced_injection,client);

  if(Force_floating.Value == 1)
	{
    	Write(&Force_floating,client);

    	Force_floating.Value = 0;
  }

 	if(Force_new_cycle.Value == 1)
	{
		Write(&Force_new_cycle,client);
		Force_new_cycle.Value = 0;
	}
}
/*----------------------------------------------------------------------------------------------*/


/*-------------------- Routine de lectures des parametres necessaires pour l'algo -------------*/
void Read_p(MQTTClient *client,MQTTClient *client_bat)/*scom_frame_t* frame,scom_property_t* property ,char* buffer,size_t buffer_length, struct connection* socket_struct, struct studer_data* data,char* ret_val)MQTTClient *client)*/
{
 	Read(&i_Battery_Voltage_Studer,client);
 	Read(&i_Input_voltage_AC_IN,client);
 	Read(&i_State_of_charge,client);
 	Read(&i_Battery_Charge_current,client);
 	Read(&i_Output_voltage_AC_OUT,client);
 	Read(&i_Output_power_AC_OUT,client);
 	Read_bat(&i_Battery_Voltage,client_bat);
	Read_bat(&i_Battery_Current,client_bat);
	Read_bat(&i_soc_value_battery,client_bat);
	Read_bat(&i_State_of_Health,client_bat);
	Read_bat(&i_Battery_Voltage_Charge_limit,client_bat);
	Read_bat(&i_Battery_Voltage_Discharge_limit,client_bat);
	Read_bat(&i_Battery_Current_Charge_limit,client_bat);
	Read_bat(&i_Battery_Current_Discharge_limit,client_bat);
	Read_bat(&i_Battery_Current_Charge_recommanded,client_bat);
	Read_bat(&i_Battery_Current_Discharge_recommanded,client_bat);
}
/*----------------------------------------------------------------------------------------------*/


void get_Time(void)
{
 time(&my_time);
 timeinfo = localtime(&my_time);
 Time_now = timeinfo->tm_hour*60 + timeinfo->tm_min;
}


void catch_alarm (int sig)
{
 	static int i = 0;
 	//printf("I'm running %d\n", i);
 	//printf("**%d\n", i);
 	printf(" ");
 	algo_on = 1;
 	i++;
 	if(i == 6) i = 0;
 		get_Time();
   	printf("%d:%d;", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
		//printf(" Soc_Backup, = %f;", Soc_Backup);
		//printf(" Soc_Inject, = %f;", Soc_Inject);
   	printf("SOC = %f;", SOC);
		printf("Eb = %f;", Eb);
		printf("Pr = %f;", Pr);
   	//printf("Kg = %f;", Kg);
   	printf("Ps = %f;", Ps);
   	printf("Pl = %f;", Pl);
   	printf("Ppv = %f;", Ppv);
   	printf("Pb = %f;", Pb);
   	printf("Plsec = %f;", Plsec);
   	printf("Ibat = %f;", i_Battery_Charge_current.Value);
   	printf("Ubat = %f;",i_Battery_Voltage_Studer.Value);
   	printf("Iac_in = %f;", i_Input_current_AC_IN.Value);
   	printf("Uac = %f;",i_Input_voltage_AC_IN.Value);
		printf("Max_grid_geed = %f;\n",Max_Grid_Feeding_current.Value);

   	FILE * fd = 0;
   	fd = fopen("read_fast.txt", "a");
   	fprintf(fd,"STATE = %d;",STATE);
   	fprintf(fd,"%d:%d;", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
		//fprintf(fd," Soc_Backup, = %f;", Soc_Backup);
		//fprintf(fd," Soc_Inject, = %f;", Soc_Inject);
   	fprintf(fd,"SOC = %f;", SOC);
		fprintf(fd,"Eb = %f;",Eb);
		fprintf(fd,"Pr = %f;",Pr);
   	//fprintf(fd," Kg = %f;", Kg);
   	fprintf(fd,"Ps = %f;", Ps);
   	fprintf(fd,"Pl = %f;", Pl);
   	fprintf(fd,"Ppv = %f;", Ppv);
   	fprintf(fd,"Pb = %f;", Pb);
   	fprintf(fd,"Plsec = %f;", Plsec);
   	fprintf(fd,"Ibat = %f;", i_Battery_Charge_current.Value);
   	fprintf(fd,"Ubat = %f;",i_Battery_Voltage_Studer.Value);
   	fprintf(fd,"Iac_in = %f;", i_Input_current_AC_IN.Value);
   	fprintf(fd,"Uac = %f;",i_Input_voltage_AC_IN.Value);
		fprintf(fd,"Max_grid_geed = %f;\n",Max_Grid_Feeding_current.Value);
  	fclose(fd);

  	//printf("time now %d:%d\n", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
  	//printf("time old %d:%d\n", (int)(Time_old/60), (int)(Time_old - (int)(Time_old/60)*60));
 	if(!(Time_old == Time_now))
  	{
   		Time_old = Time_now;
   		algo_on = 1 ;
  	}
 	signal (sig, catch_alarm);
}


void timer_init(void)
{
 	struct itimerval old;
 	struct itimerval new;
 	signal (SIGALRM, catch_alarm);
 	new.it_interval.tv_sec = int_temp_s;
 	new.it_interval.tv_usec = int_temp_us;
 	new.it_value.tv_sec = int_temp_s;
 	new.it_value.tv_usec = int_temp_us;
 	old.it_interval.tv_sec = 0;
 	old.it_interval.tv_usec = 0;
	old.it_value.tv_sec = 0;
 	old.it_value.tv_usec = 0;
 	if (setitimer (ITIMER_REAL, &new, &old) < 0) printf("timer init failed\n");
 	else printf("timer init succeeded\n");
}


void Time_init(void)
{
	time(&my_time);
	timeinfo = localtime(&my_time);
	Time_now = timeinfo->tm_hour*60 + timeinfo->tm_min;
	Time_old = Time_now;
}


int main()
{/*
	// 1. fork off the parent process
	fork_process();

	// 2. create new session
	if (setsid() == -1) {
		syslog (LOG_ERR, "ERROR while creating new session");
		exit (1);
	}

	// 3. fork again to get rid of session leading process
	fork_process();

	// 4. capture all required signals
	struct sigaction act = {.sa_handler = catch_signal,};
	sigaction (SIGHUP,  &act, NULL);  //  1 - hangup
	sigaction (SIGINT,  &act, NULL);  //  2 - terminal interrupt
	sigaction (SIGQUIT, &act, NULL);  //  3 - terminal quit
	sigaction (SIGABRT, &act, NULL);  //  6 - abort
	sigaction (SIGTERM, &act, NULL);  // 15 - termination
	sigaction (SIGTSTP, &act, NULL);  // 19 - terminal stop signal

	// 5. update file mode creation mask
	umask(0027);

	// 6. change working directory to appropriate place
	//if (chdir ("/") == -1) {
	//	syslog (LOG_ERR, "ERROR while changing to working directory");
	//	exit (1);
	//}

	// 7. close all open file descriptors
	for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
		close (fd);
	}

	// 8. redirect stdin, stdout and stderr to /dev/null
	if (open ("/dev/null", O_RDWR) != STDIN_FILENO) {
		syslog (LOG_ERR, "ERROR while opening '/dev/null' for stdin");
		exit (1);
	}
	if (dup2 (STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
		syslog (LOG_ERR, "ERROR while opening '/dev/null' for stdout");
		exit (1);
	}
	if (dup2 (STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
		syslog (LOG_ERR, "ERROR while opening '/dev/null' for stderr");
		exit (1);
	}*/
  	MQTTClient  client_charger;
  	MQTTClient_connectOptions conn_opts_charger = MQTTClient_connectOptions_initializer;

		MQTTClient client_bat;
  	MQTTClient_connectOptions conn_opts_bat = MQTTClient_connectOptions_initializer;

  	int rc_charger;
		//crée le client mqtt pour le charger inverter --------------------------------------------
  	MQTTClient_create(&client_charger, ADDRESS, CLIENTID_CHARGER
,
    MQTTCLIENT_PERSISTENCE, NULL);
  	conn_opts_charger.keepAliveInterval = 20;
  	conn_opts_charger.cleansession = 1;
  	MQTTClient_setCallbacks(client_charger, NULL, connlost, msgarrvd, delivered);

  	if ((rc_charger = MQTTClient_connect(client_charger, &conn_opts_charger)) != MQTTCLIENT_SUCCESS)
  	{
      	printf("Failed to connect, return code %d\n", rc_charger);
      	exit(EXIT_FAILURE);
  	}
  	printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
		"Press Q<Enter> to quit\n\n", TOPIC, CLIENTID_CHARGER, QOS);

  	MQTTClient_subscribe(client_charger, TOPIC, QOS);

		//--------------------------------------------------------------------------------------

		//Crée le client mqtt pour la batterie -------------------------------------------------
		int rc_bat;
		MQTTClient_create(&client_bat, ADDRESS, CLIENTID_BAT,
		MQTTCLIENT_PERSISTENCE, NULL);
		conn_opts_bat.keepAliveInterval = 20;
		conn_opts_bat.cleansession = 1;
		MQTTClient_setCallbacks(client_bat, NULL, connlost, msgarrvd, delivered);

		if ((rc_bat = MQTTClient_connect(client_bat, &conn_opts_bat)) != MQTTCLIENT_SUCCESS)
		{
				printf("Failed to connect, return code %d\n", rc_bat);
				exit(EXIT_FAILURE);
		}
		printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
		"Press Q<Enter> to quit\n\n", TOPIC, CLIENTID_BAT, QOS);

		MQTTClient_subscribe(client_bat, TOPIC, QOS);

		//Crée le client mqtt pour la batterie ---------------------------------------------------

  	timer_init();
  	Time_init();


  	printf("--------------------------------EXEC----------------------------------\n");
		printf("\n========== launch connection ==========\n");

  	printf("Hey !\n");
		printf("\n========== Launch Initialisation ==========\n");
		Init(client_charger);
  	do
  	{
		//printf("IN MAIN LOOP\n");
   		if(algo_on == 1 )
   		{
			algo_on = 0;

			//printf(" ********************** %d : %d **********************\n", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
			//printf("========== Lecture des    printf("Ibat = %f;", i_Battery_Charge_current.Value);informations Onduleur ==========\n");
			printf("\n===========read =============\n");
			Read_p(client_charger,client_bat);//&frame, &property,buffer,sizeof(buffer), &ipv4_struct,&data,ret_val);

			printf("\n========== Algorithme =========\n");
			Algo(client_bat);
			printf("\n========== Ecriture des parametres sur Onduleur ==========\n");

	   	Write_p(client_charger);//&frame, &property,buffer,sizeof(buffer), &ipv4_struct,&data,ret_val);

	 		// Commented for test json
	 		Read(&i_Battery_cycle_phase,client_charger);
	 		Read(&i_Battery_Charge_current,client_charger);
	 		Read(&i_Input_current_AC_IN,client_charger);

			//display value in terminal

			printf("%d:%d;", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
			printf(" SOC = %f;", SOC);
			printf("Ps = %f;", Ps);
			printf("Pl = %f;", Pl);
			printf("Ppv = %f;", Ppv);
			printf("Pb = %f;", Pb);
			printf("Plsec = %f;", Plsec);
			printf("Ibat = %f;", i_Battery_Charge_current.Value);
			printf("Ubat = %f;",i_Battery_Voltage_Studer.Value);
			printf("Iac_in = %f;", i_Input_current_AC_IN.Value);
			printf("Uac = %f;",i_Input_voltage_AC_IN.Value);
			printf("Kg = %f;\n", Kg);

			printf("\n========== Write data of algorithm ==========\n");
			//print data in file for data analysis
			FILE * fd = 0;
			fd = fopen("donnes.txt", "w");
			fprintf(fd,"STATE = %d;",STATE);
			fprintf(fd,"%d:%d;", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
			fprintf(fd," SOC = %f;", SOC);
			fprintf(fd," Kg = %f;", Kg);
			fprintf(fd,"Ps = %f;", Ps);
			fprintf(fd,"Pl = %f;", Pl);
			fprintf(fd,"Ppv = %f;", Ppv);
			fprintf(fd,"Pb = %f;", Pb);
			fprintf(fd,"Plsec = %f;", Plsec);
			fprintf(fd,"Ibat = %f;", i_Battery_Charge_current.Value);
			fprintf(fd,"Ubat = %f;",i_Battery_Voltage_Studer.Value);
			fprintf(fd,"Iac_in = %f;", i_Input_current_AC_IN.Value);
			fprintf(fd,"Uac = %f;\n",i_Input_voltage_AC_IN.Value);
			fclose(fd);
		}
	} while(1);
	MQTTClient_disconnect(client_charger, 10000);
	MQTTClient_destroy(&client_charger);
	MQTTClient_disconnect(client_bat, 10000);
	MQTTClient_destroy(&client_bat);
	return rc_charger;
}

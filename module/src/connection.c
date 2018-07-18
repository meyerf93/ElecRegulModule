//Fichier : connection.c
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
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

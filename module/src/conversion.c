//Fichier : coversion.c:
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
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

void strip(char *s,char * to_remove)
{
    char *p = s;
    int n;
    while (*s)
    {
        printf(" in loop of strip");
        n = strcspn(s, to_remove);
        strncpy(p, s, n);
        p += n;
        s += n + strspn(s+n, to_remove);
    }
    *p = 0;
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

  /*char temp_playload[100] ;
  if (!cJSON_PrintPreallocated(root,*temp_playload, 100, true)){
    printf("error when parsin file");
  }*/
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

void parse_studer_message(char* payload,cJSON* data) {
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
  else if (strstr(payload,MAX_GRID_FEEDING_CURRENT) != NULL){
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
  else if (strstr(payload,FAST_CHARGE_INJECT_REGULATION) != NULL){
    Fast_charge_inject_regulation.Value=(float) data->valueint;
  }
  else if (strstr(payload,PULSE_CUTING_REGULATION_FOR_XT) != NULL) {
    Pulses_cutting_regulation_for_XT.Value = (float) data->valueint;
  }
}

void parse_batt_message(char* payload,cJSON* data){
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

#ifndef INCLUDE_WRITE_READ_V1_H_
#define INCLUDE_WRITE_READ_V1_H_

#include <MQTTClient.h>
#include "type.h"

//Fichier : Write_read_v1.h
//Projet  : PS6 Pilotage-micro réseau - Solar Decathlon 2017
//But	  : Haader file for write, read, connect and deconnect
//Auteurs : Capone M., Botta D.
//Version : 1.0
//Date 	  : 21.03.2017
//Status  : test

#define ARRAY_OF(x) (sizeof(x)/sizeof(x[0]))
#define UNUSED(x) (void)(x)

#define Not_Value -10

#define PV_WEST 		"knx1/:1.1.8/:/power.1"
#define PV_EAST 		"knx1/:1.1.10/:/power.1"
#define PV_SOUTH 		"knx1/:1.1.9/:/power.1"

#define SECURE_LOAD "knx1/:1.1.18/:/power.1"

#define OVEN 				"knx1/:1.1.11/:/power.1"
#define COOKTOP			"knx1/:1.1.12/:/power.1"
#define BATTERY_IN 	"knx1/:1.1.17/:/power.1"
#define CAR_CHARGER "knx1/:1.1.13/:/power.1"
#define NORTH_RECEP "knx1/:1.1.14/:/power.1"
#define SOUTH_RECEP "knx1/:1.1.15/:/power.1"
#define SKIN_RECEP	"knx1/:1.1.16/:/power.1"
/*#define PV_WEST 	"NH/Skin/Production/West/Power/Res"
#define PV_EAST 	"NH/Skin/Production/South/Power/Res"
#define PV_SOUTH 	"NH/Skin/Production/East/Power/Res"

#define SECURE_LOAD "NH/Core/Cons/Elec_Mng_out/Power/Res"

#define OVEN 				"NH/Core/Cons/Oven/Power/Res"
#define COOKTOP			"NH/Core/Cons/Cooktop/Power/Res"
#define BATTERY_IN	"NH/Core/Cons/Elec_Mng_in/Power/Res"
#define CAR_CHARGER	"NH/Skin/Cons/Car_Battery/Power/Res"
#define NORTH_RECEP	"NH/Core/Cons/Rec_core_north/Power/Res"
#define SOUTH_RECEP	"NH/Core/Cons/Rec_core_south/Power/Res"
#define SKIN_RECEP	"NH/Skin/Cons/Rec/Power/Res"*/

#define OPTI_KG		"opti/:/kg"
#define OPTI_PROD	"opti/:/next_production"
#define OPTI_CONSO	"opti/:/next_consumption"
#define OPTI_POWER	"opti/:/batt_power"

#define CHARGER_ALLOWED "xcom1/:101/:/config/:/1125"
#define BATTERY_CHARGE_CURRENT_DC  "xcom1/:101/:/config/:/1138"
#define FORCE_NEW_CYCLE "xcom1/:101/:/config/:/1142"
#define USE_DYNAMIC_COMP  "xcom1/:101/:/config/:/1608"
#define FLOATING_VOLTAGE "xcom1/:101/:/config/:/1140"
#define FORCE_FLOATING "xcom1/:101/:/config/:/1467"
#define VOLTAGE_1_START_NEW_CYCLE  "xcom1/:101/:/config/:/1143"
#define VOLTAGE_2_START_NEW_CYCLE "xcom1/:101/:/config/:/1145"
#define TIME_1_UNDER_VOLTAGE  "xcom1/:101/:/config/:/1144"
#define TIME_2_UNDER_VOLTAGE  "xcom1/:101/:/config/:/1146"
#define ABSORPTION_PHASE_ALLOWED  "xcom1/:101/:/config/:/1155"
#define ABSORPTION_VOLTAGE  "xcom1/:101/:/config/:/1156"
#define ABSORPTION_DURATION  "xcom1/:101/:/config/:/1157"
#define END_ABSORPTION_CURRENT  "xcom1/:101/:/config/:/1158"
#define CURRENT_TO_QUIT_ABSORPTION  "xcom1/:101/:/config/:/1159"
#define MAXIMAL_FREQ_ABSORPTION  "xcom1/:101/:/config/:/1160"
#define DELAY_BEETWEN_ABSORPTION  "xcom1/:101/:/config/:/1161"
#define EQUALIZATION_PHASE_ALLOWED  "xcom1/:101/:/config/:/1163"
#define REDUCED_FLOATIN_ALLOWED  "xcom1/:101/:/config/:/1170"
#define PERIODIC_ABSORPTION_ALLOWED  "xcom1/:101/:/config/:/1173"
#define BAT_TEMP_COMP_COEF "xcom1/:101/:/config/:/1139"

 /*------------------------ Declaration parametres -------------------------------*/
	/*----------------- Battery Charge ----------------------------*/
	extern t_param Charger_allowed; //PCO OK
	extern t_param Battery_Charge_current_DC;//step 1A; PCO OK on init à zero
	extern t_param Force_new_cycle; // PCO param Inutile ?
	extern t_param Use_dynamic_comp; // PCO param Inutile ?
	extern t_param Floating_voltage;//step 0.1 , PCO en mode inspect il faudrait lire avant 7061
	extern t_param Force_floating; // PCO param inutile
	extern t_param Voltage_1_start_new_cycle;//step 0.10 PCO parm inutile
	extern t_param Voltage_2_start_new_cycle;//step 0.10 PCO parm inutile
	extern t_param Time_1_under_voltage;//step 1 PCO Parm inutile
	extern t_param Time_2_under_voltage;//step 10 PCO Parm inutile
	extern t_param Absorption_phase_allowed; // PCO OK
	extern t_param Absorption_voltage;//step 0.1 //PCO en mode inspect il faudrait lire avant 7061
	extern t_param Absorption_duration;//step 0.25h PCO : V. Penas suggere zero
	extern t_param End_absorption_current; // PCO parm inutile
	extern t_param Current_to_quit_absorption;//step 1A // PCO parm inutile
	extern t_param Maximal_freq_absorption;//PCO parm inutile
	extern t_param Delay_beetween_absorption;//step 1h , PCO parm inutile
	extern t_param Equalization_phase_allowed; // PCO Parm inutile
	extern t_param Reduced_floating_allowed; // PCO Parm inutile
	extern t_param Periodic_absorption_allowed; // PCO Parm inutile
	extern t_param Over_Voltage_Value_allowed; // PCO OK
	extern t_param Voltage_After_Over_allowed; // PCO OK
	extern t_param Under_Voltage_Value_allowed; // PCO suggere 15 x 2.9V limite de declenchement au lieu de 41V
	extern t_param Voltage_After_Under_allowed; // PCO Pram inutile
	extern t_param Bat_Temp_Com_Coef; // PCO OK
	/*-----------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
#define INVERTER_ALLOWED  "xcom1/:101/:/config/:/1124"
#define INVERTER_OUTPUT_VOLTAGE  "xcom1/:101/:/config/:/1286"
#define INVERTER_FREQUENCY  "xcom1/:101/:/config/:/1112"
#define BATT_PRIORITY_SOURCE  "xcom1/:101/:/config/:/1296"
	/* ----------------------------Inverter -----------------------------*/
	extern t_param Inverter_Allowed;//step 1 PCO OK
	extern t_param Inverter_Output_Voltage;//step 1 PCO : parma inutile ?
	extern t_param Inverter_Frequency;//step 0.1 PCO : parma inutile ?
	extern t_param Batt_priority_source; //PCO : parma inutile ?
	/*------------------------------------------------------------------*/
#define STANDBY_LEVEL  "xcom1/:101/:/config/:/1187"
#define STANDBY_NBR_PULSE  "xcom1/:101/:/config/:/1188"

	/*------------------------ Standby and turn on ------------------*/
	extern t_param Standby_level;//step 10% , à tester PCO : parma inutile ?
	extern t_param Standby_nbr_pulse;//step 1 //PCO : parma inutile ?
	/*--------------------------------------------------------------*/
#define TRANSFER_RELAY_ALLOED  "xcom1/:101/:/config/:/1128"
#define SMART_BOOS_ALLOWED  "xcom1/:101/:/config/:/1126"
#define MAX_CURRENT_OF_AC_IN  "xcom1/:101/:/config/:/1107"
	/*-------------------- AC-IN and transfert ---------------------*/
	extern t_param Transfer_relay_allowed; // PCO param inutile ?
	extern t_param Smart_boost_allowed; // PCO OK voir alogo ensuite
	extern t_param MAX_current_of_AC_IN;//step 1 PCO : OK
	/*-------------------------------------------------------------*/
#define MULTI_INVERTERS_ALLOWED  "xcom1/:101/:/config/:/1461"
#define INTEGRAL_MODE  "xcom1/:101/:/config/:/1283"
	//-------------- Multi Xtender System ----------------------
	extern t_param Multi_inverters_allowed; // PCO param inutile
	extern t_param Integral_mode; // PCO param inutile
	/*---------------------------------------------------------*/

#define SOC_BACKUP  "xcom1/:600/:/config/:/6062"
#define SOC_INJECT  "xcom1/:600/:/config/:/6063"
	//-------------- Multi Xtender System ----------------------
	extern t_param Soc_Backup; // PCO OK
	extern t_param Soc_Inject; // PCO OK corriger 6063 !
	/*---------------------------------------------------------*/

#define GRID_FEEDING_ALLOWED  "xcom1/:101/:/config/:/1127"
#define MAX_GRID_FEEDING_CURRENT  "xcom1/:101/:/config/:/1523"
#define BATTERY_VOLTAGE_FORCED_FEEDING  "xcom1/:101/:/config/:/1524"
#define START_TIME_FORCED_INJECTION  "xcom1/:101/:/config/:/1525"
#define STOP_TIME_FORCED_INJECTION  "xcom1/:101/:/config/:/1526"
#define USE_DEFINED_PHASE_SHIFT_CURVE  "xcom1/:101/:/config/:/1610"
#define COS_PHI_P_0  "xcom1/:101/:/config/:/1622"
#define COS_PHI_P_100  "xcom1/:101/:/config/:/1624"
#define COS_PHI_AT_P_1613  "xcom1/:101/:/config/:/1623"
#define POWER_OF_SECOND_COS_PHI_POINT  "xcom1/:101/:/config/:/1613"
#define FAST_CHARGE_INJECT_REGULATION "xcom1/:101/:/config/:/1615"
#define PULSE_CUTING_REGULATION_FOR_XT "xcom1/:101/:/config/:/1645"
	/*----------------- Grid Feeding --------------------------*/
	extern t_param Grid_Feeding_allowed; // PCO OK selon algo
	extern t_param Max_Grid_Feeding_current;//step 0.2 // PCO Rajouter test et diminuer 30 à 2?
	extern t_param Battery_voltage_forced_feeding;//step 0.1 à tester //PCO  Parm inutile ?
	extern t_param Start_Time_forced_injection;//step : 1, regarder avec Flo le format PCO pourquoi ?
	extern t_param Stop_Time_forced_injection;//step : 1, regarder avec Flo le format PCO pourquoi ?
	extern t_param Use_defined_phase_shift_curve; // PCO Parm inutile
	extern t_param Cos_phi_P_0;//step 0.01, à voir PCO parm inutile
	extern t_param Cos_phi_P_100;//step 0.01, à voir PCO parm inutile
	extern t_param Cos_phi_at_P_1613;//step 0.01 à voir PACO parm inutile
	extern t_param Power_of_second_cos_phi_point;//step 5% of Pnom, à voir parm inutile
	extern t_param Fast_charge_inject_regulation;
	extern t_param Pulses_cutting_regulation_for_XT;
/*------------------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
#define I_BATTERY_VOLTAGE_STUDER  "xcom1/:101/:/config/:/3000"
#define I_WANTED_BATTERY_CHARGE_CURRENT  "xcom1/:101/:/config/:/3004"
#define I_BATTERY_CHARGE_CURRENT  "xcom1/:101/:/config/:/3005"
#define I_STATE_OF_CHARGE  "xcom1/:101/:/config/:/3007"
#define I_BATTERY_CYCLE_PHASE  "xcom1/:101/:/config/:/3010"
#define I_INPUT_VOLTAGE_AC_IN  "xcom1/:101/:/config/:/3011"
#define I_INPUT_CURRENT_AC_IN  "xcom1/:101/:/config/:/3012"
#define I_INPUT_POWER_AC_IN  "xcom1/:101/:/config/:/3013"
#define I_INPUT_CURRENT_LIMIT  "xcom1/:101/:/config/:/3017"
#define I_INPUT_CURRENT_LIMIT_REACHED  "xcom1/:101/:/config/:/3018"
#define I_BOOST_ACTIVE  "xcom1/:101/:/config/:/3019"
#define I_OUTPUT_VOLTAGE_AC_OUT  "xcom1/:101/:/config/:/3021"
#define I_OUTPUT_CURRENT_AC_OUT  "xcom1/:101/:/config/:/3022"
#define I_OUTPUT_POWER_AC_OUT  "xcom1/:101/:/config/:/3023"
#define I_OPERATING_STATE  "xcom1/:101/:/config/:/3028"
#define I_STATE_OF_INVERTER  "xcom1/:101/:/config/:/3049"
#define I_DISCHARGE_BATTERY_POWER_DAY_AHEAD  "xcom1/:101/:/config/:/3076"
#define I_DISCHARGE_BATTERY_POWER_TODAY  "xcom1/:101/:/config/:/3078"

#define I_ENERGY_AC_IN_DAY_A_HEAD  "xcom1/:101/:/config/:/3080"
#define I_ENERGY_AC_IN_TODAY "xcom1/:101/:/config/:/3081"
#define I_ENERGY_AC_OUT_DAY_A_HEAD "xcom1/:101/:/config/:/30802"
#define I_ENERGY_AC_OUT_TODAY "xcom1/:101/:/config/:/3083"
#define I_AC_IN_FREQUENCY "xcom1/:101/:/config/:/3084"
#define I_AC_OUT_FREQUENCY "xcom1/:101/:/config/:/3085"
#define I_INJECTION_CURRENT_AC_DESIRED "xcom1/:101/:/config/:/3155"
#define I_BATTERY_PRIORITY_ACTIVE "xcom1/:101/:/config/:/3161"
#define I_FORCED_GRID_FEEDING_ACTIVE "xcom1/:101/:/config/:/3162"

/*--------------------------- Xtender_Infos --------------------------------------*/
	extern t_param i_Battery_Voltage_Studer;
	extern t_param i_Wanted_battery_charge_current;
	extern t_param i_Battery_Charge_current;
	extern t_param i_State_of_charge;
	extern t_param i_Battery_cycle_phase; // 0 = invalid , 1 = Bulk , 2 = absorption , 3 = equalize , 4 = floating , 5 = Reduce floating , 6 = Periodic absorption , 7 = mixing , 8 = forming
	extern t_param i_Input_voltage_AC_IN;
	extern t_param i_Input_current_AC_IN;
	extern t_param i_Input_power_AC_IN;
	extern t_param i_Input_current_limit;
	extern t_param i_Input_current_limit_reached; // 0 = Off , 1 = on
	extern t_param i_Boost_active;// 0 = Off , 1 = on
	extern t_param i_Output_voltage_AC_OUT;
	extern t_param i_Output_current_AC_OUT;
	extern t_param i_Output_power_AC_OUT;
	extern t_param i_Operating_state; // 0 = invalid, 1 = inverte, 2 = charger, 3 = Boost,  4 = Injection
	extern t_param i_State_of_inverter; // 0 = Off , 1 = on
	extern t_param i_Discharge_Battery_Power_Day_ahead;
	extern t_param i_Dishcarge_Battery_Power_today;

	extern t_param i_Energy_AC_IN_Day_ahead;
	extern t_param i_Energy_AC_IN_today;
	extern t_param i_Energy_AC_OUT_Day_ahead;
	extern t_param i_Energy_AC_OUT_today;
	extern t_param i_AC_IN_frequency;
	extern t_param i_AC_OUT_frequency;
	extern t_param i_Injection_current_AC_desired;
	extern t_param i_Battery_priority_active;// 0 = Off , 1 = on
	extern t_param i_Forced_grid_feeding_active;// 0 = Off , 1 = on


	#define I_BATTERY_VOLTAVGE "xcom1/:600/:/config/:/7000"
	#define I_BATTERYY_CURRENT "xcom1/:600/:/config/:/7001"
	#define I_SOC_VALUE_BATTERY "xcom1/:600/:/config/:/7047"
	#define I_STATE_OF_HEALTH "xcom1/:600/:/config/:/7057"
	#define I_BATTERY_VOLTAGE_CHARGE_LIMIT "xcom1/:600/:/config/:/7061"
	#define I_BATTERY_VOLTAGE_DISCHARGE_LIMIT "xcom1/:600/:/config/:/7062"
	#define I_BATTERY_CURRENT_CHARGE_LIMIT "xcom1/:600/:/config/:/7063"
	#define I_BATTERY_CURRENT_DISCHARGE_LIMIT "xcom1/:600/:/config/:/7064"
	#define I_BATTERY_CURRENT_CHARGE_RECOMMANDED "xcom1/:600/:/config/:/7065"
	#define I_BATTERY_CURRENT_DISCHARGE_RECOMMANDED "xcom1/:600/:/config/:/7066"
	/*--------------------------- Batterry info --------------------------------------*/


// PCO
// psmax = courant_decharge_limite IAdc (param 7064) x tension_dc (param 7000)
// a voir s'il faut pas remplacer par des valeur moyenne de 1 min, autre param disponibles

extern t_param i_Battery_Voltage;
extern t_param i_Battery_Current;
extern t_param i_soc_value_battery;
extern t_param i_State_of_Health;


extern t_param i_Battery_Voltage_Charge_limit;
extern t_param i_Battery_Voltage_Discharge_limit;
extern t_param i_Battery_Current_Charge_limit;
extern t_param i_Battery_Current_Discharge_limit;
extern t_param i_Battery_Current_Charge_recommanded;
extern t_param i_Battery_Current_Discharge_recommanded;

// PCO : En mode inspect control, soit Xtender master
  // On peut lire la tension de charge à appliquer sur {1140} et {1156} (={7061}) (floating et absorption voltages)
  // de même courant de charge  -	décharge :
  //{1523}[Aac] < {7064}[Adc]*{7000}[Udc]/230[Vac]
  //charge {1138} [Adc] < {7065} [Adc] )


/*----------------------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
extern t_param Parameters_saved_in_flash_memory;

extern double meters[];

extern float Bat_Capacite_nominal;
extern float Bat_Capacite_disponible; // avec un state of health de 100% batterie pleine et SOCmin=0%, 10.8 justeune valeu par défaut

extern float Eb;
extern float SOCmin;
extern float SOCmax;
extern float Cve;
extern float SOC;
extern float Ps;
extern float Ps_opti;
extern float Pr;
extern float Kg;
extern float Kr;
extern float Ppv;
extern float Pl;
extern float Psmax;
// PCO
extern float Psmax_charge;
extern float Psmax_discharge;
extern float Prmax;
extern float Kc;
extern float Pb;
extern float Plsec;
extern float Kp;
extern float Time_now;
extern float Temps_routine;
extern float Epv;
extern float El;
extern float Km;
extern float K_g;
extern int algo_on;
extern float Kg_1;
extern float Kg_2;
extern int Time_now_i;
extern int charge_on;
extern float AC_IN_INJ_I_Des;
extern int STATE;
extern int INJ;

extern int Time_old;
extern time_t my_time;
extern struct tm * timeinfo;

#endif

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


#define PV_WEST 	"NH/Skin/Production/West/Power/Res"
#define PV_EAST 	"NH/Skin/Production/South/Power/Res"
#define PV_SOUTH 	"NH/Skin/Production/East/Power/Res"

#define SECURE_LOAD "NH/Core/Cons/Elec_Mng_out/Power/Res"

#define OVEN 				"NH/Core/Cons/Oven/Power/Res"
#define COOKTOP			"NH/Core/Cons/Cooktop/Power/Res"
#define BATTERY_IN	"NH/Core/Cons/Elec_Mng_in/Power/Res"
#define CAR_CHARGER	"NH/Skin/Cons/Car_Battery/Power/Res"
#define NORTH_RECEP	"NH/Core/Cons/Rec_core_north/Power/Res"
#define SOUTH_RECEP	"NH/Core/Cons/Rec_core_south/Power/Res"
#define SKIN_RECEP	"NH/Skin/Cons/Rec/Power/Res"

#define OPTI_KG		"opti/:/kg"
#define OPTI_PROD	"opti/:/next_production"
#define OPTI_CONSO	"opti/:/next_consumption"
#define OPTI_POWER	"opti/:/batt_power"

 /*------------------------ Declaration parametres -------------------------------*/
	/*----------------- Battery Charge ----------------------------*/
	extern t_param Charger_allowed; //PCO OK
	extern t_param Battery_Charge_current_DC;//step 1A; PCO OK on init à zero
	extern t_param Floating_voltage;//step 0.1 , PCO en mode inspect il faudrait lire avant 7061
	extern t_param Force_floating; // PCO param inutile
	/*-----------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
	/* ----------------------------Inverter -----------------------------*/
	extern t_param Inverter_Allowed;//step 1 PCO OK
	extern t_param Batt_priority_source; //PCO : parma inutile ?
	/*------------------------------------------------------------------*/
	/*-------------------- AC-IN and transfert ---------------------*/
	extern t_param Smart_boost_allowed; // PCO OK voir alogo ensuite
	extern t_param MAX_current_of_AC_IN;//step 1 PCO : OK
	/*-------------------------------------------------------------*/
	/*----------------- Grid Feeding --------------------------*/
	extern t_param Grid_Feeding_allowed; // PCO OK selon algo
	extern t_param Max_Grid_Feeding_current;//step 0.2 // PCO Rajouter test et diminuer 30 à 2?
/*------------------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
/*--------------------------- Xtender_Infos --------------------------------------*/
	extern t_param i_Battery_Voltage_Studer;
	extern t_param i_Battery_Charge_current;
	extern t_param i_State_of_charge;
	extern t_param i_Battery_cycle_phase; // 0 = invalid , 1 = Bulk , 2 = absorption , 3 = equalize , 4 = floating , 5 = Reduce floating , 6 = Periodic absorption , 7 = mixing , 8 = forming
	extern t_param i_Input_voltage_AC_IN;
	extern t_param i_Input_current_AC_IN;
	extern t_param i_Output_voltage_AC_OUT;
	extern t_param i_Output_power_AC_OUT;
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

extern float Eb;
extern float SOCmin;
extern float SOCmax;
extern float SOC;
extern float Ps;
extern float Ps_opti;
extern float Pr;
extern float Ppv;
extern float Pl;
extern float Psmax;
extern float Kg;

// PCO
extern float Psmax_charge;
extern float Psmax_discharge;
extern float Prmax;
extern float Pb;
extern float Plsec;
extern float Time_now;
extern float Temps_routine;
extern float Epv;
extern float El;
extern int algo_on;
extern int Time_now_i;
extern int STATE;

extern int Time_old;
extern time_t my_time;
extern struct tm * timeinfo;
extern Dictionary *Parameter_dic;

#endif

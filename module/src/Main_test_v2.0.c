//Fichier : Main_test
//Projet  : ∑PS6 Pilotage-micro réseau - Solar Decathlon 2017
//But	  : Test des routine connect, deconnect read and write
//Auteurs : Capone M., Botta D, Meyer Florian.
//Version : 2.1
//Date 	  : 28.08.2018
//Status  : test

#include <stdio.h>
#include <string.h>
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

#include <MQTTClient.h>
#include "conversion.h"
#include "connection.h"
#include "Write_read_v20.h"

#define true 1
#define false 0
#define int_temp_s 60
#define int_temp_us 0

#include "cJSON.h"
;

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
//niveau de tension -> floating voltage
//forcer l'extender en float
//bloquer absorption et egalisation

	//---------- DECLARATRION OF VAR -----------------------------------------------
	/*------------------------ Declaration parametres -------------------------------*/
	/*----------------- Battery Charge ----------------------------*/
	t_param Charger_allowed = 		{1125, 2, 13, 1,   1,"NH/Mng_elec/Ondu/Mode/Charger_allow/Res","NH/Mng_elec/Ondu/Mode/Charger_allow/Set"}; //PCO OK
	t_param Battery_Charge_current_DC = 	{1138, 2, 13, 6,  0,"NH/Mng_elec/Ondu/Batt/Charge_cur/Res","NH/Mng_elec/Ondu/Batt/Charge_cur/Set"};//step 1A; PCO OK on init à zero
	t_param Floating_voltage = 		{1140, 2, 13, 6,  61.5,"NH/Mng_elec/Ondu/Batt/Floating_volt/Res","NH/Mng_elec/Ondu/Batt/Floating_volt/Set"};//step 0.1 , PCO en mode inspect il faudrait lire avant 7061
	t_param Force_floating = 		{1467, 2, 13, 5,   0,"NH/Mng_elec/Ondu/Batt/Force_Flt/Res","NH/Mng_elec/Ondu/Batt/Force_Flt/Set"}; // PCO param inutile
	/*-----------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11
	/* ----------------------------Inverter -----------------------------*/
	t_param Inverter_Allowed = 	  	  {1124, 2, 13, 1, 1,"NH/Mng_elec/Ondu/Mode/Inverter_allow/Res","NH/Mng_elec/Ondu/Mode/Inverter_allow/Set"};//step 1 PCO OK
	t_param Batt_priority_source =    {1296, 2, 13, 1, 0,"NH/Mng_elec/Ondu/Mode/Batt_prio/Res","NH/Mng_elec/Ondu/Mode/Batt_prio/Set"}; //PCO : parma inutile ?
	/*------------------------------------------------------------------*/
	/*-------------------- AC-IN and transfert ---------------------*/
	t_param Smart_boost_allowed = 	 {1126, 2, 13, 1, 1,"NH/Mng_elec/Ondu/Mode/Smtboos_allow/Res","NH/Mng_elec/Ondu/Mode/Smtboos_allow/Set"}; // PCO OK voir alogo ensuite
	t_param MAX_current_of_AC_IN =   {1107, 2, 13, 6, 35,"NH/Mng_elec/Ondu/Input/Max_cur/Res","NH/Mng_elec/Ondu/Input/Max_cur/Set"};//step 1 PCO : OK
	t_param Transfer_relay_allowed = {1128, 2, 13, 1, 1,"NH/Mng_elec/Ondu/Mode/Transfer_relay_allowed/Res","NH/Mng_elec/Ondu/Mode/Transfer_relay_allowed/Set"}; // PCO param inutile ?
	/*-------------------------------------------------------------*/
	/*----------------- Grid Feeding --------------------------*/
	t_param Grid_Feeding_allowed = 			     {1127, 2, 13, 1,    1,"NH/Mng_elec/Ondu/Mode/Grid_feed_allow/Res","NH/Mng_elec/Ondu/Mode/Grid_feed_allow/Set"}; // PCO OK selon algo
	t_param Max_Grid_Feeding_current = 		   {1523, 2, 13, 6,   30.0,"NH/Mng_elec/Ondu/Input/Max_gird_cur/Res","NH/Mng_elec/Ondu/Input/Max_gird_cur/Set"};//step 0.2 // PCO Rajouter test et diminuer 30 à 2?
	/*------------------------------------------------------------------------------*/
	/*--------------------------- Xtender_Infos --------------------------------------*/
	t_param i_Battery_Voltage_Studer = 				 {3000, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Voltage/Res","NH/Mng_elec/Ondu/Voltage/Set"};
	t_param i_Battery_Charge_current = 			   {3005, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Batt/Meas/Charge_cur/Res","NH/Mng_elec/Ondu/Batt/Meas/Charge_cur/Set"};
	t_param i_State_of_charge = 				       {3007, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Batt/Soc/Res","NH/Mng_elec/Ondu/Batt/Soc/Set"};
	t_param i_Battery_cycle_phase = 			     {3010, 1, 1, 11, Not_Value,"NH/Mng_elec/Ondu/Batt/Cycle_phase/Res","NH/Mng_elec/Ondu/Batt/Cycle_phase/Set"}; // 0 = invalid , 1 = Bulk , 2 = absorption , 3 = equalize , 4 = floating , 5 = Reduce floating , 6 = Periodic absorption , 7 = mixing , 8 = forming
	t_param i_Input_voltage_AC_IN = 			     {3011, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Input/Voltage/Res","NH/Mng_elec/Ondu/Input/Voltage/Set"};
	t_param i_Input_current_AC_IN = 			     {3012, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Input/Cur/Res","NH/Mng_elec/Ondu/Input/Cur/Set"};
	t_param i_Output_voltage_AC_OUT = 			   {3021, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Output/Voltage/Res","NH/Mng_elec/Ondu/Output/Voltage/Set"};
	t_param i_Output_power_AC_OUT = 			     {3023, 1, 1,  6, Not_Value,"NH/Mng_elec/Ondu/Output/Power/Res","NH/Mng_elec/Ondu/Output/Power/Set"};
	/*--------------------------- Batterry info --------------------------------------*/
	// PCO
	// psmax = courant_decharge_limite IAdc (param 7064) x tension_dc (param 7000)
	// a voir s'il faut pas remplacer par des valeur moyenne de 1 min, autre param disponibles
	t_param i_Battery_Voltage = 	{7000, 1, 1, 6, 0,"NH/Mng_elec/Batt/Voltage/Res","NH/Mng_elec/Batt/Voltage/Set"};
	t_param i_Battery_Current = 	{7001, 1, 1, 6, 0,"NH/Mng_elec/Batt/Current/Res","NH/Mng_elec/Batt/Current/Set"};
	t_param i_soc_value_battery = {7047, 1, 1, 6, 0,"NH/Mng_elec/Batt/Soc/Res","NH/Mng_elec/Batt/Soc/Set"};
	t_param i_State_of_Health = 	{7057, 1, 1, 6, 0,"NH/Mng_elec/Batt/State_of_health/Res","NH/Mng_elec/Batt/State_of_health/Set"};

	t_param i_Battery_Voltage_Charge_limit = 	 				{7061, 1, 1, 6, 0,"NH/Mng_elec/Batt/Charge/Volt_limit/Res","NH/Mng_elec/Batt/Charge/Volt_limit/Set"};
	t_param i_Battery_Voltage_Discharge_limit =	 			{7062, 1, 1, 6, 0,"NH/Mng_elec/Batt/Discharge/Volt_limit/Res","NH/Mng_elec/Batt/Discharge/Volt_limit/Set"};
	t_param i_Battery_Current_Charge_limit = 	 				{7063, 1, 1, 6, 0,"NH/Mng_elec/Batt/Charge/Curr_limit/Res","NH/Mng_elec/Batt/Charge/Curr_limit/Set"};
	t_param i_Battery_Current_Discharge_limit =				{7064, 1, 1, 6, 0,"NH/Mng_elec/Batt/Discharge/Curr_limit/Res","NH/Mng_elec/Batt/Discharge/Curr_limit/Set"};
	t_param i_Battery_Current_Charge_recommanded = 		{7065, 1, 1, 6, 0,"NH/Mng_elec/Batt/Charge/Curr_recommand/Res","NH/Mng_elec/Batt/Charge/Curr_recommand/Set"};
	t_param i_Battery_Current_Discharge_recommanded = {7066, 1, 1, 6, 0,"NH/Mng_elec/Batt/Discharge/Curr_recommand/Res","NH/Mng_elec/Batt/Discharge/Curr_recommand/Set"};

	// PCO : En mode inspect control, soit Xtender master
  // On peut lire la tension de charge à appliquer sur {1140} et {1156} (={7061}) (floating et absorption voltages)
  // de même courant de charge  -	décharge :
  //{1523}[Aac] < {7064}[Adc]*{7000}[Udc]/230[Vac]
  //charge {1138} [Adc] < {7065} [Adc] )


	/* ----- Old Parameters not used ---------------------------
	//----------------- Battery Charge ----------------------------
	t_param Force_new_cycle = 		{1142, 2, 13, 5,   0}; // PCO param Inutile ?
	t_param Use_dynamic_comp = 		{1608, 2, 13, 1,   0}; // PCO param Inutile ?
	t_param Voltage_1_start_new_cycle = 	{1143, 2, 13, 6,  45.0};//step 0.10 PCO parm inutile
	t_param Voltage_2_start_new_cycle = 	{1145, 2, 13, 6,  44.0};//step 0.10 PCO parm inutile
	t_param Time_1_under_voltage = 		{1144, 2, 13, 6, 180};//step 1 PCO Parm inutile
	t_param Time_2_under_voltage = 		{1146, 2, 13, 6, 180};//step 10 PCO Parm inutile
	t_param Absorption_phase_allowed = 	{1155, 2, 13, 1,   0}; // PCO OK
	t_param Absorption_voltage = 		{1156, 2, 13, 6,  63.0};//step 0.1 //PCO en mode inspect il faudrait lire avant 7061
	t_param Absorption_duration = 		{1157, 2, 13, 6,   0};//step 0.25h PCO : V. Penas suggere zero
	t_param End_absorption_current = 	{1158, 2, 13, 1,   1}; // PCO parm inutile
	t_param Current_to_quit_absorption = 	{1159, 2, 13, 6,   2};//step 1A // PCO parm inutile
	t_param Maximal_freq_absorption = 	{1160, 2, 13, 1,   1};//PCO parm inutile
	t_param Delay_beetween_absorption =	{1161, 2, 13, 6,   1};//step 1h , PCO parm inutile
	t_param Equalization_phase_allowed = 	{1163, 2, 13, 1,   0}; // PCO Parm inutile
	t_param Reduced_floating_allowed = 	{1170, 2, 13, 1,   0}; // PCO Parm inutile
	t_param Periodic_absorption_allowed = 	{1173, 2, 13, 1,   0}; // PCO Parm inutile
	t_param Over_Voltage_Value_allowed = 	{1121, 2, 13, 6,  63.0}; // PCO OK
	t_param Voltage_After_Over_allowed =    {1122, 2, 13, 6,  61.5}; // PCO OK
	t_param Under_Voltage_Value_allowed =	{1108, 2, 13, 6, 43.0}; // PCO suggere 15 x 2.9V limite de declenchement au lieu de 41V
	t_param Voltage_After_Under_allowed = 	{1110, 2, 13, 6, 45.0}; // PCO Pram inutile
	t_param Bat_Temp_Com_Coef = {1139,2,13,6,0}; // PCO OK
	//-----------------------------------------------------------------------
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11
	// ----------------------------Inverter -----------------------------
	t_param Inverter_Output_Voltage = {1286, 2, 13, 6, 230.0};//step 1 PCO : parma inutile ?
	t_param Inverter_Frequency = 	    {1112, 2, 13, 6,  50.0};//step 0.1 PCO : parma inutile ?
	//------------------------------------------------------------------
	//------------------------ Standby and turn on ------------------
	t_param Standby_level = 	    {1187, 2, 13, 6, 20};//step 10% , à tester PCO : parma inutile ?
	t_param Standby_nbr_pulse =  {1188, 2, 13, 6,  1};//step 1 //PCO : parma inutile ?
	//--------------------------------------------------------------
	//-------------------- AC-IN and transfert ---------------------
	//-------------------------------------------------------------
	//-------------- Multi Xtender System ----------------------
	t_param Multi_inverters_allowed = {1461, 2, 13, 1, 0}; // PCO param inutile
	t_param Integral_mode = 	  	    {1283, 2, 13, 1, 0}; // PCO param inutile
	//---------------------------------------------------------
	//-------------- Multi Xtender System ----------------------
	t_param Soc_Backup = {6062, 2, 13, 6, 80}; // PCO OK
	t_param Soc_Inject = {6063, 2, 13, 6, 80}; // PCO OK corriger 6063 !
	//---------------------------------------------------------
	//----------------- Grid Feeding --------------------------
	t_param Battery_voltage_forced_feeding = {1524, 2, 13, 6,    40.0};//step 0.1 à tester //PCO  Parm inutile ?
	t_param Start_Time_forced_injection =	   {1525, 2, 13, 5,   10};//step : 1, regarder avec Flo le format PCO pourquoi ?
	t_param Stop_Time_forced_injection =	   {1526, 2, 13, 5,   10};//step : 1, regarder avec Flo le format PCO pourquoi ?
	t_param Use_defined_phase_shift_curve  = {1610, 2, 13, 1,    1}; // PCO Parm inutile
	t_param Cos_phi_P_0 = 					         {1622, 2, 13, 6,    0};//step 0.01, à voir PCO parm inutile
	t_param Cos_phi_P_100 = 				         {1624, 2, 13, 6,  0.1};//step 0.01, à voir PCO parm inutile
	t_param Cos_phi_at_P_1613 = 			       {1623, 2, 13, 6, 0.05};//step 0.01 à voir PACO parm inutile
	t_param Power_of_second_cos_phi_point =  {1613, 2, 13, 6,   50};//step 5% of Pnom, à voir parm inutile
	t_param Fast_charge_inject_regulation =  {1615, 2, 13, 1, 	 1};
	t_param Pulses_cutting_regulation_for_XT = {1645, 2, 13, 1, 1};

	//------------------------------------------------------------------------------

	//--------------------------- Xtender_Infos --------------------------------------
	t_param i_Wanted_battery_charge_current =  {3004, 1, 1,  6, Not_Value};
	t_param i_Input_power_AC_IN = 				     {3013, 1, 1,  6, Not_Value};
	t_param i_Input_current_limit = 			     {3017, 1, 1,  6, Not_Value};
	t_param i_Input_current_limit_reached = 	 {3018, 1, 1, 11, Not_Value}; // 0 = Off , 1 = on
	t_param i_Boost_active = 					         {3019, 1, 1, 11, Not_Value};// 0 = Off , 1 = on
	t_param i_Output_current_AC_OUT = 			   {3022, 1, 1,  6, Not_Value};
	t_param i_Operating_state = 				       {3028, 1, 1, 11, Not_Value}; // 0 = invalid, 1 = inverte, 2 = charger, 3 = Boost,  4 = Injection
	t_param i_State_of_inverter = 				     {3049, 1, 1, 11, Not_Value}; // 0 = Off , 1 = on
	t_param i_Discharge_Battery_Power_Day_ahead = {3076, 1, 1,  6, Not_Value};
	t_param i_Dishcarge_Battery_Power_today =  {3078, 1, 1,  6, Not_Value};

	t_param i_Energy_AC_IN_Day_ahead = 			   {3080, 1, 1,  6, Not_Value};
	t_param i_Energy_AC_IN_today = 				     {3081, 1, 1,  6, Not_Value};
	t_param i_Energy_AC_OUT_Day_ahead = 		   {3082, 1, 1,  6, Not_Value};
	t_param i_Energy_AC_OUT_today = 			     {3083, 1, 1,  6, Not_Value};
	t_param i_AC_IN_frequency = 				       {3084, 1, 1,  6, Not_Value};
	t_param i_AC_OUT_frequency = 				       {3085, 1, 1,  6, Not_Value};
	t_param i_Injection_current_AC_desired = 	 {3155, 1, 1,  6, Not_Value};
	t_param i_Battery_priority_active = 		   {3161, 1, 1, 11, Not_Value};// 0 = Off , 1 = on
	t_param i_Forced_grid_feeding_active = 		 {3162, 1, 1, 11, Not_Value};// 0 = Off , 1 = on

	//----------------------------------------------------------------------------------
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
	t_param Parameters_saved_in_flash_memory = {1550, 2, 5, 1, 1};*/


double meters[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0,0.0,0.0,0.0};

float Eb=0;
float SOCmin = 20;
float SOCmax = 80;
float SOC = 0;
float Ps = 0;
float Ps_opti = 0;
float Pr = 0;
float Ppv = 0;
float Pl = 0;
float Psmax = 0;
// PCO
float Psmax_charge =0;
float Psmax_discharge =0;
float Prmax = 0;
float Pb = 0;
float Plsec = 0;
float Time_now = 0;
float Temps_routine = 0;
float Epv = 0;
float El = 0;
float Kg = 0;
int algo_on = 1;
int Time_now_i;
int STATE;

int Time_old;
time_t my_time;
struct tm * timeinfo;
Dictionary *Parameter_dic;


/*----------------------------------------------------------------------------------------------*/

void State_management(int state){
	switch (state){
	case 1:
		printf("case 1 of state management ----------------\n");
		//Activer l'onduleur;
		Inverter_Allowed.Value = true; //Param 1124;
		//Bloquer la charge;
		Charger_allowed.Value = false;//Param 1125;
		//Activation du SmartBoost;
		Smart_boost_allowed.Value = true; //Param 1126;
		//Autoriser l'injection;
		Grid_Feeding_allowed.Value = true; //Param 1127;
		//tran transfert allowed
		Transfer_relay_allowed.Value = 1; //Param 1128
		Force_floating.Value = true;
		Floating_voltage.Value  = i_Battery_Voltage_Discharge_limit.Value;
		printf("value of floating : %f; %f\n",Floating_voltage.Value,i_Battery_Voltage_Discharge_limit.Value);

		//Régulation du ratio de puissance Pbatt vs Pres via Iac AC-IN;
		Max_Grid_Feeding_current.Value = fabs(Ps) / i_Input_voltage_AC_IN.Value;
		if(Max_Grid_Feeding_current.Value >= (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value)
		Max_Grid_Feeding_current.Value = (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value;													//value dynamic for discharge
		//printf("Max grid feeding : %f\n",Max_Grid_Feeding_current.Value);
		//printf("Battery current discharge limi : %f\n",i_Battery_Current_Discharge_limit.Value);
		if(Max_Grid_Feeding_current.Value >= 34.0) Max_Grid_Feeding_current.Value = 34.0; // 8.6 pour 2 kW

		MAX_current_of_AC_IN.Value = 8000 / i_Input_voltage_AC_IN.Value;
		if(MAX_current_of_AC_IN.Value >= 34.0) MAX_current_of_AC_IN.Value = 34.0; // 8.6 pour 2 kw

		printf("end case 1 :--------------------------------------\n");
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		printf("case 2,3, 4 and 5 of state management ----------------\n");
		Inverter_Allowed.Value = true; //Param 1124;
		//Activer la charge
		Charger_allowed.Value = true; //Param 1125;
		//Activation du SmartBoost;
		Smart_boost_allowed.Value = true; //Param 1126;
		//Activation l'injection;
		Grid_Feeding_allowed.Value = false; //Param 1127;

		Force_floating.Value = true;
		Floating_voltage.Value = i_Battery_Voltage_Charge_limit.Value; //tension maximun de charge de 61 Voltage_1_start_new_cycle
		printf("value of floating : %f; %f\n",Floating_voltage.Value,i_Battery_Voltage_Charge_limit.Value);


		Battery_Charge_current_DC.Value = fabs(Ps)/i_Battery_Voltage.Value;
		if(Battery_Charge_current_DC.Value > i_Battery_Current_Charge_limit.Value)
	 	Battery_Charge_current_DC.Value = i_Battery_Current_Charge_limit.Value;
		if(Battery_Charge_current_DC.Value > 200) //limit des disjoncteur des batteries
		Battery_Charge_current_DC.Value = 190;

		MAX_current_of_AC_IN.Value = 8000 / i_Input_voltage_AC_IN.Value;
		if(MAX_current_of_AC_IN.Value >= 34.0) MAX_current_of_AC_IN.Value = 34.0; // 8.6 pour 2 kw

		printf("end case 2,3,4 & 5 :--------------------------------------\n");
		break;
	}
}

/*---------------------------- Routine de Calculs des coefficients ----------------------------*/
void Calculs_p_k(void)
{
	if (Time_now ==1440) Time_now = 0;
	//Ppv = 10000;       //Puissance des PV
	//Pl = 9000;        //Puissance des charges
	Epv = Ppv;//Ppv_1(int)Time_now]/60;       //Energie PV
	El = Pl;//Pl_1[(int)Time_now]/60;       //Energie des chatges
	Eb = Epv - El; //Energie bilan
	Pb = (Ppv - Pl);  //Puissance bilan

	SOC = i_soc_value_battery.Value;
	printf("Soc value : %f\n",SOC);
	//SOC = 90;

	//Plsec = 500;     //Puissance des charges securisées
	Plsec = i_Output_power_AC_OUT.Value*1000;

	//Ps = Ps_opti*1000;        //Puissance demandée au stockage

	Ps = 0;				//puissance échanger avec le réseau
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
void Algo()
{
	Calculs_p_k();
	// PCO : a défaut d'une valeur Ps (stockage) calculée dans un algorithme d'optimisation, Ps =Pb (balance)
	Ps=Pb;
	printf("ps : %f, pb : %f\n",Ps,Pb);
	State_management(STATE);
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
      	printf("STATE = 5;\n");
      	STATE = 5;
				Ps = 0; //couvre l'autoconsomation de la batterie'
				Pr = Pb;
			}
			else
			{
				printf("STATE = 4;\n");
				STATE = 4;
				//Ps =  Ps;
			}

		}
		else
		{	//la batterie n'est pas trop pas encore pleine
      printf("STATE = 3;\n");
      STATE = 3;
			Ps = Pb;
			Pr = 0;
		}
		State_management(STATE);
		// la puissance max demander au réseau est elle trop grande ?
		if (fabs(Pr) >= Prmax)
		{
			Pr = Prmax;
		}
	} // -------------------------------------------------------------------------
	else
	{ // non on ne produit pas
		// la batterie est elle presque vide ?
		if (1.03*SOCmin >= SOC)
		{
			// la batterie est elle vraiment vide ?
			if (SOCmin >= SOC)
			{
      	STATE = 2;
      	printf("STATE = 2;\n");
      	Ps = 50; //couvre l'autoconsomation de la batterie'
				Pr = Pb;
			}
			/*else
			{
				Ps = Ps;
			}*/
		}
		else
		{
    	STATE = 1;
    	printf("STATE = 1;\n");
			Ps = Pb;
			Pr = 0;
		}
		State_management(STATE);
		if (fabs(Pr) >= Prmax)
		{
			Pr = - Prmax;
		}
	}
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
 	printf(" ");
 	algo_on = 1;
 	i++;
 	if(i == 6) i = 0;
 		get_Time();
   	printf("%d:%d;", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
   	printf("SOC = %f;", SOC);
		printf("Eb = %f;", Eb);
		printf("Pr = %f;", Pr);
   	printf("Ps = %f;", Ps);
   	printf("Pl = %f;", Pl);
   	printf("Ppv = %f;", Ppv);
   	printf("Pb = %f;", Pb);
   	printf("Plsec = %f;", Plsec);
   	printf("Ibat = %f;", i_Battery_Charge_current.Value);
   	printf("Ubat = %f;",i_Battery_Voltage_Studer.Value);
   	printf("Iac_in = %f;", i_Input_current_AC_IN.Value);
   	printf("Uac = %f;",i_Input_voltage_AC_IN.Value);
		printf("Max_grid_geed = %f;",Max_Grid_Feeding_current.Value);

		printf("floating voltage : %f ;bat actu : %f;",Floating_voltage.Value,i_Battery_Voltage.Value-1);
		printf("battery volt charge limit : %f; batt volt discharge limit : %f;",i_Battery_Voltage_Charge_limit.Value,i_Battery_Voltage_Discharge_limit.Value);
		printf("batt current charge limit :%f;",i_Battery_Current_Charge_limit.Value);
		printf("batt current discharge limit : %f;",i_Battery_Current_Discharge_limit.Value);
		printf("batt charge current  : %f;",Battery_Charge_current_DC.Value);
		printf("max grid feeding : %f;",Max_Grid_Feeding_current.Value);
		printf("actual vol : %f;\n",i_Battery_Voltage.Value);

   	FILE * fd = 0;
   	fd = fopen("read_fast.txt", "a");
   	fprintf(fd,"STATE = %d;",STATE);
   	fprintf(fd,"%d:%d;", (int)(Time_now/60), (int)(Time_now - (int)(Time_now/60)*60));
   	fprintf(fd,"SOC = %f;", SOC);
		fprintf(fd,"Eb = %f;",Eb);
		fprintf(fd,"Pr = %f;",Pr);
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

		fprintf(fd,"floating voltage : %f ; bat actu : %f;",Floating_voltage.Value,i_Battery_Voltage.Value-1);
		fprintf(fd,"battery volt charge limit : %f; batt volt discharge limit : %f;",i_Battery_Voltage_Charge_limit.Value,i_Battery_Voltage_Discharge_limit.Value);
		fprintf(fd,"batt current charge limit :%f;",i_Battery_Current_Charge_limit.Value);
		fprintf(fd,"batt current discharge limit : %f;",i_Battery_Current_Discharge_limit.Value);
		fprintf(fd,"batt charge current  : %f;",Battery_Charge_current_DC.Value);
		fprintf(fd,"max grid feeding : %f;",Max_Grid_Feeding_current.Value);
		fprintf(fd,"actual vol : %f;\n",i_Battery_Voltage.Value);
  	fclose(fd);

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
void init_dic(Dictionary *dictionary){
	dict_add(dictionary,Charger_allowed.Id_read,&Charger_allowed);
	dict_add(dictionary,Battery_Charge_current_DC.Id_read,&Battery_Charge_current_DC);
	dict_add(dictionary,Floating_voltage.Id_read,&Floating_voltage);
	dict_add(dictionary,Force_floating.Id_read,&Force_floating);
	dict_add(dictionary,Inverter_Allowed.Id_read,&Inverter_Allowed);
	dict_add(dictionary,Batt_priority_source.Id_read,&Batt_priority_source);
	dict_add(dictionary,Smart_boost_allowed.Id_read,&Smart_boost_allowed);
	dict_add(dictionary,MAX_current_of_AC_IN.Id_read,&MAX_current_of_AC_IN);
	dict_add(dictionary,Grid_Feeding_allowed.Id_read,&Grid_Feeding_allowed);
	dict_add(dictionary,Max_Grid_Feeding_current.Id_read,&Max_Grid_Feeding_current);
	dict_add(dictionary,i_Battery_Voltage_Studer.Id_read,&i_Battery_Voltage_Studer);
	dict_add(dictionary,i_Battery_Charge_current.Id_read,&i_Battery_Charge_current);
	dict_add(dictionary,i_State_of_charge.Id_read,&i_State_of_charge);
	dict_add(dictionary,i_Battery_cycle_phase.Id_read,&i_Battery_cycle_phase);
	dict_add(dictionary,i_Input_voltage_AC_IN.Id_read,&i_Input_voltage_AC_IN);
	dict_add(dictionary,i_Input_current_AC_IN.Id_read,&i_Input_current_AC_IN);
	dict_add(dictionary,i_Output_voltage_AC_OUT.Id_read,&i_Output_voltage_AC_OUT);
	dict_add(dictionary,i_Output_power_AC_OUT.Id_read,&i_Output_power_AC_OUT);
	dict_add(dictionary,i_Battery_Voltage.Id_read,&i_Battery_Voltage);
	dict_add(dictionary,i_Battery_Current.Id_read,&i_Battery_Current);
	dict_add(dictionary,i_soc_value_battery.Id_read,&i_soc_value_battery);
	dict_add(dictionary,i_State_of_Health.Id_read,&i_State_of_Health);
	dict_add(dictionary,i_Battery_Voltage_Charge_limit.Id_read,&i_Battery_Voltage_Charge_limit);
	dict_add(dictionary,i_Battery_Voltage_Discharge_limit.Id_read,&i_Battery_Voltage_Discharge_limit);
	dict_add(dictionary,i_Battery_Current_Charge_limit.Id_read,&i_Battery_Current_Charge_limit);
	dict_add(dictionary,i_Battery_Current_Discharge_limit.Id_read,&i_Battery_Current_Discharge_limit);
	dict_add(dictionary,i_Battery_Current_Charge_recommanded.Id_read,&i_Battery_Current_Charge_recommanded);
	dict_add(dictionary,i_Battery_Current_Discharge_recommanded.Id_read,&i_Battery_Current_Discharge_recommanded);
	dict_add(dictionary,Transfer_relay_allowed.Id_read,&Transfer_relay_allowed);
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

	// end of the code for the service

		printf("hiiii : the algorithm launched\n");

		Parameter_dic = dict_new();
		init_dic(Parameter_dic);
		dict_print(Parameter_dic);

  	MQTTClient  client;
  	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

		printf("initialize the var for mqtt\n");
  	int rc;
		//crée le client mqtt pour le charger inverter --------------------------------------------
  	MQTTClient_create(&client, ADDRESS, CLIENT_ID,MQTTCLIENT_PERSISTENCE_NONE, NULL);

		printf("create the first mqtt client\n");

  	conn_opts.keepAliveInterval = 20;
  	conn_opts.cleansession = 1;
  	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

		printf("set the first callback methods\n");

  	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  	{
      	printf("Failed to connect, return code %d\n", rc);
      	exit(EXIT_FAILURE);
  	}
		Dictionary *head = Parameter_dic;
		while(Parameter_dic->head != NULL && Parameter_dic->tail != NULL){
			MQTTClient_subscribe(client, Parameter_dic->head->value->Id_read, QOS);
			printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
						 , Parameter_dic->head->value->Id_read, CLIENT_ID, QOS);
					Parameter_dic = Parameter_dic->tail;
		}
		Parameter_dic = head;
		//printf("print the dictionnary after the subscribe");
		dict_print(Parameter_dic);
		//--------------------------------------------------------------------------------------

		//Crée le client mqtt pour la batterie ---------------------------------------------------

  	timer_init();
  	Time_init();


  	printf("--------------------------------EXEC----------------------------------\n");
		printf("\n========== launch connection ==========\n");

  	printf("Hey !\n");
		printf("\n========== Launch Initialisation ==========\n");
		//Init(client_charger);
  	do
  	{
		//printf("IN MAIN LOOP\n");
   		if(algo_on == 1 )
   		{
			algo_on = 0;

			printf("\n===========read =============\n");
			Read_p(client);

			printf("\n========== Algorithme =========\n");
			Algo();
			printf("\n========== Ecriture des parametres sur Onduleur ==========\n");

	   	Write_p(client);
	 		// Commented for test json
		}
	} while(1);
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
	return rc;
}

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
#define int_temp_s 10
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


//---------- DECLARATRION OF VAR -----------------------------------------------
 /*------------------------ Declaration parametres -------------------------------*/
	/*----------------- Battery Charge ----------------------------*/
	t_param Charger_allowed = 		{1125, 2, 5, 1,   1}; //PCO OK
	t_param Battery_Charge_current_DC = 	{1138, 2, 5, 6,  0};//step 1A; PCO OK on init à zero
	t_param Force_new_cycle = 		{1142, 2, 5, 5,   0}; // PCO param Inutile ?
	t_param Use_dynamic_comp = 		{1608, 2, 5, 1,   0}; // PCO param Inutile ?
	t_param Floating_voltage = 		{1140, 2, 5, 6,  61.5};//step 0.1 , PCO en mode inspect il faudrait lire avant 7061
	t_param Force_floating = 		{1467, 2, 5, 5,   0}; // PCO param inutile
	t_param Voltage_1_start_new_cycle = 	{1143, 2, 5, 6,  45};//step 0.10 PCO parm inutile
	t_param Voltage_2_start_new_cycle = 	{1145, 2, 5, 6,  44};//step 0.10 PCO parm inutile
	t_param Time_1_under_voltage = 		{1144, 2, 5, 6, 180};//step 1 PCO Parm inutile
	t_param Time_2_under_voltage = 		{1146, 2, 5, 6, 180};//step 10 PCO Parm inutile
	t_param Absorption_phase_allowed = 	{1155, 2, 5, 1,   1}; // PCO OK
	t_param Absorption_voltage = 		{1156, 2, 5, 6,  61.5};//step 0.1 //PCO en mode inspect il faudrait lire avant 7061
	t_param Absorption_duration = 		{1157, 2, 5, 6,   0};//step 0.25h PCO : V. Penas suggere zero
	t_param End_absorption_current = 	{1158, 2, 5, 1,   1}; // PCO parm inutile
	t_param Current_to_quit_absorption = 	{1159, 2, 5, 6,   2};//step 1A // PCO parm inutile
	t_param Maximal_freq_absorption = 	{1160, 2, 5, 1,   1};//PCO parm inutile
	t_param Delay_beetween_absorption =	{1161, 2, 5, 6,   1};//step 1h , PCO parm inutile
	t_param Equalization_phase_allowed = 	{1163, 2, 5, 1,   0}; // PCO Parm inutile
	t_param Reduced_floating_allowed = 	{1170, 2, 5, 1,   0}; // PCO Parm inutile
	t_param Periodic_absorption_allowed = 	{1173, 2, 5, 1,   0}; // PCO Parm inutile
	t_param Over_Voltage_Value_allowed = 	{1121, 2, 5, 6,  63}; // PCO OK
	t_param Voltage_After_Over_allowed =    {1122, 2, 5, 6,  61.5}; // PCO OK
	t_param Under_Voltage_Value_allowed =	{1108, 2, 5, 6, 41.0}; // PCO suggere 15 x 2.9V limite de declenchement au lieu de 41V
	t_param Voltage_After_Under_allowed = 	{1110, 2, 5, 6, 55.5}; // PCO Pram inutile
	t_param Bat_Temp_Com_Coef = {1139,2,5,6,0}; // PCO OK
	/*-----------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11
	/* ----------------------------Inverter -----------------------------*/
	t_param Inverter_Allowed = 	  	  {1124, 2, 5, 1,   1};//step 1 PCO OK
	t_param Inverter_Output_Voltage = {1286, 2, 5, 6, 230};//step 1 PCO : parma inutile ?
	t_param Inverter_Frequency = 	    {1112, 2, 5, 6,  50};//step 0.1 PCO : parma inutile ?
	t_param Batt_priority_source =    {1296, 2, 5, 1, 0}; //PCO : parma inutile ?
	/*------------------------------------------------------------------*/
	/*------------------------ Standby and turn on ------------------*/
	t_param Standby_level = 	    {1187, 2, 5, 6, 20};//step 10% , à tester PCO : parma inutile ?
	t_param Standby_nbr_pulse =  {1188, 2, 5, 6,  1};//step 1 //PCO : parma inutile ?
	/*--------------------------------------------------------------*/
	/*-------------------- AC-IN and transfert ---------------------*/
	t_param Transfer_relay_allowed = {1128, 2, 5, 1, 1}; // PCO param inutile ?
	t_param Smart_boost_allowed = 	 {1126, 2, 5, 1, 1}; // PCO OK voir alogo ensuite
	t_param MAX_current_of_AC_IN =   {1107, 2, 5, 6, 35};//step 1 PCO : OK
	/*-------------------------------------------------------------*/
	//-------------- Multi Xtender System ----------------------
	t_param Multi_inverters_allowed = {1461, 2, 5, 1, 0}; // PCO param inutile
	t_param Integral_mode = 	  	    {1283, 2, 5, 1, 0}; // PCO param inutile
	/*---------------------------------------------------------*/
	//-------------- Multi Xtender System ----------------------
	t_param Soc_Backup = {6062, 2, 5, 6, 80}; // PCO OK
	t_param Soc_Inject = {6063, 2, 5, 6, 80}; // PCO OK corriger 6063 !
	/*---------------------------------------------------------*/
	/*----------------- Grid Feeding --------------------------*/
	t_param Grid_Feeding_allowed = 			     {1127, 2, 5, 1,    1}; // PCO OK selon algo
	t_param Max_Grid_Feeding_current = 		   {1523, 2, 5, 6,   30};//step 0.2 // PCO Rajouter test et diminuer 30 à 2?
	t_param Battery_voltage_forced_feeding = {1524, 2, 5, 6,    40};//step 0.1 à tester //PCO  Parm inutile ?
	t_param Start_Time_forced_injection =	   {1525, 2, 5, 5,   10};//step : 1, regarder avec Flo le format PCO pourquoi ?
	t_param Stop_Time_forced_injection =	   {1526, 2, 5, 5,   10};//step : 1, regarder avec Flo le format PCO pourquoi ?
	t_param Use_defined_phase_shift_curve  = {1610, 2, 5, 1,    1}; // PCO Parm inutile
	t_param Cos_phi_P_0 = 					         {1622, 2, 5, 6,    0};//step 0.01, à voir PCO parm inutile
	t_param Cos_phi_P_100 = 				         {1624, 2, 5, 6,  0.1};//step 0.01, à voir PCO parm inutile
	t_param Cos_phi_at_P_1613 = 			       {1623, 2, 5, 6, 0.05};//step 0.01 à voir PACO parm inutile
	t_param Power_of_second_cos_phi_point =  {1613, 2, 5, 6,   50};//step 5% of Pnom, à voir parm inutile
/*------------------------------------------------------------------------------*/

/*--------------------------- Xtender_Infos --------------------------------------*/
	t_param i_Battery_Voltage_Studer = 				 {3000, 1, 1,  6, Not_Value};
	t_param i_Wanted_battery_charge_current =  {3004, 1, 1,  6, Not_Value};
	t_param i_Battery_Charge_current = 			   {3005, 1, 1,  6, Not_Value};
	t_param i_State_of_charge = 				       {3007, 1, 1,  6, Not_Value};
	t_param i_Battery_cycle_phase = 			     {3010, 1, 1, 11, Not_Value}; // 0 = invalid , 1 = Bulk , 2 = absorption , 3 = equalize , 4 = floating , 5 = Reduce floating , 6 = Periodic absorption , 7 = mixing , 8 = forming
	t_param i_Input_voltage_AC_IN = 			     {3011, 1, 1,  6, Not_Value};
	t_param i_Input_current_AC_IN = 			     {3012, 1, 1,  6, Not_Value};
	t_param i_Input_power_AC_IN = 				     {3013, 1, 1,  6, Not_Value};
	t_param i_Input_current_limit = 			     {3017, 1, 1,  6, Not_Value};
	t_param i_Input_current_limit_reached = 	 {3018, 1, 1, 11, Not_Value}; // 0 = Off , 1 = on
	t_param i_Boost_active = 					         {3019, 1, 1, 11, Not_Value};// 0 = Off , 1 = on
	t_param i_Output_voltage_AC_OUT = 			   {3021, 1, 1,  6, Not_Value};
	t_param i_Output_current_AC_OUT = 			   {3022, 1, 1,  6, Not_Value};
	t_param i_Output_power_AC_OUT = 			     {3023, 1, 1,  6, Not_Value};
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

	/*--------------------------- Batterry info --------------------------------------*/


// PCO
// psmax = courant_decharge_limite IAdc (param 7064) x tension_dc (param 7000)
// a voir s'il faut pas remplacer par des valeur moyenne de 1 min, autre param disponibles

	t_param i_Battery_Voltage = 	{7000, 1, 1, 6, 0};
	t_param i_Battery_Current = 	{7001, 1, 1, 6, 0};
	t_param i_soc_value_battery = {7047, 1, 1, 6, 0};
	t_param i_State_of_Health = 	{7057, 1, 1, 6, 0};

	t_param i_Battery_Voltage_Charge_limit = 	 				{7061, 1, 1, 6, 0};
	t_param i_Battery_Voltage_Discharge_limit =	 			{7062, 1, 1, 6, 0};
	t_param i_Battery_Current_Charge_limit = 	 				{7063, 1, 1, 6, 0};
	t_param i_Battery_Current_Discharge_limit =				{7064, 1, 1, 6, 0};
	t_param i_Battery_Current_Charge_recommanded = 		{7065, 1, 1, 6, 0};
	t_param i_Battery_Current_Discharge_recommanded = {7066, 1, 1, 6, 0};

// PCO : En mode inspect control, soit Xtender master
  // On peut lire la tension de charge à appliquer sur {1140} et {1156} (={7061}) (floating et absorption voltages)
  // de même courant de charge  -	décharge :
  //{1523}[Aac] < {7064}[Adc]*{7000}[Udc]/230[Vac]
  //charge {1138} [Adc] < {7065} [Adc] )


/*----------------------------------------------------------------------------------*/
	//Format : BOOL(1) FORMAT(2) ENUM(3) ERROR(4) INT32(5) FLOAT(6) STRING(7) DYNAMIC(8) BYTE_STREAM(9) LONG_ENUM(10) SHORT_ENUM(11)
  t_param Parameters_saved_in_flash_memory = {1550, 2, 5, 1, 0};

double meters[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0,0.0,0.0,0.0};

float Bat_Capacite_nominale = 10.8;
float Bat_Capacite_disponible =10.8; // avec un state of health de 100% batterie pleine et SOCmin=0%, 10.8 justeune valeu par défaut

float Eb=0;
float SOCmin = 20;
float SOCmax = 80;
float Cve = 0;
float SOC = 0;
float Ps = 0;
float Ps_opti = 0;
float Pr = 0;
float Kg = 0;
float Kr = 0;
float Ppv = 0;
float Pl = 0;
float Psmax = 0;
// PCO
float Psmax_charge =0;
float Psmax_discharge =0;
float Prmax = 0;
float Kc = 0;
float Pb = 0;
float Plsec = 0;
float Kp = 0;
float Time_now = 0;
float Temps_routine = 0;
float Epv = 0;
float El = 0;
float Km = 0;
float Ke = 0;
float K_g = 0;
int algo_on = 1;
float Kg_1 = 0;
float Kg_2 = 0;
int Time_now_i;
int charge_on = 0;
float AC_IN_INJ_I_Des = 0;
int STATE;int INJ=0;

int Time_old;
time_t my_time;
struct tm * timeinfo;

//------------------------------------------------------------------------------
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

/*void Battery_management(float P_s,MQTTClient* client)
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
	if(Ps >= 0)
	{
		printf("CHARGE DE LA BATTERIE\n");
		printf("========== Battery en charge ==========\n");
		INJ=0;
		//Bloquer l'injection;
		Grid_Feeding_allowed.Value = false; //Param 1127;
		//Activer l'onduleur;
		Inverter_Allowed.Value = true; //Param 1124;
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

		printf("==========================================\n");

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

				//Régulation du ratio de puissance Pbatt vs Pres via Iac AC-IN;
				Max_Grid_Feeding_current.Value = fabs(Ps) / i_Input_voltage_AC_IN.Value;
				if(Max_Grid_Feeding_current.Value >= (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value)
			 	Max_Grid_Feeding_current.Value = (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value;													//value dynamic for discharge
				printf("Max grid feeding : %f\n",Max_Grid_Feeding_current.Value);
				printf("Battery current discharge limi : %f\n",i_Battery_Current_Discharge_limit.Value);
				if(Max_Grid_Feeding_current.Value >= 34.0) Max_Grid_Feeding_current.Value = 34.0; // 8.6 pour 2 kW

				//Temps d'injection;
				Start_Time_forced_injection.Value = Time_now; //L?injection débuterai dans 1 minute
				Stop_Time_forced_injection.Value = Start_Time_forced_injection.Value +1;
				//L'injection s'arrêtera après le nouveau cycle;
				printf("==========================================\n");
			}
			else
			{
				printf("ALIMENTATION DES CHARGES SECURISEES\n");
				INJ=0;
	     	printf("========== Puissance insuff pour injecter, alim le charge securisé ==========\n");
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

				printf("==========================================\n");

		}
	}
	if(Soc_Backup.Value > SOCmax) {
		Soc_Backup.Value = SOCmax;
		Soc_Inject.Value = SOCmax;
	}
	else if (Soc_Backup.Value < SOCmin) {
		Soc_Backup.Value = SOCmin;
		Soc_Inject.Value = SOCmin;
	}
	Write_bat(&Soc_Backup,client);
	Write_bat(&Soc_Inject,client);
}*/
/*----------------------------------------------------------------------------------------------*/

void State_management(int state){
	switch (state){
	case 1:
		printf("case 1 of state management ----------------\n");
		//Bloquer l'onduleur;
		Inverter_Allowed.Value = false; //Param 1124;
		//Activer la charge
		Charger_allowed.Value = true; //Param 1125;
		//Activation du SmartBoost;
		Smart_boost_allowed.Value = true; //Param 1126;
		//Activation l'injection;
		Grid_Feeding_allowed.Value = true; //Param 1127;

		Max_Grid_Feeding_current.Value = fabs(Ps) / i_Input_voltage_AC_IN.Value;
		if(Max_Grid_Feeding_current.Value >= (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value)
		Max_Grid_Feeding_current.Value = (i_Battery_Current_Discharge_limit.Value*i_Battery_Voltage.Value)/i_Input_voltage_AC_IN.Value;													//value dynamic for discharge
		if(Max_Grid_Feeding_current.Value >= 34.0) Max_Grid_Feeding_current.Value = 34.0; // 8.6 pour 2 kW

		printf("Max grid feeding : %f\n",	Max_Grid_Feeding_current.Value );
		//L'injection s'arrêtera après le nouveau cycle;
		Force_new_cycle.Value = true;
		printf("end case 1 :--------------------------------------\n");
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		printf("case 2,3, 4 and 5 of state management ----------------\n");
		Inverter_Allowed.Value = false; //Param 1124;
		//Activer la charge
		Charger_allowed.Value = true; //Param 1125;
		//Activation du SmartBoost;
		Smart_boost_allowed.Value = true; //Param 1126;
		//Activation l'injection;
		Grid_Feeding_allowed.Value = false; //Param 1127;


		Battery_Charge_current_DC.Value = fabs(Ps)/i_Battery_Voltage.Value;
		if(Battery_Charge_current_DC.Value > i_Battery_Current_Charge_limit.Value)
	 	Battery_Charge_current_DC.Value = i_Battery_Current_Charge_limit.Value;
		if(Battery_Charge_current_DC.Value > 200) //limit des disjoncteur des batteries
		Battery_Charge_current_DC.Value = 190;

		Force_new_cycle.Value = true;

		printf("Charge current of bat : %f\n",Battery_Charge_current_DC.Value);

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
	Ppv = Ppv;//Ppv_1[(int)Time_now];
	Pl = Pl;//Pl_1[(int)Time_now];
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
				Ps =  Ps;
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
      	Ps = 5; //couvre l'autoconsomation de la batterie'
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
    	printf("STATE = 1;\n");
			Ps = Pb;
			Pr = 0;
			//Kr = Ppv / Pl;
		}
		//Ps = Ps * 3;
		State_management(STATE);
		if (fabs(Pr) >= Prmax)
		{
			//Kr = Prmax / Pr;
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
    /* EXEMPLE CODE MQTT*/
  char* payloadptr;
  cJSON * root;

  UNUSED(payloadptr);
  UNUSED(context);
  UNUSED(topicLen);

	bool I_paquet = false;
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

	select_meters = parse_energy_meters(payload);
	//	printf("payload display : %s\n", payload);

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
		parse_studer_message(payload,data);
	}
	else if (strstr(payload,XCOM_ID_BAT) != NULL){
			root = cJSON_Parse(payload);
			//printf("receive paylaod with xcom bat  : %s\n",payload);
			//printf("palyoad parsed : %s\n",cJSON_Print(root));

			cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
      UNUSED(data);
			parse_batt_message(payload,data);
	}
	 MQTTClient_freeMessage(&message);
	 MQTTClient_free(topicName);
	return 1;
}


int main()
{
	/*
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

  	MQTTClient  client_charger;
  	MQTTClient_connectOptions conn_opts_charger = MQTTClient_connectOptions_initializer;

		MQTTClient client_bat;
  	MQTTClient_connectOptions conn_opts_bat = MQTTClient_connectOptions_initializer;
		printf("initialize the var for mqtt\n");
  	int rc_charger;
		//crée le client mqtt pour le charger inverter --------------------------------------------
  	MQTTClient_create(&client_charger, ADDRESS, CLIENTID_CHARGER,MQTTCLIENT_PERSISTENCE_NONE, NULL);

		printf("create the first mqtt client\n");

  	conn_opts_charger.keepAliveInterval = 20;
  	conn_opts_charger.cleansession = 1;
  	MQTTClient_setCallbacks(client_charger, NULL, connlost, msgarrvd, delivered);

		printf("set the first callback methods\n");

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
		MQTTClient_create(&client_bat, ADDRESS, CLIENTID_BAT,MQTTCLIENT_PERSISTENCE_NONE, NULL);
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
			Algo();
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

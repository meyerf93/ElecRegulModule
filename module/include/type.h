//Fichier : conversion.h
//Projet  : Pilotage-micro réseau - NeighborHub
//But	    : Connection and mqtt method for charger inverter
//Auteurs : Florian Meyer.
//Version : 2.1
//Date 	  : 13.07.2018
//Status  : test

//#include "connection.h"

typedef struct Parametres {
			   uint32_t User_ref;
			   int 		Object_type;
			   uint16_t Proprety_id;
			   int 		Format;
			   float 	Value;
			   }t_param;

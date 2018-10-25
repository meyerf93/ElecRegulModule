
#ifndef _TYPE_H
#define _TYPE_H
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
			   uint32_t User_ref;
			   int 		Object_type;
			   uint16_t Proprety_id;
			   int 		Format;
			   float 	Value;
				 char Id_read[60];
				 char Id_write[60];
			   }t_param;
#endif

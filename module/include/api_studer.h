#ifndef INCLUDE_API_STUDER_H_
#define INCLUDE_API_STUDER_H_
/**
 * Copyright 2016 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project:		HEIA-FR /Open MicropGrid Management System
 *
 * Abstract: 	Header file containing the information of API for studer protocol
 *
 * @author 	Florian Meyer
 * @version 1.0
 * Date: 	28.06.2016
 */

#include "scom_data_link.h"
#include "scom_property.h"
#include "connection.h"

/**
 * Struct containing the data used for the Studer protocol
 */

struct studer_data{
	scom_format_t format;
	uint16_t data_16;
	uint32_t data_32;
	float data_f;
};

/**
 * Realised by Studer
 * \brief example code to exchange a frame and print possible error on standard output
 *
 * \param frame an initialized frame configured for a service
 * \return a possible error that occurred or SCOM_NO_ERROR
 */
scom_error_t exchange_frame(scom_frame_t* frame,struct connection* connect);

/**
 * Method to forge the frame and exchange it
 *@param frame		structure who contain the frame information of Studer protocol
 *@param property	structure who contain the property information of Studer protocol
 *@param buffer		buffer who contain the frame encoded
 *@param src_add	address of the source
 *@param dst_add	address if the dest
 *@param obj_type	value of the object type
 *@param obj_id		value of the object id
 *@param prop_id	value of the property id
 *@param value		contain the value to send
 *@param format		value of the format of the data
 *@param serv 		contain the value of the service used
 *@return 			return -1 if error else 0
 */
int forge_studer_frame(scom_frame_t* frame,scom_property_t* property,char* buffer,
size_t buffer_length,uint32_t src_add, uint32_t dst_add,int obj_type,
uint32_t obj_id, uint16_t prop_id,char* value, int format,int serv);

/**
 * Methode to decode frame, detect error and add data of the frame
 *@param frame		structure who contain the frame information of Studer protocol
 *@param property	structure who contain the property information of Studer protocol
 *@param ret_value	buffer who contain the string of return value
 *@param data		structure who contain data in different right format
 *@param serv		contain the value of the service used
 *@param prop_id	value of the property id
 *@param format		value of the format of the data
 *@return			return 1- if error else 0
 */

int decode_studer_frame(scom_frame_t* frame,scom_property_t* property,char* ret_value,struct studer_data* data,int serv,uint16_t prop_id,int format);

#endif



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
 * Abstract: 	srouce file containing the information of API for studer protocol
 *
 * @author 	Florian Meyer
 * @version 2.0
 * Date: 	30.11.2016
 */

#include <stdio.h>
#include <string.h>
#include <scom_property.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "api_studer.h"
#include "connection.h"
#include "scom_data_link.h"
#include "scom_property.h"

#define SCOM_SERVICE_HEADER_SIZE 2
#define SCOM_PROPERTY_HEADER_SIZE 8

#define SCOM_PROPERTY_HEADER_OFFSET (SCOM_FRAME_HEADER_SIZE + SCOM_SERVICE_HEADER_SIZE)
#define SCOM_PROPERTY_VALUE_OFFSET (SCOM_FRAME_HEADER_SIZE + SCOM_SERVICE_HEADER_SIZE + SCOM_PROPERTY_HEADER_SIZE)

/**
 *Methode to encode string to correct data on the frame
 *@param buffer_frame	buffer who the data encoded is stored
 *@param value			data to encode
 *@param forma			format of the encoded data
 *@return				return the length of the buffer
 */
size_t encode_data(char* buffer_frame,const char* value, int format);

/**
 * Method to decode the result buffer
 *@param buffer_frame	contain the value to decode
 *@param value			string who the data encoded is stored
 *@param buffer_length	contain the length of the buffer_frame
 *@param format			format to data to decode
 *@return				return structure containing data decoded
 */
struct studer_data decode_data(const char* buffer_frame, char* value,size_t buffer_length,int format);


/**
 * Method to convert data to the right format
 *@param value_buffer	contain data formated
 *@param value			contain data to fromat
 *@param format			contain the format to convert
 *@return 				return the number of byte of the format (1,2 or 4)
 */
size_t encode_data(char * value_buffer,const char* value, int format){

	size_t n_byte = 0;
	uint16_t data_16 = 0;
	uint32_t data_32 = 0;
	float data_f = 0.0;

	switch(format){
		case SCOM_FORMAT_BOOL:
				if(strcmp(value,"true")==0){
					value_buffer[0] = 0x1;
				}
				else{
					value_buffer[0] = 0x0;
				}
				n_byte = 1;
				break;
		case SCOM_FORMAT_LONG_ENUM:
				data_32 = atoi(value);
				scom_write_le32(value_buffer,data_32);
				n_byte = 4;
				break;
		case SCOM_FORMAT_SHORT_ENUM:
		case SCOM_FORMAT_ENUM:
				data_16 = atoi(value);
				scom_write_le16(value_buffer,data_16);
				n_byte = 2;
				break;
		case SCOM_FORMAT_INT32:
				data_32 = atoi(value);
				scom_write_le32(value_buffer,data_32);
				n_byte = 4;
				break;
		case SCOM_FORMAT_FLOAT:
				data_f = atof(value);
				scom_write_le_float(value_buffer,data_f);
				n_byte = 4;
				break;
		default:
			fprintf(stderr,"ERROR : can't convert this format\n");
	}

	/*printf("number of byte send : %ld\n",n_byte);
	printf("value of the buffer : ");
	for(unsigned int i =0;i<n_byte;++i){
		printf("%x ",value_buffer[i]);
	}
	printf("\n");*/
	return n_byte;
}

/**
 * Method to decode the result buffer
 *@param buffer_frame	contain the value to decode
 *@param value			string who the data encoded is stored
 *@param buffer_length	contain the length of the buffer_frame
 *@param format			format to data to decode
 *@return				return structure containing data decoded
 */
struct studer_data decode_data(const char* buffer_frame, char* value,size_t buffer_length,int format){

		struct studer_data studer_data ={0,0,0,0.0};
		char temp_str[200];

		uint16_t data_16 = 0;
		uint32_t data_32 = 0;
		float data_f = 0.0;

		strcpy(value,"value : ");
		//printf("value : ");

		switch(format){
			case SCOM_FORMAT_BOOL:
					if(buffer_frame[0] == 0x1){
						strcat(value,"true");
						//printf("true");
						studer_data.data_16=0x1;
					}
					else{
						strcat(value,"false");
						//printf("false");
						studer_data.data_16=0x0;
					}
					//strcat(value,", format : BOOl\n");
					//printf(", format : BOOL\n");
					studer_data.format = SCOM_FORMAT_BOOL;

					break;
			case SCOM_FORMAT_LONG_ENUM:
			case SCOM_FORMAT_SHORT_ENUM:
			case SCOM_FORMAT_ENUM:
					if(buffer_length == 2){
						data_16 = scom_read_le16(buffer_frame);
						//printf("%d",data_16);
						sprintf(temp_str,"%d",data_16);
						strcat(value,temp_str);
						studer_data.data_16=data_16;
					}
					else{
						data_32 = scom_read_le32(buffer_frame);
						//printf("%d",data_32);
						sprintf(temp_str,"%d",data_32);
						strcat(value,temp_str);
						studer_data.data_32=data_32;
					}
					//printf(", format : ENUM\n");
					strcat(value,", format : ENUM\n");
					studer_data.format = SCOM_FORMAT_ENUM;
					break;
			case SCOM_FORMAT_INT32:
					data_32 = scom_read_le32(buffer_frame);
					//printf("%d",data_32);
					sprintf(temp_str,"%d",data_32);
					strcat(value,temp_str);
					studer_data.data_32 = data_32;
					//printf(", format : INT32\n");
					strcat(value,", format : INT32\n");
					studer_data.format = SCOM_FORMAT_INT32;
					break;
			case SCOM_FORMAT_FLOAT:
					data_f = scom_read_le_float(buffer_frame);
					//printf("%.2f",data_f);
					sprintf(temp_str,"%.2f",data_f);
					strcat(value,temp_str);
					studer_data.data_f = data_f;
					//printf(", format : FLOAT\n");
					strcat(value,", format : FLOAT\n");
					studer_data.format = SCOM_FORMAT_FLOAT;
					break;
			default:
				//printf("\n");
				fprintf(stderr,"ERROR : can't display this format\n");
		}

		return studer_data;
}

/**
 * Realized by Studer
 * \brief example code to exchange a frame and print possible error on standard output
 *
 * \param frame an initialized frame configured for a service
 * \return a possible error that occurred or SCOM_NO_ERROR
 */
scom_error_t exchange_frame(scom_frame_t* frame,struct connection* connect ){
    size_t byte_count;

    connect->connection_clear(connect);
	connect->buf_tx_length = scom_frame_length(frame);
	//printf("value of frame :");
	for(unsigned int i=0;i<frame->buffer_size;++i){
		//printf("%d\n",frame->buffer[i]);
		connect->buf_tx[i]=frame->buffer[i];
	};
	//printf(": end of frame\n");

    /* send the request on the com port */
    byte_count = connect->connection_write(connect);
    if(byte_count != connect->buf_tx_length) {
        printf("error when writing to the com port\n");
        return SCOM_ERROR_STACK_PORT_WRITE_FAILED;
    }

    /* reuse the structure to save space */
    scom_initialize_frame(frame, frame->buffer, frame->buffer_size);

    /* clear the buffer for debug purpose */
    memset(frame->buffer,0, frame->buffer_size);
	for(unsigned int i=0;i<frame->buffer_size;++i){
		connect->buf_rx[i]=frame->buffer[i];
	}
	connect->start_read = 0;
	connect->buf_rx_length = SCOM_FRAME_HEADER_SIZE;

    /* Read the fixed size header.
       A platform specific handling of a timeout mechanism should be
       provided in case of the possible lack of response */

    byte_count = connect->connection_read(connect);

    if(byte_count != SCOM_FRAME_HEADER_SIZE) {
        printf("error when reading the header from the com port\n");
        return SCOM_ERROR_STACK_PORT_READ_FAILED;
    }


	for(unsigned int i=0;i<connect->buf_rx_length;++i){
		frame->buffer[i]=connect->buf_rx[i];
	}

    /* decode the header */
    scom_decode_frame_header(frame);
    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link header decoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }
    /* read the data part */
	connect->start_read = SCOM_FRAME_HEADER_SIZE;
	connect->buf_rx_length = scom_frame_length(frame)-SCOM_FRAME_HEADER_SIZE;
    byte_count = connect->connection_read(connect);
    if(byte_count != (scom_frame_length(frame) - SCOM_FRAME_HEADER_SIZE)) {
        printf("error when reading the data from the com port\n");
        return SCOM_ERROR_STACK_PORT_READ_FAILED;
    }

    /* decode the data */
	connect->start_read = 0;
	for(unsigned int i=SCOM_FRAME_HEADER_SIZE;i<connect->buf_rx_length+SCOM_FRAME_HEADER_SIZE;++i){
		frame->buffer[i]=connect->buf_rx[i];
	}
    scom_decode_frame_data(frame);
    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link data decoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    return SCOM_ERROR_NO_ERROR;
}


/**
 * Main method of the api
 */
int forge_studer_frame(scom_frame_t* frame,scom_property_t* property,char* buffer,
					   size_t buffer_length,uint32_t src_add,uint32_t dst_add,
					   int obj_type,uint32_t obj_id,uint16_t prop_id,char* value,
					   int format,int serv){

//----------------------------------------------------------------------
	//try to initilize the serial port
	//Initialization of the frame --------------------------------------
	scom_initialize_frame(frame, buffer, buffer_length);
	scom_initialize_property(property, frame);

	frame->src_addr = src_add;     /* set the src address */
	frame->dst_addr = dst_add;   /* set the dst adress */

	property->object_type = obj_type;  /* set the object type */
	property->object_id = obj_id;  /* set the object id */
	property->property_id = prop_id; /* set the propety id */

	property->value_length = encode_data(property->value_buffer,value,format); /* set the data to send */

	if(serv == SCOM_READ_PROPERTY_SERVICE){
		scom_encode_read_property(property);
	}
	else if (serv ==SCOM_WRITE_PROPERTY_SERVICE){
		scom_encode_write_property(property);
	}
	else{
		fprintf(stderr,"ERROR : bad number for the service\n");
		return -1;
	}


	if(frame->last_error != SCOM_ERROR_NO_ERROR) {
		printf("data link frame encoding failed with error %d\n", (int) frame->last_error);
		printf("size of buffer : %ld\n",frame->buffer_size);
		return frame->last_error;
	}

	scom_encode_request_frame(frame);
	if(frame->last_error != SCOM_ERROR_NO_ERROR) {
	    printf("read property frame encoding failed with error %d\n", (int) frame->last_error);
	    return -1;
	}

	return 0;
}
//-------------------------------

	/* do the exchange of frames */
	/*if(exchange_frame(frame,connect)!= SCOM_ERROR_NO_ERROR) {
    	close_serial_port();
	    return -1;
	}*/
/**
 * Method to decode frame, detect error and add data of the frame
 */

int decode_studer_frame(scom_frame_t* frame,scom_property_t* property,
						char* ret_value,struct studer_data* data,int serv,
						uint16_t prop_id,int format){
	/* reuse the structure to save space */
	scom_initialize_property(property, frame);

	/* decode the read property service part */

	if(serv == SCOM_READ_PROPERTY_SERVICE){
		scom_decode_read_property(property);
	}
	else if (serv ==SCOM_WRITE_PROPERTY_SERVICE){
		scom_decode_write_property(property);
	}
	else{
		fprintf(stderr,"ERROR : bad number for the service\n");
		return -1;
	}
	if(frame->last_error != SCOM_ERROR_NO_ERROR) {
	    printf("read property decoding failed with error %d\n", (int) frame->last_error);
	    return -1;
	}
	uint16_t format_send = 0;
	if(prop_id == 0x8){
		format_send = SCOM_FORMAT_ENUM;
	}
	else{
		format_send = SCOM_FORMAT_DYNAMIC;
	}

	/* check the size of the frame */
	unsigned int length_ref = 0;
	switch(format_send){
		case SCOM_FORMAT_BOOL:
				length_ref = 1;
				break;
		case SCOM_FORMAT_FORMAT:
		case SCOM_FORMAT_ENUM:
		case SCOM_FORMAT_ERROR:
				length_ref = 2;
				break;
		case SCOM_FORMAT_INT32:
		case SCOM_FORMAT_FLOAT:
				length_ref = 4;
				break;
		case SCOM_FORMAT_STRING:
		case SCOM_FORMAT_DYNAMIC:
		case SCOM_FORMAT_BYTE_STREAM:
				length_ref = 5;
				break;
	}
	if((property->value_length != length_ref) && length_ref != 5){
		printf("invalid property data response size\n");
	    return -1;
	}

	*data = decode_data(property->value_buffer,ret_value,property->value_length,format);

    return 0;
}

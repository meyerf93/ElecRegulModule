#ifndef INCLUDE_CONNECTION_H_
#define INCLUDE_CONNECTION_H_
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
 * Abstract: 	Header file containing the information of connection to the
 * 				Xcom-232i product from Studer
 *
 * @author 	Florian Meyer
 * @version 2.0
 * Date: 	30.11.2016
 */
#define SERIAL_PORT	1
#define SOCKET_IPV4 2

#include <termios.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>

#define SERIAL 1
#define IPV4 2

/**
 * Structur containing the parameter of the type of connection
 */
struct connection_type {
	int type;						//contain the type of connection
	char parameter[200];			//contain the parameter for the connection
	struct termios serial;			//structure containing parameter for serial
	struct sockaddr_in socket;		//structure containing parameter for IP
};

/**
 * Struct containing the parameter and method of the connection
 */
struct connection {	
	struct connection_type connect;	//contain informations for the connection type
	int fd;							//file descriptor to acces to the conneciton
	int start_read;					//start of reading the buffer
	char buf_tx[1024];				//buffer for write information 
	char buf_rx[1024];				//buffer for read information
	size_t buf_tx_length;			//contain the number of byte inside buf_tx
	size_t buf_rx_length;			//contain the number of byte inside buf_rx
    size_t(*connection_read) (struct connection *self);
									//function pointer for the reading method
	int(*connection_write) (struct connection *self);
									//function pointer for the writing method
	void(*connection_clear) (struct connection *self);
									//function pointer to clear the connection
};									



//Method for serial conenction ---------------------------------------------------
/**
 * Method to initialize a serial port
 * this method use ttyUSB0 and locked parameter
 *@param driver_path	contain the path to the serial port 
 *@param serial_struct  contain the structure of the connection
 *@return 				return -1 if error else 0	
 */
int initialize_serial_port(const char* driver_path,struct connection* serial_struct);

/**
 * Method to clear the buffer of the serial port
 */
void clear_serial_port(struct connection* sellf);

/**
 * Method to read the data on the serial port
 *@param buffer 	pointer to the buffer who save data
 *@param byte_count number of bytes to read
 *@return 			return the number of byte read
 */
size_t read_serial_port(struct connection* self);

/**
 * Method to write the data on the serial port
 *@param buffer		pointer to the buffer who the data to write is save
 *@param byte_count	number of byte to write
 *@return 			return -1 if error else return the number of byte write
 */
int write_serial_port(struct connection* self);

/**
 * Method to close properly the serial port
 */
void close_serial_port(struct connection* serial_struct);
//----------------------------------------------------------------------------------

//Methode for socket_ipv4 connection -----------------------------------------------
/**
 * Method to initialize a socket ipv4 connection
 * this method use ttyUSB0 and locked parameter
 * @return 	return 0 if error else return the fd of the port	
 */
int initialize_socket_ipv4(const char* addres,struct connection* socket_struct);

/**
 * Method to clear the buffer of the serial port
 */
void clear_socket_ipv4(struct connection* sellf);

/**
 * Method to read the data on the serial port
 *@param buffer 	pointer to the buffer who save data
 *@param byte_count number of bytes to read
 *@return 			return the number of byte read
 */
size_t read_socket_ipv4(struct connection* self);

/**
 * Method to write the data on the serial port
 *@param buffer		pointer to the buffer who the data to write is save
 *@param byte_count	number of byte to write
 *@return 			return -1 if error else return the number of byte write
 */
int write_socket_ipv4(struct connection* self);

/**
 * Method to close properly the serial port
 */
void close_socket_ipv4(struct connection* socket_struct);
#endif





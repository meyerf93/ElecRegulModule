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
 * Abstract: 	srouce file containing the information for the connection to
 *				the module Xcon-232i
 *
 * @author 	Florian Meyer
 * @version 2.0
 * Date: 	30.11.2016
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <scom_property.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "connection.h"

/**
 * \brief configure the serial port
 * \return zero in case of an error
 */
int initialize_serial_port(const char* driver_path,struct connection* serial_struct){
	serial_struct->fd = 0;
	serial_struct->connect.type = SERIAL_PORT;

	serial_struct->connection_read = &read_serial_port;
	serial_struct->connection_write = &write_serial_port;
	serial_struct->connection_clear = &clear_serial_port;
	serial_struct->start_read = 0;

	//try to open a serial port
	serial_struct->fd = open(driver_path,O_RDWR|O_NOCTTY|O_NDELAY);
    if(serial_struct->fd == -1){
		fprintf(stderr,"ERROR : can't open serial port fd\n");
		return -1;
    }
	strcpy(serial_struct->connect.parameter,driver_path);
    //try to configure the serialport
	struct termios tty;
	if(tcgetattr(serial_struct->fd,&tty)!=0){
		fprintf(stderr,"ERROR : can't read the parameters of serial port\n");
		return -1;
	}
	//inputs flags config
	tty.c_iflag &= ~(IGNBRK|BRKINT|ICRNL|INLCR|PARMRK|INPCK|ISTRIP|IXON);

	//output flags config
	tty.c_oflag = 0;

	//line processing config
	tty.c_lflag &= ~(ECHO | ECHONL|ICANON|IEXTEN|ISIG);

	//control flag config
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= (CS8|PARENB);

	//set speed
	if(cfsetospeed(&tty,B38400) < 0 || cfsetispeed(&tty,B38400) <0){
		fprintf(stderr,"ERROR : can't configure the baudrate\n");
		return -1;
	}

	if(tcsetattr(serial_struct->fd,TCSANOW,&tty) < 0){
		fprintf(stderr,"ERROR : cant't set the configuration to the serial port");
		return -1;
	}
	serial_struct->connect.serial = tty;
	//serial_struct->connect.socket = NULL;

	//printf("Open correctly the serial port ttyUSB0 with baudrate 38400 and 8e1\n");
    return 0;
}

/**
 * \brief empty the rx and tx buffer before a new exchange
 */
void clear_serial_port(struct connection* self){

	//clear the rx/tx buffer
	for(unsigned int i=0;i<sizeof(self->buf_tx);++i){
		self->buf_tx[i]=0x0;
		self->buf_rx[i]=0x0;
	}
}

/**
 * \brief read in a blocking way on the serial port
 *
 * This function must implement the proper timeout mechanism.
 * * \return number of byte read
 */
size_t read_serial_port(struct connection* self){

	//try to read the data on the port
	size_t byte_read = 0;
	while(byte_read !=self->buf_rx_length){
		byte_read = read(self->fd,&self->buf_rx[self->start_read],self->buf_rx_length);
		/*printf("while loop read, byte_read : %ld, buf_rx_length : %ld, fd: %d, rx : %s,start_read : %d\n",byte_read,self->buf_rx_length,self->fd,self->buf_rx,self->start_read);
		char estr[100] = {[0]=0,};
		strerror_r(errno,estr,sizeof(estr)-1);
		fprintf(stderr,"EROOR : %s\n",estr);*/
		usleep(100000); // must have
	}

	/*printf("value of read buffer : ");
	for(unsigned int i=self->start_read;i<self->buf_rx_length+self->start_read;++i){
		printf("0x%x ",self->buf_rx[i]);
	}
	printf("\n");
	printf("total byte read : %ld\n",byte_read);*/
	return byte_read;
}

/**
 * \brief write in a blocking way on the serial port
 *
 * This function must implement the proper timeout mechanism.
 * \return number of byte written
 */
int write_serial_port(struct connection* self){
	int byte_send = 0;
	//try to send a single byte
	if(self->buf_tx_length > 0){
		while(byte_send!=(int)self->buf_tx_length){
			byte_send += write(self->fd,self->buf_tx,self->buf_tx_length);
			/*printf("while loop write, byte_send : %d, total : %ld\n",byte_send,self->buf_tx_length);*/
			if (byte_send == -1){
				fprintf(stderr,"ERROR : error during sending byte\n");
				return -1;
			}
			usleep(100000);//MUST HAVE
		}
		//printf("byte send : %d\n",byte_send);
	}
	else{
		fprintf(stderr,"ERROR : can't send a null or negative number of byte\n");
		return -1;
	}
	/*printf("value of write buffer : ");
	for(unsigned int i=0;i<self->buf_tx_length;++i){
		printf("0x%x ",self->buf_tx[i]);
	}
	printf("\n");
	printf("total byte write : %d\n",byte_send);
	printf("%d byte is send to port ttyUSB0 successfully\n",byte_send);*/
	return byte_send;
}

/**
 * \brief close the serial port even if initialize_serial_port() failed
 */
void close_serial_port(struct connection* self){
	//try to close the serial port
	close(self->fd);
}

/**
 * Initialze the socket structur for a connection with ipV4
 * return 0 if error else return fd of the socket
 */
 int initialize_socket_ipv4(const char* addresse, struct connection* socket_struct){
	socket_struct->fd = 0;
	socket_struct->connect.type = SOCKET_IPV4;

	socket_struct->connection_read = &read_socket_ipv4;
	socket_struct->connection_write = &write_socket_ipv4;
	socket_struct->connection_clear = &clear_socket_ipv4;
	socket_struct->start_read = 0;

	//create a socket ipv4
	socket_struct->fd = socket(AF_INET, SOCK_STREAM, 0);
	socket_struct->connect.socket.sin_family = AF_INET;
	inet_pton(AF_INET,addresse, &socket_struct->connect.socket.sin_addr);
	socket_struct->connect.socket.sin_port = htons(4001);
	connect (socket_struct->fd, (struct sockaddr *) &socket_struct->connect.socket,sizeof(socket_struct->connect.socket));

	clear_socket_ipv4(socket_struct);


	//try to open a socket ipv4

    if(socket_struct->fd == -1){
		fprintf(stderr,"ERROR : can't open serial port fd\n");
		return -1;
    }
	strcpy(socket_struct->connect.parameter,addresse);

	//socket_struct->connect.socket = NULL;

	 //printf("Open correctly the serial port ttyUSB0 with baudrate 38400 and 8e1\n");
    return 0;
 }


 void clear_socket_ipv4(struct connection* self){
	 //clear the rx/tx buffer
	for(unsigned int i=0;i<sizeof(self->buf_tx);++i){
		self->buf_tx[i]=0x0;
		self->buf_rx[i]=0x0;
	}
 }

 size_t read_socket_ipv4(struct connection* self){
	//try to read the data on the s
	size_t byte_read = 0;
	while(byte_read !=self->buf_rx_length){
		byte_read = read(self->fd,&self->buf_rx[self->start_read],self->buf_rx_length);
		/*printf("while loop read, byte_read : %ld, buf_rx_length : %ld, fd: %d, rx : %s,start_read : %d\n",byte_read,self->buf_rx_length,self->fd,self->buf_rx,self->start_read);
		char estr[100] = {[0]=0,};
		strerror_r(errno,estr,sizeof(estr)-1);
		fprintf(stderr,"EROOR : %s\n",estr);*/
		//usleep(100000); // must have
	}

	/*printf("value of read buffer : ");
	for(unsigned int i=self->start_read;i<self->buf_rx_length+self->start_read;++i){
		printf("0x%x ",self->buf_rx[i]);
	}
	printf("\n");
	printf("total byte read : %ld\n",byte_read);*/
	return byte_read;
 }

 int write_socket_ipv4(struct connection* self){
	int byte_send = 0;
	//try to send a single byte
	if(self->buf_tx_length > 0){
		while(byte_send!=(int)self->buf_tx_length){
			byte_send += write(self->fd,self->buf_tx,self->buf_tx_length);
			/*printf("while loop write, byte_send : %d, total : %ld\n",byte_send,self->buf_tx_length);*/
			if (byte_send == -1){
				fprintf(stderr,"ERROR : error during sending byte\n");
				return -1;
			}
			//usleep(100000);//MUST HAVE
		}
		//printf("byte send : %d\n",byte_send);
	}
	else{
		fprintf(stderr,"ERROR : can't send a null or negative number of byte\n");
		return -1;
	}
	/*printf("value of write buffer : ");
	for(unsigned int i=0;i<self->buf_tx_length;++i){
		printf("0x%x ",self->buf_tx[i]);
	}
	printf("\n");
	printf("total byte write : %d\n",byte_send);
	printf("%d byte is send to port ttyUSB0 successfully\n",byte_send);*/
	return byte_send;
 }

 void close_socket_ipv4(struct connection* socket_struct){
	 //try to close the socket
	 close(socket_struct->fd);
 }

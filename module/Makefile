# Project:		SLC / HEIA-FR /PS6 Pilotage-micro réseau - SOlar Decathlon 2017
#
# Abstract: 	Makefile program for Charger/inverter from Studer-Innotec
#
# @author 	Davbide ... Michele .... Florian Meyer
# @version 	2.0
# Date: 	18.05.2016

#TOOLCHAIN_PATH=~/workspace/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/
#TOOLCHAIN=$(TOOLCHAIN_PATH)arm-linux-gnueabihf-


JSON_C_DIR=/usr/local
CFLAGS += -I$(JSON_C_DIR)/include/json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c

CFLAGS+=-Wall -Wextra -g -c -Og -MD -std=gnu11 -Iinclude
CFLAGSDEB+=$(CFLAGS) -DDEBUG
CFLAGSLIB+=$(CFLAGS) -fPIC
CC=$(TOOLCHAIN)gcc
LD=$(TOOLCHAIN)gcc

OBJDIR = target
SRCDIR = src
LIBDIR = lib
MQTT = -lpaho-mqtt3c
JSON = -ljson-c
#object,src and exec for the test of studer lib
#on mac -I/usr/local/include -L/usr/local/lib option and -ljson-c option

OBJECTS =$(OBJDIR)/cJSON.o $(OBJDIR)/scom_data_link.o  $(OBJDIR)/scom_property.o $(OBJDIR)/Main_test_v2.0.o -lm
SRCS = $(SRCDIR)/cJSON.c $(SRCDIR)/scom_data_link.c .c $(SRCCDIR)/scom_property.c $(SRCDIR)/Main_test_v2.0.c
EXEC = $(OBJDIR)/test_alog
MAC = -I/usr/local/include -L/usr/local/lib -ljson-c

LIB = $(LIBDIR)/API_Studer.so $(LIBDIR)/libpaho-mqtt3c.so
default: $(EXEC)

all : $(EXEC) lib

lib: $(OBJECTS)
	@mkdir -p $(LIBDIR)
	$(CC) $(LDFLAGS) -o $(LIB) -shared $(OBJECTS)

mkdir :
	@mkdir -p target

$(EXEC): mkdir $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJECTS) $(MQTT) $(JSON)

$(OBJDIR)/%o: $(SRCDIR)/%c
	@mkdir -p target
	$(CC) $(CFLAGSLIB) $< -o $@

clean:
	rm -rf target

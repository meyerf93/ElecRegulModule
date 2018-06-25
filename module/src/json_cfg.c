#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cfg.h"

/*saving the value corresponding to boolean, double, integer and strings into struct config*/
void save_json_value(struct config * cfg, json_object *jobj, char* key){
	printf("in sage json\n");
	enum json_type type;
	type = json_object_get_type(jobj); /*Getting the type of the json object*/
	int ival = 0; const char* sval = ""; bool bval = false; double dval = 0;
	printf("get type of the object\n");
	switch (type) {
		case json_type_boolean:
			printf("boolean value\n");
			bval = json_object_get_boolean(jobj);
			break;
		case json_type_double:
			printf("double value\n");
			dval = json_object_get_double(jobj);
			printf("%f\n",dval);
			break;
		case json_type_int:
			printf("int value\n");
			ival = json_object_get_int(jobj);
                	break;
		case json_type_string:
			printf("string value\n");
			sval = json_object_get_string(jobj);
	            	printf(sval);
	              	printf("\n");
			break;
		default: break;
		pintf(sval);
		printf("\n");	
}


//{ "dttp": null, "data": 1400.049927, "t": "2017-05-18T13:21:51Z", "id": "knx1\/:1.1.9\/:\/power.1" }
	if	(strcmp(key,	"dttp")				== 0) strncpy(cfg->dttp, sval,sizeof(cfg->dttp)-1);
	else if	(strcmp(key,	"data")				 	== 0) cfg->data= dval;
	else if	(strcmp(key,	"t")		== 0) strncpy(cfg->t, sval,sizeof(cfg->t)-1);
	else if	(strcmp(key,	"id")	== 0) strncpy(cfg->id, sval,sizeof(cfg->id)-1);
	else 	fprintf(stderr,	"ERROR: Key from config file not recognized \n");
}

/* get element in the array and extract it for saving it in config struct*/
void json_parse_array(struct config * cfg, json_object *jobj, char *key) {
	printf("in json parse array\n");
	void json_parse(struct config * cfg, json_object * jobj); /*Forward Declaration*/
	enum json_type type;

	json_object *jarray = jobj; /*Simply get the array*/
	if(key) {
		jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
	}

	int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
	int i;
	json_object * jvalue;

	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
		type = json_object_get_type(jvalue);
		if (type == json_type_array) {
			json_parse_array(cfg, jvalue, NULL);
		}
		else if (type != json_type_object) {
			save_json_value(cfg, jobj, key);
		}
		else {
			json_parse(cfg, jvalue);
		}
	}
}

/*Parsing the json object*/
void json_parse(struct config * cfg, json_object * jobj) {
	printf("in json parse \n");
	enum json_type type;
	json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
		type = json_object_get_type(val);
		switch (type) {
			case json_type_boolean:
			case json_type_double:
			case json_type_int:
			case json_type_string:
				save_json_value(cfg, val, key);
				break;
			case json_type_object:
				json_parse(cfg, json_object_object_get(jobj, key));
				break;
			case json_type_array:
				json_parse_array(cfg, jobj, key);
				break;
			default:
				break;
		}
	}
} 

struct config cfg_json_parse(const char* config_filename) {
/*	// open config file
	int fd = open (config_filename, O_RDONLY);
	if (fd == -1) {
dval		perror ("can't open configuration file\n");
		exit(1);
	}
*/
	// create a default configuration
	struct config cfg = default_config;
/*	struct stat stat;
	fstat (fd, &stat);
	off_t size = stat.st_size+1;
	char* string = malloc (size);
	ssize_t nr = read (fd, string, size);
	if (nr == -1) {
		perror ("can't read configuration file\n");
		exit(1);
	}
	// close file
	close (fd);
*/
	// parse file
	printf("try to parse the message\n");
	json_object * jobj = json_tokener_parse(config_filename);//string);     
	json_parse(&cfg, jobj);

	return cfg;
}


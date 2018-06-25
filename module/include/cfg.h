/*
 * cfg.h
 *
 *  Created on: Mar 17, 2016
 *      Author: David Gremaud
 */
#pragma once
#ifndef SRC_CFG_H_
#define SRC_CFG_H_

//{ "dttp": null, "data": 1400.049927, "t": "2017-05-18T13:21:51Z", "id": "knx1\/:1.1.9\/:\/power.1" }
;
struct config {
	char dttp[100];
	double data;
	char t[100];
	char id[100];
};

/* default struct config */
static const struct config default_config = {
	.dttp = "bonjour",
	.data = 123.4,
	.t    = "mille heure 24",
        .id   = "test d id"
};

/* functions for retrieved config data from config files */
extern struct config cfg_json_parse (const char* config_filename);

#endif /* SRC_CFG_H_ */

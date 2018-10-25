
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "type.h"

typedef struct {
    char *key;
    t_param *value;
} KVPair;

typedef struct Dictionary_t {
    KVPair *head;
    struct Dictionary_t *tail;
} Dictionary;

Dictionary* dict_new();
void dict_print(Dictionary *dictionary);
void dict_add(Dictionary *dictionary, const char *key, t_param *value);
int dict_has(Dictionary *dictionary, const char *key);
t_param* dict_get(Dictionary *dictionary, const char *key);
void dict_remove(Dictionary *dictionary, const char *key);
void dict_free(Dictionary *dictionary);

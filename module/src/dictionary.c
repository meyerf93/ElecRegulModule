#include "dictionary.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "type.h"

Dictionary* dict_new() {
    Dictionary *dictionary = (Dictionary*)malloc(sizeof(Dictionary));
    assert(dictionary != NULL);
    dictionary->head = NULL;
    dictionary->tail = NULL;
    return dictionary;
}
void dict_print(Dictionary *dictionary){
  printf("dictionnary : ");
  while(dictionary != NULL){
      printf("value : %s\n",dictionary->head->value->Id_read);
      dictionary = dictionary->tail;
  }
  printf("\n");
}
void dict_add(Dictionary *dictionary, const char *key, t_param *value) {
    if(dict_has(dictionary, key))
            dict_remove(dictionary, key);
    if (dictionary->head != NULL) {
        while(dictionary->tail != NULL) {
            dictionary = dictionary->tail;
        }
        Dictionary *next = dict_new();
        dictionary->tail = next;
        dictionary = dictionary->tail;
    }
    int key_length = strlen(key) + 1;
    dictionary->head = (KVPair*)malloc(sizeof(KVPair));
    assert(dictionary->head != NULL);
    dictionary->head->key = (char *)malloc(key_length * sizeof(char));
    assert(dictionary->head->key != NULL);
    strcpy(dictionary->head->key, key);
    dictionary->head->value = value;
}

int dict_has(Dictionary *dictionary, const char *key) {
    if (dictionary->head == NULL)
        return 0;
    while(dictionary != NULL) {
        if(strncmp(dictionary->head->key, key,strlen(key) + 1) == 0)
            return 1;
        dictionary = dictionary->tail;
    }
    return 0;
}

t_param* dict_get(Dictionary *dictionary, const char *key) {
    if (dictionary->head == NULL)
        return 0;
    while(dictionary != NULL) {
        printf("try to get the right parameter : %s\n",key);
        printf("key : %s, dictionary key : %s\n",key,dictionary->head->key);
        printf("Compare the two key and pass to the next one\n");
        if(strcmp(dictionary->head->key, key,strlen(key) + 1) == 0)
            return dictionary->head->value;
        dictionary = dictionary->tail;
        printf("key : %s, dictionary key : %s\n",key,dictionary->head->key);
    }
    return NULL;
}

void dict_remove(Dictionary *dictionary, const char *key) {
    if (dictionary->head == NULL)
        return;
    Dictionary *previous = NULL;
    while(dictionary != NULL) {
        if(strncmp(dictionary->head->key, key,strlen(key) + 1) == 0) {
            if(previous == NULL) {
                free(dictionary->head->key);
                dictionary->head->key = NULL;
                if (dictionary->tail != NULL) {
                    Dictionary *toremove = dictionary->tail;
                    dictionary->head->key = toremove->head->key;
                    dictionary->tail = toremove->tail;
                    free(toremove->head);
                    free(toremove);
                    return;
                }
            }
            else {
                previous->tail = dictionary->tail;
            }
            free(dictionary->head->key);
            free(dictionary->head);
            free(dictionary);
            return;
        }
        previous = dictionary;
        dictionary = dictionary->tail;
    }
}

void dict_free(Dictionary *dictionary) {
    if(dictionary == NULL)
        return;
    free(dictionary->head->key);
    free(dictionary->head);
    Dictionary *tail = dictionary->tail;
    free(dictionary);
    dict_free(tail);
}

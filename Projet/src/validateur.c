
#define _GNU_SOURCE //to avoid c99 related warnings
#include "validateur.h"
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

int validateur_JSON(char *data) {
    char datasave[strlen(data)+1];
    strcpy(datasave, data);
    int i = 0;
    int is_data = 0;
    int guillemets = 0;
    int accolades = 0;
    int crochets = 0;
    int length = strlen(data);
    for (i; i<length; ++i) {  
        switch(data[i]){
            case '"':
                guillemets++;
                break;
            case '{':
                accolades++;
                break;
            case '}':
                accolades++;
                break;
            case '[':
                crochets++;
                break;
            case ']':
                crochets++;
                break;
        }
    }
    if(guillemets%2 != 0 || crochets%2 != 0 || accolades%2 != 0){
        printf("JSON non valide\n");
        return 0;
    }
    if (data[0] != '{' && data[length-1] != '}')
    {
        printf("JSON non valide\n");
        return 0;
    }
    for (i = 0; i < length; i++)
    {
        if (data[i] == '"')
        {
            memmove(data, data+i, strlen(data));
            i = i + valide_guillemets(data);
            strcpy(data, datasave);
            if(is_data == 0){
                memmove(data, data+i, strlen(data));
                i = i + valide_deux_points(data);
                strcpy(data, datasave);
                is_data = 1;
            }
            else {
                memmove(data, data+i, strlen(data));
                i = i + valide_virgule(data);
                strcpy(data, datasave);
                is_data = 0;
            }
        }
        if (data[i] == '[')
        {
            memmove(data, data+i, strlen(data));
            i = i + valide_guillemets(data);
            strcpy(data, datasave);
            if(is_data == 0){
                memmove(data, data+i, strlen(data));
                i = i + valide_deux_points(data);
                strcpy(data, datasave);
                is_data = 1;
            }
            else {
                memmove(data, data+i, strlen(data));
                i = i + valide_virgule(data);
                strcpy(data, datasave);
                is_data = 0;
            }
        }
    }
    

    return 1;
}

int valide_guillemets(char *data){
    int i = 0;
    int count = strlen(data);
    if (data[0] != '"')
    {
        return 0;
    }
    for (i = 0; i < count; i++)
    {
        if (data[i] == '"')
        {
            return i;
        }
    }
    return 1;
}

int valide_deux_points(char *data){
    int i = 0;
    int count = strlen(data);
    if (data[0] != ' ')
    {
        return 0;
    }
    if (data[1] != ':')
    {
        return 0;
    }
    if (data[2] != ' ')
    {
        return 0;
    }
    
    return 2;
}

int valide_virgule(char *data){
    if (data[0] != ',')
    {
        return 0;
    }
    if (data[1] != ' ')
    {
        return 0;
    }
    return 2;
}

int valide_datas(char *data){}
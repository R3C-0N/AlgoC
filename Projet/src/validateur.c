
#define _GNU_SOURCE //to avoid c99 related warnings
#include "validateur.h"
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

int validateur_JSON(char *data) {
    char datasave[strlen(data)+1];
    strcpy(datasave, data);
    int i;
    int is_data = 0;
    int guillemets = 0;
    int accolades = 0;
    int crochets = 0;
    int length = strlen(data);
    int validateur = 0;
    for (i = 0; i<length; ++i) {  
        switch(data[i]){
            case '"':
                guillemets++;
                break;
            case '{':
                accolades++;
                break;
            case '}':
                accolades--;
                break;
            case '[':
                crochets++;
                break;
            case ']':
                crochets--;
                break;
        }
    }
    if(guillemets%2 != 0 || crochets != 0 || accolades != 0){
        printf("JSON non valide: %i\n", __LINE__);
        return 0;
    }
    if (data[0] != '{' && data[length-1] != '}')
    {
        printf("JSON non valide: %i\n", __LINE__);
        return 0;
    }
    for (i = 0; i < length; i++)
    {
        if (data[i] == '"')
        {
            memmove(data, data+i, strlen(data));
            validateur = valide_guillemets(data);
            if (validateur == 0)
            {
                printf("JSON non valide: %i\n", __LINE__);
                return 0;
            }
            i = i + validateur + 1;
            strcpy(data, datasave);
            if(is_data == 0){
                memmove(data, data+i, strlen(data));
                validateur = valide_deux_points(data);
                if (validateur == 0)
                {
                    printf("JSON non valide: %i\n", __LINE__);
                    return 0;
                }
                i = i + validateur;

                strcpy(data, datasave);
                is_data = 1;
            }
            else {
                memmove(data, data+i, strlen(data));
                validateur = valide_virgule(data);
                if (validateur == 0)
                {
                    printf("JSON non valide: %i\n", __LINE__);
                    return 0;
                }
                i = i + validateur - 1;
                strcpy(data, datasave);
                is_data = 0;
            }
        }
        if (data[i] == '[')
        {
            memmove(data, data+i, strlen(data));
            validateur = valide_crochets(data);
            if (validateur == 0)
            {
                printf("JSON non valide: %i\n", __LINE__);
                return 0;
            }
            i = i + validateur + 1;
            strcpy(data, datasave);
            
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
    for (i = 1; i < count; i++)
    {
        if (data[i] == '"')
        {
            return i;
        }
    }
    return 1;
}

int valide_deux_points(char *data){
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

int valide_datas(char *data){
    char datasave[strlen(data)+1];
    strcpy(datasave, data);
    int i;
    int length = strlen(data);
    int validateur = 0;
    for (i = 0; i < length; i++)
    {
        if (data[i+1] == ' ' && data[i+2] == ']')
        {
            /* FIN DU TABLEAU */
            return i+2;
        }
        
        if (i!=0)
        {
            memmove(data, data+i+1, strlen(data));
            data[strlen(data)-1]='\0';
            
            validateur = valide_virgule(data);
            if(validateur == 0){
                return 0;
            }
            i = i + validateur + 1;
            strcpy(data, datasave);
            memmove(data, data+i, strlen(data));
        }
        else
        {
            memmove(data, data+i+2, strlen(data));
        }
        
        data[strlen(data)-1]='\0';
        validateur = valide_guillemets(data);
        if(validateur == 0){
            return 0;
        }
        if (i==0)
        {
            i = validateur + 1;
        }
        else
        {
            i = i + validateur - 1;
        }
        strcpy(data, datasave);
    }
    
}

int valide_crochets(char *data){
    int compteur = 1;
    int i;
    int length = strlen(data);
    for (i = 0; i < length-1; i++)
    {
        if (data[i] == '['){
            compteur++;
        } else if(data[i] == ']'){
            compteur--;
        }
        if (compteur<0)
        {
            return 0;
        }
        if (compteur==0)
        {
            break;
        }
    }
    data[i+1]="\0";
    int valideDatas = valide_datas(data);
    if (valideDatas != 0)
    {
        return valideDatas;
    }
    
    return 0;
}
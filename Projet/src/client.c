/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"
#include "validateur.h"

int envoie_nom(int socketfd){
  char data[1024];
  char hostname[128];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  //envoie nom machine
  gethostname(hostname, 128);
  strcpy(data, "nom: ");
  strcat(data, hostname);

  char encoded_data[512];
  encode_JSON(data, encoded_data);

  int write_status = write(socketfd, encoded_data, strlen(encoded_data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(encoded_data, 0, sizeof(encoded_data));

  // lire les données de la socket
  int read_status = read(socketfd, encoded_data, sizeof(encoded_data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message encodé: %s\n", encoded_data);

  decode_JSON(encoded_data, data);
  printf("Message recu: %s\n", data);
 
  return 0;
}

/* 
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd) {
 
  char data[1024];
  char encoded_data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));
  memset(encoded_data, 0, sizeof(encoded_data));


  // Demandez à l'utilisateur d'entrer un message
  char message[100];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, 1024, stdin);
  strcpy(data, "message: ");
  message[strlen(message)-1] = '\0';
  strcat(data, message);
  
  encode_JSON(data, encoded_data);

  int write_status = write(socketfd, encoded_data, strlen(encoded_data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(encoded_data, 0, sizeof(encoded_data));


  // lire les données de la socket
  int read_status = read(socketfd, encoded_data, sizeof(encoded_data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message encodé: %s\n", encoded_data);

  decode_JSON(encoded_data, data);
  int length = strlen(data);
  int i;
  for (i = 0; i < length; i++)
  {
    if (data[i]==',')
    {
      data[i]=' ';
    }
    
  }
  
  printf("Message recu: %s\n", data);
 
  return 0;
}

int envoie_operateur_numeros(int socketfd, char *argv[]) {
 
  char data[1024];
  int i;
  size_t size = sizeof(argv);

  // la réinitialisation de l'ensemble des données
  memset(data, 0, size);

  strcpy(data, "calcule: ");
  for (i = 2;i <= size; i++)
  {
    strcat(data, argv[i]);
    if(i != size)
      strcat(data, ",");
  }

  // lire les données de la socket
  char encoded_data[512];
  encode_JSON(data, encoded_data);

  int write_status = write(socketfd, encoded_data, strlen(encoded_data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
}

int envoie_couleurs(int socketfd, char *argv[]) {
  char data[1024];
  memset(data, 0, sizeof(data));
  int nb = atoi(argv[2]);
  

  if (nb > 30)
  {
    perror("Nombre de couleurs trop grand");
    exit(EXIT_FAILURE);
  }


  strcpy(data, "couleurs: ");
  
  int i;
  for (i = 3; i < nb + 3; ++i)
  {
  printf("ERREUR: %i\n", __LINE__);
  printf("DATA: %s\n", argv[i]);
    strcat(data, argv[i]);
    strcat(data, ",");
  printf("ERREUR: %i\n", __LINE__);
  }

  char encoded_data[512];
  encode_JSON(data, encoded_data);

  int write_status = write(socketfd, encoded_data, strlen(encoded_data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int envoie_balises(int socketfd, char *argv[]) {
  char data[1024];
  char encoded_data[1024];
  memset(data, 0, sizeof(data));
  memset(encoded_data, 0, sizeof(encoded_data));
  int nb = atoi(argv[2]);

  if (nb > 30)
  {
    perror("Nombre de balises trop grand");
    exit(EXIT_FAILURE);
  }

  strcpy(data, "balises: ");
  int i;
  for (i = 3; i < nb + 3; ++i)
  {
    strcat(data, argv[i]);
    strcat(data, ",");
  }

  encode_JSON(data, encoded_data);

  int write_status = write(socketfd, encoded_data, strlen(encoded_data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

void analyse(char *pathname, char *data, char *nb) {
  //compte de couleurs
  int intNb = atoi(nb);
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "analyse: ");
  char temp_string[intNb];
  sprintf(temp_string, "%d,", intNb);
  if (cc->size < intNb) {
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);
  
  //choisir intNb couleurs
  for (count = 1; count < intNb+1 && cc->size - count >0; count++) {
    if(cc->compte_bit ==  BITS32) {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size-count].c.rouge,cc->cc.cc32[cc->size-count].c.vert,cc->cc.cc32[cc->size-count].c.bleu);
    }
    if(cc->compte_bit ==  BITS24) {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size-count].c.rouge,cc->cc.cc32[cc->size-count].c.vert,cc->cc.cc32[cc->size-count].c.bleu);
    }
    strcat(data, temp_string);
  }

  //enlever le dernier virgule
  data[strlen(data)-1] = '\0';
}

int envoie_images(int socketfd, char *nb, char *pathname) {
  char data[1024];
  char encoded_data[1024];
  int intNb = atoi(nb);
  memset(data, 0, sizeof(data));
  memset(encoded_data, 0, sizeof(encoded_data));

  if(intNb>30){
    perror("Nombre de couleurs demande trop grand");
    exit(EXIT_FAILURE);
  }

  analyse(pathname, data, nb);
  
  encode_JSON(data, encoded_data);

  int write_status = write(socketfd, encoded_data, strlen(encoded_data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int encode_JSON(char* data, char* encoded_data){
  printf("Message à encoder: %s\n", data);
  
  char * decoder = strtok(data, ": ");
  sprintf(encoded_data, "{\"code\" : \"%s\", \"valeurs\" : [ ", decoder);

  decoder = strtok(NULL, ":");
  memmove(decoder, decoder+1, strlen(decoder));
  decoder = strtok(decoder, ",");

  while(decoder != NULL){
    sprintf(encoded_data, "%s\"%s\", ", encoded_data, decoder);
    decoder = strtok(NULL, ",");
  }
  encoded_data[strlen(encoded_data)-1] = '\0';
  encoded_data[strlen(encoded_data)-1] = ' ';
  
  strcat(encoded_data, "]}");

  printf("Message encodé envoyé: %s\n", encoded_data);
  return 1;
}

int decode_JSON(char *data, char *decoded_data){
  char data2[strlen(data)+1];
  strcpy(data2, data);

  // Isoler code d'instruction
  char * decoder_code = strtok(data, ",");
  decoder_code = strtok(decoder_code, " : ");
  decoder_code = strtok(NULL, " : ");

  // Supprimer les guillemets
  memmove(decoder_code, decoder_code+1, strlen(decoder_code));
  decoder_code[strlen(decoder_code)-1]='\0';

  // Ajout de l'instruction a la chaine décodé
  sprintf(decoded_data, "%s: ", decoder_code);

  // Isoler les valeurs
  char * decoder_valeurs = strtok(data2, "[");
  decoder_valeurs = strtok(NULL, "[");

  // Isoler chaque valeurs entre elles
  char * decoder_valeur = strtok(decoder_valeurs, "\", \"");
  while(decoder_valeur!=NULL) {
    sprintf(decoded_data, "%s%s,", decoded_data, decoder_valeur);
    decoder_valeur = strtok(NULL, "\", \"");
  }
  decoded_data[strlen(decoded_data)-4]='\0';
  return 1;
}

int main(int argc, char **argv) {
  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if ( connect_status < 0 ) {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }
  

  if (!strcmp(argv[1],"message")) {
    envoie_recois_message(socketfd);
  }

  else if (!strcmp(argv[1], "nom")) {
    envoie_nom(socketfd);
  }

  else if (!strcmp(argv[1], "calcule")) {
    envoie_operateur_numeros(socketfd, argv);
  }

  else if (!strcmp(argv[1], "couleurs")) {
    printf("ARGV: %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
    envoie_couleurs(socketfd, argv);
  }

  else if (!strcmp(argv[1], "balises")) {
    envoie_balises(socketfd, argv);
  }

  else {
    envoie_images(socketfd, argv[1], argv[2]);
  }

  close(socketfd);
}

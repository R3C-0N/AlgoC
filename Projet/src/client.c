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

/* 
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_nom(int socketfd){
  char data[1024];
  char hostname[128];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  //envoie nom machine
  gethostname(hostname, 128);
  strcpy(data, "nom: ");
  strcat(data, hostname);

  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
}

int envoie_recois_message(int socketfd) {
 
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // Demandez à l'utilisateur d'entrer un message
  char message[100];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, 1024, stdin);
  strcpy(data, "message: ");
  strcat(data, message);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);
 
  return 0;
}

int envoie_operateur_numeros(int socketfd, char *argv[]) {
 
  char data[1024];
  char hostname[128];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  strcpy(data, "calcule: ");
  strcat(data, argv[2]);
  strcat(data, " ");
  strcat(data, argv[3]);
  strcat(data, " ");
  strcat(data, argv[4]);

  // lire les données de la socket
  int write_status = write(socketfd, data, strlen(data));
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
    strcat(data, argv[i]);
    strcat(data, " ");
  }

  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int envoie_balises(int socketfd, char *argv[]) {
  char data[1024];
  memset(data, 0, sizeof(data));
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
    strcat(data, " ");
  }

  int write_status = write(socketfd, data, strlen(data));
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
  int intNb = atoi(nb);
  memset(data, 0, sizeof(data));

  if(intNb>30){
    perror("Nombre de couleurs demande trop grand");
    exit(EXIT_FAILURE);
  }

  analyse(pathname, data, nb);


  printf("%s\n", data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
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
  //envoie_recois_message(socketfd);

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

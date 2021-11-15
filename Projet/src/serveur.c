/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serveur.h"

void plot(char *data) {
  //Extraire le compteur et les couleurs RGB 
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot\n");
  int count = 0;
  int n;
  char *saveptr = NULL;
  char *str = data;
  char strNb[] = {data[9], data[10]};
  int nb = atoi(strNb);
  fprintf(p, "set terminal x11\n");
  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top %s colors'\n", strNb);
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
  while(1) {
    char *token = strtok_r(str, ",", & saveptr);
    if (token == NULL) {
      break;
    }
    str=NULL;
    if (count == 0) {
      n = atoi(token);
    }
    else {
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      printf("%d %s\n", count, token);
      printf("0 0 10 %d %d 0x%s\n", (count-1)*360/nb, count*360/nb, token+1);
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count-1)*360/nb, count*360/nb, token+1);
    }
    count++;
  }
  fprintf(p, "e\n");

  printf("Plot: FIN\n");
  pclose(p);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data) {
  int data_size = write (client_socket_fd, (void *) data, strlen(data));
      
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

int renvoie_nom(int client_socket_fd, char *data){
  int data_size = write (client_socket_fd, (void *) data, strlen(data));
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

struct Calcul
{
    char operateur;
    float  nombre1;
    float  nombre2;
    float resultat;
};

int renvoie_calcul(int client_socket_fd, char *data){
  struct Calcul calcul;
  get_calcule_from_parameters(data, &calcul);

  printf("calcule: %.2f\n", calcul.resultat);

  int data_size = write (client_socket_fd, (void *) data, strlen(data));
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

int get_calcule_from_parameters(char *parameters, struct Calcul* resultat){
  char * tableau = strtok(parameters, " ");

  tableau = strtok(NULL, " ");
  (*resultat).operateur = tableau[0];

  tableau = strtok(NULL, " ");
  (*resultat).nombre1 = atof(tableau);

  tableau = strtok(NULL, " ");
  (*resultat).nombre2 = atof(tableau);

  switch((*resultat).operateur) {
    case '-':
      (*resultat).resultat = (*resultat).nombre1 - (*resultat).nombre2;
    break;
    case '/':
      (*resultat).resultat = (*resultat).nombre1 / (*resultat).nombre2;
    break;
    case '*':
      (*resultat).resultat = (*resultat).nombre1 * (*resultat).nombre2;
    break;
    case '+':
    default:
      (*resultat).resultat = (*resultat).nombre1 + (*resultat).nombre2;
    break;
  }
 }

 int recois_couleurs(int client_socket_fd, char *data){
  FILE *fp;
  printf("couleurs: ");

  char * tableau = strtok(data, " ");
  tableau = strtok(NULL, " ");
  int nb = atoi(tableau);
  char couleurs[1000];
  int i = 0;

  while(tableau != NULL){
    strcat(couleurs, tableau);
    strcat(couleurs, " ");
    printf("%s ", tableau);
    tableau = strtok(NULL, " ");
    i++;
  }
  printf("\n");

  fp = fopen("./tmp/couleurs.txt", "w+");
  fputs(couleurs, fp);
  fclose(fp);

  int data_size = write (client_socket_fd, (void *) data, strlen(data));
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
 }
}

 int recois_balises(int client_socket_fd, char *data){
  FILE *fp;
  printf("balises: ");

  char * tableau = strtok(data, " ");
  tableau = strtok(NULL, " ");
  int nb = atoi(tableau);
  char balises[1000];
  int i = 0;

  while(tableau != NULL){
    strcat(balises, tableau);
    strcat(balises, " ");
    printf("%s ", tableau);
    tableau = strtok(NULL, " ");
    i++;
  }
  printf("\n");

  fp = fopen("./tmp/balises.txt", "w+");
  fputs(balises, fp);
  fclose(fp);

  int data_size = write (client_socket_fd, (void *) data, strlen(data));
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
 }
}


/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd) {
  struct sockaddr_in client_addr;
  char data[1024];

  int client_addr_len = sizeof(client_addr);
 
  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *) &client_addr, &client_addr_len);
  if (client_socket_fd < 0 ) {
    perror("accept");
    return(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  //lecture de données envoyées par un client
  int data_size = read (client_socket_fd, (void *) data, sizeof(data));
      
  if (data_size < 0) {
    perror("erreur lecture");
    return(EXIT_FAILURE);
  }
  
  /*
   * extraire le code des données envoyées par le client. 
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf ("Message recu: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);
  //Si le message commence par le mot: 'message:' 
  if (strcmp(code, "message:") == 0) {
    renvoie_message(client_socket_fd, data);
  }
  else if (strcmp(code, "nom:") == 0) {
    renvoie_nom(client_socket_fd, data);
  }
  else if (strcmp(code, "calcule:") == 0) {
    renvoie_calcul(client_socket_fd, data);
  }
  else if (strcmp(code, "couleurs:") == 0) {
    recois_couleurs(client_socket_fd, data);
  }
  else if (strcmp(code, "balises:") == 0) {
    recois_balises(client_socket_fd, data);
  }

  else {
    plot(data);
  }
;
  //fermer le socket 
  close(socketfd);
}

int main() {

  int socketfd;
  int bind_status;
  int client_addr_len;

  struct sockaddr_in server_addr, client_addr;

  //while(1){
    /*
     * Creation d'une socket
     */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( socketfd < 0 ) {
      perror("Unable to open a socket");
      return -1;
    }

    int option = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    //détails du serveur (adresse et port)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Relier l'adresse à la socket
    bind_status = bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_status < 0 ) {
      perror("bind");
      return(EXIT_FAILURE);
    }
 
      // Écouter les messages envoyés par le client
    listen(socketfd, 10);

    //Lire et répondre au client
    recois_envoie_message(socketfd);
 // }

  return 0;
}

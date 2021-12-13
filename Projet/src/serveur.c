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
#include <math.h>

#include "serveur.h"
#include "validateur.h"

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
  char encoded_data[1024];
  encode_JSON(data, encoded_data);
  int data_size = write (client_socket_fd, encoded_data, strlen(encoded_data));

  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

int renvoie_nom(int client_socket_fd, char *data){
  char encoded_data[1024];
  encode_JSON(data, encoded_data);
  int data_size = write(client_socket_fd, encoded_data, strlen(encoded_data));

  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

int renvoie_calcul(int client_socket_fd, char *data){
  float resultat;

  get_calcule_from_parameters(data, &resultat);

  printf("calcule: %.2f\n", resultat);

  int data_size = write (client_socket_fd, (void *) data, strlen(data));
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

float moyenne(float tab[], int nb){
    int i;
    float moyenne = 0;
    for (i = 0 ; i < nb ; i++)
        moyenne += (float)tab[i];
    return moyenne/nb;
}

float min(float tab[], int nb){
    int i;
    float min = tab[0];
    for (i = 1 ; i < nb ; i++)
        if(tab[i] < min)
          min = tab[i];
    return min;
}

float max(float tab[], int nb){
    int i;
    float max = tab[0];
    for (i = 1 ; i < nb ; i++)
        if(tab[i] > max)
          max = tab[i];
    return max;
}

float ecartType(float tab[], int nb){
    int i;
    float moy = moyenne(tab,nb);
    float somme = 0;
    for (i = 0 ; i < nb ; i++)
        somme += powf(tab[i] - moy,2);
    return sqrt(somme/nb);
}

int get_calcule_from_parameters(char *parameters, float *resultat){
  char operateur[20];
  float nombre1,nombre2;
  float tabNombre[20];
  char * tableau = strtok(parameters, " ");
  int nb = 0;

  tableau = strtok(NULL, " ");

  tableau = strtok(tableau, ",");
  strcpy(operateur,tableau);
  int i = 0;
  tableau = strtok(NULL, ",");
  while ( tableau != NULL ) {
    tabNombre[i] = atof(tableau);
    tableau = strtok(NULL, ",");
    i++;
    nb++;
  }


  switch(operateur[0]) {
    case '-':
      *resultat = tabNombre[0] - tabNombre[1];
    break;
    case '/':
      *resultat = tabNombre[0] / tabNombre[1];
    break;
    case '*':
      *resultat = tabNombre[0] * tabNombre[1];
    break;
    case '+':
      *resultat = tabNombre[0] + tabNombre[1];
    break;

    default:
      if(strcmp(operateur, "moyenne") == 0 )
        *resultat = moyenne(tabNombre, nb);
      else if(strcmp(operateur, "minimum") == 0 )
        *resultat = min(tabNombre, nb);
      else if(strcmp(operateur, "maximum") == 0 )
        *resultat = max(tabNombre, nb);
      else if(strcmp(operateur, "ecart-type") == 0 )
        *resultat = ecartType(tabNombre, nb);
    break;
  }
 }

 int recois_couleurs(int client_socket_fd, char *data){
  FILE *fp;

  char * tableau = strtok(data, " ");
  tableau = strtok(NULL, " ");
  tableau = strtok(tableau, ",");

  char couleurs[1000];

  while(tableau != NULL){
    sprintf(couleurs, "%s %s", couleurs, tableau);
    tableau = strtok(NULL, ",");
  }

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

  char * tableau = strtok(data, " ");
  tableau = strtok(NULL, " ");
  tableau = strtok(tableau, ",");

  char balises[1000];

  while(tableau != NULL){
    sprintf(balises, "%s %s", balises, tableau);
    tableau = strtok(NULL, ",");
  }

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
  char decoded_data[1024];

  int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *) &client_addr, &client_addr_len);
  if (client_socket_fd < 0 ) {
    perror("accept");
    return(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));
  //memset(decoded_data, 0, sizeof(decoded_data));

  //lecture de données envoyées par un client
  int data_size = read (client_socket_fd, (void *) data, sizeof(data));

  printf("Message reçu: %s\n", data);
  if (data_size < 0) {
    perror("erreur lecture");
    return(EXIT_FAILURE);
  }

  if (!validateur_JSON(data)){
    int data_size = write(client_socket_fd, "{\"code\" : \"error\", \"valeurs\" : [ \"invalid\", \"JSON\" ]}", 53);
    close(socketfd);
    return(EXIT_FAILURE);
  }

  decode_JSON(data, decoded_data);

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf ("Message décodé: %s\n", decoded_data);
  char code[10];
  sscanf(decoded_data, "%s", code);
  //Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0) {
    renvoie_message(client_socket_fd, decoded_data);
  }
  else if (strcmp(code, "nom:") == 0) {
    renvoie_nom(client_socket_fd, decoded_data);
  }
  else if (strcmp(code, "calcule:") == 0) {
    renvoie_calcul(client_socket_fd, decoded_data);
  }
  else if (strcmp(code, "couleurs:") == 0) {
    recois_couleurs(client_socket_fd, decoded_data);
  }
  else if (strcmp(code, "balises:") == 0) {
    recois_balises(client_socket_fd, decoded_data);
  }

  else {
    plot(decoded_data);
  }
;
  //fermer le socket
  close(socketfd);
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

int encode_JSON(char* data, char* encoded_data){
  printf("Message à encoder: %s\n", data);
  
  char * decoder = strtok(data, ": ");
  sprintf(encoded_data, "{\"code\" : \"%s\", \"valeurs\" : [ ", decoder);

  decoder = strtok(NULL, ": ");
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
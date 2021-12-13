#ifndef __VALIDATEUR_H__
#define __VALIDATEUR_H__

int validateur_JSON(char *data);
int valide_guillemets(char *data);
int valide_deux_points(char *data);
int valide_virgule(char *data);
int valide_datas(char *data);
int valide_crochets(char *data);

#endif
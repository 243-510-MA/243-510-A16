/**
 * Fichier d'entête pour l'écran OLED NHD‐0420CW‐AY3 avec contrôleur US2066
 * par Jasmin St-Laurent
 * Février 2016
 */

#ifndef US2066_H
#define US2066_H

#include <stdint.h>




void Envoyer_Commande(char Commande);
/**
 * Initialise l'écran
 */
void US2066_Initialiser();

/**
 * Positionne le curseur
 * @param Ligne		Numéro de la ligne sur laquelle le curseur doit être positionné (1 à 4)
 * @param Colonne	Numéro de la colonne sur la quelle le curseur doit être positionné (1 à 20)
 */
void US2066_Positionner_Curseur(uint8_t Ligne, uint8_t Colonne);

/**
 * Affiche du texte sur l'écran
 * @param Texte		Chaine de caractère à afficher
 */
void US2066_Afficher_Texte(const char *String, uint8_t col, uint8_t row);

void Envoyer_Donnee(uint8_t Donnee);//fonction envoyer donnee


#endif	// US2066_H

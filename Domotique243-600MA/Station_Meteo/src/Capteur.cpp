
/*
 * Main.cpp
 *
 *  Created on: 27 févr. 2017
 *      Author: carlos
*/

#include<iostream>
#include<unistd.h> //for usleep
#include <cstdio>
#include"US2066.h"
#include"BME280_BB.h"
#include <fstream>		// Pour l'utilisation des fichiers
#include <string>		// Pour l'utilisation des objets string du C++

using namespace std;

#define CHEMIN		"/home/debian/243-510-A16/Domotique243-600MA/"
#define NOM_FICHIER	"Donnee_BME280.json"

#define CHEMIN_FICHIER_TXT "/home/debian/243-510-A16/Domotique243-600MA/"
#define NOM_FICHIER_TXT "FICHIER_TEXT.txt"


int main(void){            //PROGRAME MAIN

	char Tampon_Ecran[21];

	int32_t Temperature;
	uint32_t Humidite;
	uint32_t Pression;

	BME280_Initialiser();  //FONCTION QUI INITIALISE LE CAPTEUR

	US2066_Initialiser(); // FONCTION QUI INITIALISE L'ECRAN OLED


	bme280_set_power_mode(BME280_FORCED_MODE);  //METTRE LE CAPTEUR EN MARCHE FORCÉ
	usleep(10 * 1000);
	bme280_read_pressure_temperature_humidity(&Pression, &Temperature, &Humidite); //LECTURE DES DONNEES

	 float Pression2 = (float)Pression / 1000;
	 float Temperature2 = (float)Temperature / 100;
	 float Humidite2 = (float)Humidite / 1024;

							 // AFFICHAGE DES DONNEES
	//US2066_Positionner_Curseur(1, 1);
	sprintf(Tampon_Ecran, "Temperature: %5.1f",Temperature2);
	US2066_Afficher_Texte(Tampon_Ecran,0,0);

	//US2066_Positionner_Curseur(2, 1);
	sprintf(Tampon_Ecran, "Humidite: %5.1f",Humidite2);
	US2066_Afficher_Texte(Tampon_Ecran,1,0);

	//US2066_Positionner_Curseur(3, 1);
	sprintf(Tampon_Ecran, "Pression: %5.1f",Pression2);
	US2066_Afficher_Texte(Tampon_Ecran,2,0);

	fstream Fichier;					// Création d'un objet pour accéder à un fichier
    string Chemin(CHEMIN);				// Création d'un objet string contenant le chemin d'accès au fichier
    string Nom_Fichier(NOM_FICHIER);	// Création d'un objet string contenant le nom fichier

	// Concaténation d'une chaine de caractères (style C) contenant le chemin d'accès et le nom du ficher contaténés
	// puis ouverture du fichier en mode écriture
	Fichier.open((Chemin + Nom_Fichier).c_str(), fstream::out);

	Fichier << "{\"Temp\":"<< Temperature2 <<",\"Humidite\":"<< Humidite2<<",\"Pres\":"<<Pression2<<"}";	// Écriture dans le fichier

	Fichier.close();					// Fermeture du fichier


    sleep(30);

    Envoyer_Commande(0x01);
    // memset (buffer,' ',50);
    fstream Fichier_TXT;
    string Chemin_Fichier_Txt(CHEMIN_FICHIER_TXT);
    string Nom_Fichier_Txt(NOM_FICHIER_TXT);
    string Contenu_txt;
    string chainecopy;

    Fichier_TXT.open((Chemin_Fichier_Txt + Nom_Fichier_Txt).c_str(), fstream::in);

    //Fichier_TXT.read(buffer,50);

    //sprintf (buffer, "", Fichier_TXT);
    getline(Fichier_TXT,Contenu_txt);

    US2066_Afficher_Texte(Contenu_txt.c_str(),0,0);



    Fichier.close();

    //sleep(1);
    //command(0x01);



	return 0;
}






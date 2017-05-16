//============================================================================
// Name        : Ecran.cpp
// Author      : carlos
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include<iostream>
#include<unistd.h> //for usleep
#include"GPIO.h"
#include"BusDevice.h"
#include"I2CDevice.h"
#include"US2066.h"


using namespace exploringBB;
using namespace std;

  I2CDevice i2c_Ecran(2,0x3c);

/**
 * Evoyer Commande
 */
void Envoyer_Commande(char Commande){

	i2c_Ecran.writeRegister(0x00,Commande);//(COMANDE(0x00) OU DONNEE(0x40),VALUE)

}


/**
 * Evoyer Donnée
 */
void Envoyer_Donnee(uint8_t Donnee){

	i2c_Ecran.writeRegister(0x40,Donnee);//(COMANDE(0x00) OU DONNEE(0x40),VALUE)

}

void US2066_Initialiser(){

	 GPIO outReset(49);//Ecran  **outGPIO(49)**
	 outReset.setDirection(GPIO::OUTPUT); //Reset a 1
	 outReset.setValue(GPIO::LOW);
	 usleep(10000);//pour allumer ecran
	 //outReset.setDirection(GPIO::OUTPUT); //Reset a 1
	 outReset.setValue(GPIO::HIGH);             //pour allumer ecran
	 usleep(10000);
	 Envoyer_Commande(0x2A);
	 Envoyer_Commande(0x71);
	 Envoyer_Donnee(0x00);
	 Envoyer_Commande(0x28);
	 Envoyer_Commande(0x08);
	 Envoyer_Commande(0x2A);
	 Envoyer_Commande(0x79);
	 Envoyer_Commande(0xD5);
	 Envoyer_Commande(0x70);
	 Envoyer_Commande(0x78);
	 Envoyer_Commande(0x09);
	 Envoyer_Commande(0x06);
	 Envoyer_Commande(0x72);
	 Envoyer_Donnee(0x04);
	 Envoyer_Commande(0x2A);
	 Envoyer_Commande(0x79);
	 Envoyer_Commande(0xDA);
	 Envoyer_Commande(0x10);
	 Envoyer_Commande(0xDC);
	 Envoyer_Commande(0x00);
	 Envoyer_Commande(0x81);
	 Envoyer_Commande(0xFF);
	 Envoyer_Commande(0xD9);
	 Envoyer_Commande(0xF1);
	 Envoyer_Commande(0xDB);
	 Envoyer_Commande(0x40);
	 Envoyer_Commande(0x78);
	 Envoyer_Commande(0x28);
	 Envoyer_Commande(0x01);
	 Envoyer_Commande(0x80);
	 usleep(50000);
	 Envoyer_Commande(0x0C);


}

void US2066_Positionner_Curseur(uint8_t Ligne, uint8_t Colonne){
	int row_offsets[] = { 0x00,0x20,0x40,0x60};
	 Envoyer_Commande(0x80 | (Colonne + row_offsets[Ligne]));
}


void US2066_Afficher_Texte(const char *String, uint8_t Colonne, uint8_t Ligne){

	  US2066_Positionner_Curseur(Colonne, Ligne);
	  unsigned char i=0;
	  while(String[i])
	  {
		Envoyer_Donnee(String[i]);      // *** Show String to OLED
	    i++;
	  }

}



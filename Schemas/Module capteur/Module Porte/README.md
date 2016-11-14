Pour Calculer la Résistance nécessaire pour le 
courant de la base il faut utiliser la formule suivante
Ic = hfe * Ib

Ic= courant collecteur
hfe= Gain du transistor bijonction
Ib= Courant a la base


Dans notre utilisation, l'electro aimant consomme
200mA max. mais la limite du transistor est de 200mA
donc on a decicé de l'alimenter avec 190mA. 
Donc Ic=190mA

Pour calculer le hfe il faut utiliser le multimetre sur 
option NPN.   hfe = 189

avec la formule on deduit que Ib nécessaire est de 1mA.

ensuite pour trouver la resistance on utilise la loi de ohm.
R=V/I (il ne faut pas oublier de soustraire 0.7V de la tension
 au signal du a la chute de tension du transistor.

V= (3.3 - 0.7) = 2.6V

R= 2.6 / 1mA = 2600 Ohms.

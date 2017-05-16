243-600 Systèmes d'exploitation
===============================
Projet domotique
----------------
#Avant de continuer le projet, assurez vous d'avoir effectué toutes les installations nécéssaires sur le beaglebone qui sont sur le fichier => Étapes d'installation. 

Dans le cadre du cours 243-600 on doit ajouter des fonctionnalités au projet de la classe intelligente réalisé dans le cours de Communication numérique sans-fil. 
On doit être capables de faire:

*Une visualisation des conditions atmosphériques de la classe à partir de n'importe quel ordinateur connecté à Internet.
*Faire une visualisation de l'état de la Porte à partir de n'importe quel ordinateur connecté à internet
*Avoir la possibilité pour un professeur de verrouiller ou déverrouiller la classe à pârtir de n'importe quel ordinateur connecté à internet.
*Possibilite de savoir par internet qui est présent dans la classe
*Affichage d'un message envoyé par Internet sur l'écran OLED du Beaglebone
*Toute autre fonctionnalité jugée utile ou intéressante

Ce qui a été complété jusqu'a maintenant à la date du : 09/Mai/2017

* La visualisation des conditions atmosphériques de la classe à partir de n'importe quel ordinnateur connecté à internet.
>on a reussi à lire les données sur le capteur meteo BME_280 et de mettre les données >sur un fichier json. (Donnee_BME280.json) qui est sauvegarde sur /home/debian du >Beaglebone. Le code qui s'occupe de tout faire cela est dans le fichier Station_Meteo/ >Capteur. Capteur est le code source qui doit être compilé et mis dans le beaglebone.

*Visualisation de l'état de la porte à partir de n'importe quel ordinateur connecté à internet.
>Sur le dashboard: Classe intelligente sur Adafruit( username:243600MA MDP:temppwd)
>il y a un «feed» qui est un bouton(Status Porte) et auquel on peut changer l'état à >partir du beaglebone et l'inverse aussi sauf qu'il faut vérifier l'état à chaque >fois, >ce qui crée du traffic non-nécéssaire. 

*Possibilité, pour un proffesseur, de verouiller ou de deverrouiller la classe à partir de n'importe quel ordinnateur connecté à internet.
>CE QU'IL FAUT FAIRE: c'est trouver comment faire la communication avec le protocole >MQTT qui nous permettrait d'utiliser la fonction Subscribe. On retrouve l'information >sur:https://github.com/adafruit/io-client->python/blob/master/examples/mqtt_client.py
>Avec mqtt on aura pas besoin de toujours vérifier l'état puisque le server va dire au >Beaglebone dès qu'une modification a été faite sur le dashboard.

*Possibilité de savoir par internet qui est présent dans la classe
>Pour les modules MiWi, les modules en student sont capables d'envoyer leur addresse >MAC au module Computer Control a chaque minute. Le computer control les recoit mais  >fait rien à cause que ce n'est pas encore implementé. 
>il resterait a implementer un code qui envoie cette information au Beaglebone aussi >et a partir du Beaglebone envoyer cette info au Dashboard du serveur Adafruit.io 
>Il y a une document sur github qui explique le problème qu'il y a sur le UART du >Computer Control.

*Affichage d'un message envoyé par internet sur l'écran OLED du Beaglebone
>On est capables d'afficher un message sur le Beaglebone qui provient du Dashboard >sauf qu'ici encore on doit le vérifier au lieu de recevoir un avertissement qui >provient du serveur quand il a été changé. il faut trouver une facon de faire >fonctionner le MQTT




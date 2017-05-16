#coding=utf-8
import json
import subprocess

subprocess.call(["/home/debian/243-510A16/Domotique243-600MA/Capteur"])

file = open("Donnee_BME280.json","r")
message = file.read()
message_dictionnaire = json.loads(message)
message_dictionnaire["Humidite"]
print('humidite:' + str(message_dictionnaire["Humidite"]))
message_dictionnaire["Pres"]
print('Pression atmospherique:' + str(message_dictionnaire["Pres"]))
message_dictionnaire["Temp"]
print('temperature:' + str(message_dictionnaire["Temp"]))

from Adafruit_IO import Client

aio = Client('651706c4f84d4060a7b5b9db59e58862')

#envoyer données au serveur

aio.send('humidite',message_dictionnaire["Humidite"])

#aio.send('pression-atmospherique',message_dictionnaire["Pres"])

aio.send('temperature',message_dictionnaire["Temp"])


#aio.send('Status Porte','ON')

#Si On veut changer le message sur le serveur a partir du code, 
#enlever le commentaire #

#aio.send('message','Jasmin le boss')

#Lecture du message au serveur et ecriture sur un fichier text.

data = aio.receive('message')

print('message: {0}'.format(data.value))

file2 = open("/home/debian/243-510-A16/Domotique243-600MA/FICHIER_TEXT.txt","w")
file2.write(data.value)

#tout le code precedent communique par HTTP, il faut trouver une facon 
#d'utiliser les fonctions MQTT qui peuvent nous permettre d'utiliser
# les fontions subscribe, broadcast.

#TEST
## A essayer: https://loadingdata.nl/mqtt-to-adafruit-io/


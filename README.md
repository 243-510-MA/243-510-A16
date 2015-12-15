# 243-510-A15
243-510 Communications numériques sans fil - automne 2015
Collège de Maisonneuve

Notre projet est de réaliser un système dynamique contrôlable à distance à l'aide d'un réseau MiWi pour faciliter le travail
et la vie des étudiants et professeurs du département du Génie électrique du Collège de Maisonneuve. Le projet comporte un
système d'alarme, avec un gyrophare et un avertisseur sonore (buzzer), un contrôleur magnétique pour déverrouiller une porte et un contrôleur qui
permet de contrôler un projecteur et une toile.



Parties fonctionnelles :


- Contrôle du projecteur (ON/OFF) à l'aide d'une communication série (RS-232)(BAUD 9600)(LSB(inverted?)) :

  Trames utilisées :
  
    Power on
    
    - 0x00 0xBF 0x00 0x00 0x01 0x02 0xC2
    - 0x02 0x00 0x00 0x00 0x00 0x02
    
    
    Power off
    
    - 0x00 0xBF 0x00 0x00 0x01 0x00 0xC0
    - 0x02 0x00 0x00 0x00 0x00 0x03

- Le contrôle de la toile à l'aide d'un servomoteur.
- Le contrôle de la porte à l'aide d'un électroaimant.
- L'envoi d'un questionnaire par le professeur aux élèves avec les réponses affichées sur l'écran du module professeur.
- Le gyrophare et le buzzer fonctionnent.
- Le chargeur du module étudiant (prototype) fonctionne.
- Les modules peuvent tous contrôler les options en réseau MiWi (PAN et end-devices).



Parties non fonctionnelles :

- Le détecteur de mouvement n'est pas implémenté.



Bugs connus à résoudre pour avoir un fonctionnement parfait :

- [PROBLÈME TROUVÉ, MAIS NON RÉSOLU]Le projecteur ne répond pas lorsque certaines conditions sont remplies. Si la lumière verte de "STATUS" est éteinte et que le bouton "power" est orange, le projecteur ne réagit pas aux communications séries (RS-232). La solution pour résoudre le problème est d'ajouter les trames de débuts manquantes dans l'envoi d'un message RS-232. Le contrôleur doit envoyer une trame de "wake-up" au projecteur à chaque 7 secondes environ pour s'assurer qu'il est prêt à "écouter" sur le bus. La trame de "wake-up à ajouter est : 0x00 0xBF 0x00 0x00 0x01 0x00 0xC0.


Étudiants contributeurs :
  François D'Amours-Lavoie, 
  Mathieu Labelle, 
  Laurent Brochu, 
  Jonathan Chouinard, 
  Alexandru Marin, 
  Étienne Vaillancourt, 
  Renaud Varin, 
  Dominic Fournier, 
  Dominik Boudreau-Perron, 
  Gabriel Ahr


Professeur contributeur : 
  Frédéric Daigle

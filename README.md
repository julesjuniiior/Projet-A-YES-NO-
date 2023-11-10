# Projet-A-YES-NO-
## Experiment and Description
## Tools
- windows computer
- TensorFlow Lite for Microcontroller Challenge Kit or [ Nano BLE Sense 33](https://www.gotronic.fr/art-arduino-nano-33-ble-sense-abx00031-30757.htm)
- [micro-usb cable](https://www.google.com/search?rlz=1C5CHFA_enUS858US858&sxsrf=ALeKk01CbJTvQbYgX6arJbsjcRVmv-3-RQ:1584929968297&q=Micro+USB+cable&spell=1&sa=X&ved=2ahUKEwjl8IOexK_oAhXDqZ4KHZ0mCmcQBSgAegQIDhAn&biw=1680&bih=832)
## Install and Run
1. Installer [arduino IDE](https://www.arduino.cc/en/software#future-version-of-the-arduino-ide) sur votre ordinateur
2. configuration de la carte Arduino
     -branchez la carte
     -installez en accédant à Tools>Board>Board Manager et chercher **Arduino Mbed OS Nano Boards.
     ![image](https://github.com/julesjuniiior/Projet-A-YES-NO-/blob/main/images_Readme/config-carte.png)
     - sélectionner le port en accédant à Tools -> Port -> dev/cu... (Arduino Nano 33 BLE)
3. Install Arduino libraries
Navigate to Tools > Manage Libraries
Search for and install:
     -[Arduino_LSM9DS1](https://www.arduino.cc/reference/en/libraries/arduino_lsm9ds1/?_gl=1*aep7sb*_ga*MTQ4Nzc5Nzg0My4xNjkzOTE0NjA3*_ga_NEXN8H46L5*MTY5NzcxODM1Ny45LjEuMTY5NzcyMDA0MS4wLjAuMA)
     -Harvard_Tinymlx (Tensorflow)
## Etapes de réalisation de tout le projet
     Après avoir installer le logiciel et toutes les librairies nécessaires; nous allons pouvoir dans un premier temps récolter les données de l'accéloromètre et Entrainer notre modèle avec Tensorflow.
   ### Step 1 **Entrainement du modèle**
        #### collecter les données  
   


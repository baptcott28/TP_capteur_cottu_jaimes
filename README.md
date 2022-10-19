# TP_capteur_cottu_jaimes
 
Mise en œuvre du BMP280

Le BMP280 est un capteur de pression et température développé par Bosch (page produit).

À partir de la datasheet du BMP280, identifiez les éléments suivants:

1. les adresses I²C possibles pour ce composant.
The 7-bit device address is 111011x. The 6 MSB bits are fixed. The last bit is changeable by SDO value and can be changed during operation. Connecting SDO to GND results in slave address 1110110 (0x76); connection it to VDDIO results in slave address 1110111 (0x77), which is the same as BMP180’s I²C address. The SDO pin cannot be left floating; if left floating, the I²C address will be undefined. 

2. le registre et la valeur permettant d'identifier ce composant
Registre Id : 0xD0 (chip identification number) et la valeur attendue est 0x58

4. le registre et la valeur permettant de placer le composant en mode normal
5. les registres contenant l'étalonnage du composant
6. les registres contenant la température (ainsi que le format)
7. les registres contenant la pression (ainsi que le format)
8. les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits.

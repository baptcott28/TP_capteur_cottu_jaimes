# TP_capteur_cottu_jaimes
 
## Mise en œuvre du BMP280

Le BMP280 est un capteur de pression et température développé par Bosch (page produit).

À partir de la datasheet du BMP280, identifiez les éléments suivants:

1. les adresses I²C possibles pour ce composant:

The 7-bit device address is 111011x. The 6 MSB bits are fixed. The last bit is changeable by SDO value and can be changed during operation. Connecting SDO to GND results in slave address 1110110 (0x76); connection it to VDDIO results in slave address 1110111 (0x77), which is the same as BMP180’s I²C address. The SDO pin cannot be left floating; if left floating, the I²C address will be undefined. 

2. le registre et la valeur permettant d'identifier ce composant:

Registre Id : 0xD0 (chip identification number) et la valeur attendue est 0x58

3. le registre et la valeur permettant de placer le composant en mode normal

The BMP280 offers three power modes: sleep mode, forced mode and normal mode. These can
be selected using the mode[1:0] bits in control register 0xF4.

00 Sleep mode
01 & 10 Foced mode
11 Normal mode

4. les registres contenant l'étalonnage du composant

Calibration register : 0xA1->0x88 

5. les registres contenant la température (ainsi que le format)

Tempeature registers from 0XFA to 0xFC. 0xFA = MSB[7:0] et 0xF8BLSB[7:0] et 0xFC (bit 7 6 5 4) en option a voir avec la résolution de la pression.

7. les registres contenant la pression (ainsi que le format)

Pressure registers from 0x07 to 0xF9. 0xF7 = MSB[7:0] et 0xF8=LSB[7:0] et 0xF9 (bit 7 6 5 4) en option a voir avec la résolution de la temperature.

7. les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits.

La valeur de température arrive sur un entier 20 bits, mais on peut la transormer en valeur sur 32 bit en la compensant par la valeur de la pression
```C
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
BMP280_S32_t var1, var2, T;
var1 = ((((adc_T>>3) – ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
var2 = (((((adc_T>>4) – ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) – ((BMP280_S32_t)dig_T1))) >> 12) *
((BMP280_S32_t)dig_T3)) >> 14;
t_fine = var1 + var2;
T = (t_fine * 5 + 128) >> 8;
return T;
}
“”–
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
BMP280_S64_t var1, var2, p;
var1 = ((BMP280_S64_t)t_fine) – 128000;
var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
if (var1 == 0)
{
return 0; // avoid exception caused by division by zero
}
p = 1048576-adc_P;
p = (((p<<31)-var2)*3125)/var1;
var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
return (BMP280_U32_t)p;
```

Affichage du printf : Bien vider le cache avec un \r\n pour l'affichage

Sur la carte, BMP280 est connécté a VDDIO donc l'adresse du composant est 1110111 (0x77)

On configure ensuite le BMP280 en mode normal (11), pressure oversamplingx16 (101) et temperature x2 (010)

## User et Mdp

User : jaimes
Mdp : cottu_jaimes

## Serveur de base

A chaque fois que l'on modifie le fichier python du serveur, il faut le run avant de vouloir y acceder : 
`pi@raspberrypi:~/server $ FLASK_APP=hello.py FLASK_ENV=development flask run --host 0.0.0.0`

#### Accessibilité du serveur 

La variable name de `app = Flask(__name__)` est une variabe build-in donc on ne peut pas mettre ce qu'on veut. 
Pour l'instant, le serveur ne fonctionne qu'en local d'ou l'adresse 127.0.0.1 et le serveur est accessible via le port 5000.
On fait ensuite en sorte qu'il soit accessible à partir d'un navigateur. Pour y acceder, on doit rentrer l'adresse de la Rpi ainsi que le port :  
>http://192.168.88.249:5000/ 

#### @add_route
Comme son nom l'indique, `@app_route` ajoute une page au serveur. Celle-ci est accessible à partir de l'adresse de base en ajoutant le lien de la route créée : >http://168.192.88.249/api/welcome/ 

#### <int:index>
Permet d'identifier un caractère dont l'index est précisé après le dernier `/` de l'adresse entrée dans le navigateur.
>http://168.192.88.249/api/welcome/2   renvoie   `{"index": 2, "val": "l"}` 

## Serveur RESTfull
#### Obtenir une réponse JSON
Pour obtenir une réponse en JSON, on utilise les fonction `json.dumps()`. Le problème est que la réponse renvoyée n'est pas vraiment du json comme en témoign l'image suivante : 
> image 1

Pour remedier a cela, on ajoute `, {"Content-Type": "application/json"}` après `json.dumps()`. On a bien une réponse json.

>image 2

On peut aussi utiliser la commande `jsonify()` apres l'avoir importée dans le projet `from flask import jsonify`.

#### Erreur 404
On copie colle le code source de la page erreur 404 et on crée une fonction qui renvoie vers cette page lorsuqe l'index demandé est trop élevé. Pour cela, on apele juste la fonction qui genere la page d'erreur. 
```P
def page_not_found(error):
    return render_template('page_not_found.html'), 404```

def api_welcome_index(index):
        if index>19:
                return page_not_found(404)
        else :
                return jsonify({"index": index, "val": welcome[index]})
```

## Test de methode 
#### Methode naive POST
Effectivement, nous avons un erreur de type 405 :  request not allowed

> Image

#### Methode 

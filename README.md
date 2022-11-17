# INCLURE LA DOC DOXIGEN
# TP_capteur_cottu_jaimes
 
## TP1 : Mise en œuvre du BMP280
### Réponse aux questions préliminaires sur le capteur

Le BMP280 est un capteur de pression et de température développé par Bosch ([Documentation](https://moodle.ensea.fr/pluginfile.php/59766/mod_resource/content/1/bst-bmp280-ds001.pdf)).

1. Les adresses I²C possibles pour ce composant:

L'adresse sur 7 bit est la suivante : 111011x. Le dernier bit diffère selon si la pin SDO est connectée sur le GND, auquel cas l'adresse devient 1110110 (0x76), ou bien si cette pin est connectée sur le VDD auquel cas l'adresse devient 1110111 (0x77). Sur la carte préfaite, on voit sur la datasheet que cette pin est connectée à VDDIO. 

2. Le registre et la valeur permettant d'identifier ce composant:

Registre Id : 0xD0 (chip identification number) et la valeur attendue est 0x58

3. Le registre et la valeur permettant de placer le composant en mode normal

Pour le réglage du BMP280, il faut rentrer une combinaison de deux bits dans un registre dont l'adresse est 0xF4. Le détail des deux bits est donné ci-dessous.
- 00 Sleep mode
- 01 & 10 Foced mode
- 11 Normal mode

4. Les registres contenant l'étalonnage du composant

Les registres qui permettent la calibration du composant sont les registres disponibles aux adresses 0xA1->0x88 

5. Les registres contenant la température (ainsi que le format)

Les registres qui contiennent la température vont de l'adresse 0XFA à l'adresse 0xFC. 0xFA = MSB[7:0] et 0xFB = LSB[7:0]. Le registre 0xFC (bit 7 6 5 4) est configurable en option.

6. Les registres contenant la pression (ainsi que le format)

Les registres qui contiennt la pression vont de l'adresse 0x07 à l'adresse 0xF9. 0xF7 = MSB[7:0] et 0xF8 = LSB[7:0]. De même, le registre 0xF9 (bit 7 6 5 4) est configurable en option.

7. Les fonctions permettant le calcul de la température et de la pression compensée, en format entier 32 bits.

La valeur de température arrive sur un entier 20 bits, mais on peut la transformer en valeur sur 32 bit en la compensant par la valeur de la pression
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

### Fonction d'interaction avec le capteur
Pour récupérer les élements essentiels, on écrit les fonctions suivantes, regroupées dans le fichier `motor.c`. Les macros utiles sont définies quand à elles dans le fichier `motor.h`.

```C
void BMP_get_ID(void);
void BMP_send_Configuration(void);
void BMP_get_calibration_temp_press(void);
uint32_t BMP_get_temperature(void);
uint32_t BMP_get_press(void);
int get_coef_k(void);
```
On configure ensuite le BMP280 en mode normal (11), pressure oversamplingx16 (101) et temperature x2 (01). Ces informations sont centenues dans la macro `CONFIG` (0X57 = 1010111).

Toutes ces fonctions ont été testées et sont opérationnelles. 

## TP2 : Prise en main de la Rpi et implémentation de la transmission série
### Interprétation commandes STM32
L'objetif de ce TP est de créer une interface entre la Raspberry et la carte STM32. Il s'agit donc de pouvoir interpréter des commandes arrivant par UART. Pour cela, on active l'UART1 (115200baud/s, UART1_Tx = PA9, UART1_Rx = PA10). Pour plus de simplicité dans le code, on redirige le printf sur cet UART (fichier `stm32f4xx_hal_msp.c`, ligne 332).

Le protocole de communiquation est le suivant. Il est réduit au plus simple de sorte à ce qu'il n'y ait qu'une valeur qui change d'un ordre à un autre pour en faciliter le décodage. 

![image](https://user-images.githubusercontent.com/85641739/202512657-0fcd86e9-2cb5-42b2-8650-abe1fe6ed785.png)

On active un deuxième port pour que les ordres puissent arriver depuis la Rpi. L'UART1 est relié à la console de l'ordi tandis que l'UART2 est reliée à la Rpi ( 115200 baud/s, UART2_Tx = PA2, UART2_Rx = PA3). 

Pour interpreter les commandes arrivantes, on écrit les fonctions suivantes contenues dans le fichier `comm_Rpi.c`, les macros étant définies dans `comm_Rpi.h`.

```C
void comm_Rx_order_buffer_analyse(void);
void comm_wait_for_order(void);
void comm_clean_Rx_order_buffer(void);
```
Pour l'instant, l'attente de commandes se fait en mode polling mais on aurait pu mettre l'UART1 en mode interruption pour plus d'efficacité.
### Rpi : Etapes préliminaires

On doit tout d'abord modifier plusieurs éléments dans les fichiers de boot de la Rpi pour la rendre accessible par SSH. On crée donc un fichier `SSH` (vide) dans le repertoire `boot`, puis un fichier `wpa_supplicant.conf` dans lequel on écrit le code suivant
``` 
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=FR

network={
 ssid="<Name of your wireless LAN>"
 psk="<Password for your wireless LAN>"
}
```
On active ensuite l'UART 1 en ajoutant les lignes suivantes dans le fichier `config.txt` sur la partition `boot` :
```
enable_uart=1
dtoverlay=disable-bt
```
Enfin, on configure l'UART en ajoutant la ligne `console=serial0,115200` dans le fichier `cmdline.txt`.
### Point clés
- Affichage du printf : Bien vider le cache avec un `\r\n` pour que l'affichage s'execute.
- Modifier un fichier : `nano nom_fichier`

## TP3 : Serveur de base
On commence par créer un utilisateur différent de pi sur la Rpi.
### Rpi : changement user et mdp
User : jaimes

Mdp : cottu_jaimes

### Création premier fichier web
On crée d'abord un fichier `serveur_jaimes` dans lequel on crée le fichier `hello.py` et dans lequel on ajoute le code suivant :
```
from flask import Flask
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!\n'
```

On démare le serveur avec la commande `pi@raspberrypi:~/server $ FLASK_APP=hello.py flask run`, et on le teste avec la commande `pi@raspberrypi:~/server $ curl http://127.0.0.1:5000`.

### Accessibilité du serveur 

La variable name de `app = Flask(__name__)` est une variabe build-in donc on ne peut pas mettre ce qu'on veut. 
Pour l'instant, le serveur ne fonctionne qu'en local d'ou l'adresse 127.0.0.1 et le serveur est accessible via le port 5000.
On fait ensuite en sorte qu'il soit accessible à partir d'un navigateur avec la commande

`pi@raspberrypi:~/server $ FLASK_APP=hello.py FLASK_ENV=development flask run --host 0.0.0.0` 

Pour y acceder, on doit rentrer l'adresse de la Rpi ainsi que le port :  
>http://192.168.88.249:5000/ 

`FLASK_ENV=development` permet de lancer le mode debug. 

### @add_route
Comme son nom l'indique, `@app_route` ajoute une page au serveur. Celle-ci est accessible à partir de l'adresse de base en ajoutant le lien de la route créée : 
>http://168.192.88.249/api/welcome/ 

### <int:index>
Permet d'identifier un caractère dont l'index est précisé après le dernier `/` de l'adresse entrée dans le navigateur.
>http://168.192.88.249/api/welcome/2   renvoie   `{"index": 2, "val": "l"}` 

## Serveur RESTfull
### Obtenir une réponse JSON
Pour obtenir une réponse en JSON, on utilise la fonction `json.dumps()`. Le problème est que la réponse renvoyée n'est pas vraiment du json comme en témoigne l'image suivante ( navigateur : outils de developpement : onglet network->Content-type) : 

![Réponse de requette avec json.dumps()](https://github.com/baptcott28/TP_capteur_cottu_jaimes/blob/main/requette%20jsaon%20ce%20n'est%20pas%20du%20json.jpg)

Pour remedier à cela, on ajoute `, {"Content-Type": "application/json"}` après `json.dumps()`. On a bien une réponse json. (onglet network->Content-type) : 

![Réponse de requette avec le Content-Type ajouté](https://github.com/baptcott28/TP_capteur_cottu_jaimes/blob/main/requette%20jsaon%20qui%20est%20bien%20du%20json.jpg)

On peut aussi utiliser la commande `jsonify()` après l'avoir importée dans le projet avec la ligne `from flask import jsonify`.

#### Erreur 404
On copie colle le code source de la page erreur 404 et on crée une fonction qui renvoie vers cette page lorsque l'index demandé est trop élevé. Pour cela, on appele juste la fonction qui génère la page d'erreur. 
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
### Methode naive POST
Lorsque nous essayons de faire une requette non autorisée sur notre serveur, nous avons une erreur de type 405 :  request not allowed

![ Requette POST reussie ! ](https://github.com/baptcott28/TP_capteur_cottu_jaimes/blob/main/methode%20post%20pas%20valide.jpg)

Pour signifier qu'une méthode est autorisée dans la page spécifiée, nous ajoutons la ligne suivante dans notre code : 
`@app.route('/api/welcome/<int:index>', methods=['GET','POST'])`. Celle ci ne nous renvoie pas pour autant quelque chose. 

### Methode POST avec renvoi d'information

Nous utilisons la fonction curl pour effectuer nos premières requettes et remplir les champs. Notre requette s'écrit sous la forme suivante :
`curl -d '{"argc":"12", "data":"bonjour"}' -H "Content-Type: application/json" -X POST http://192.168.88.249/api/request/`

Il faut faire attention à plusieurs points dans ces requettes : 
- Ne pas oublier le slash à la fin de l'URL (ca sinon on signifie un dossier au lieu d'une URL)
- Préciser le Content-Type de la requette
- Ne pas oublier de mettre le -X devant le mot clé et le -d au début qui signifie que l'on met de la data dans la requette

#### Implementation des réponses aux autres méthodes

Avant d'implémenter d'autres méthodes, il faut préciser à chaque fois la route dans lesquelles elle sont autorisées, mais aussi quelles méthodes sont autorisées dans la dite route. Ceci se fait sous la forme suivante :
`@app.route('api/<path>', methods=['XX1','XX2',...])`
On définit ensuite la fonction qui va traiter chaque méthode.

Par exemple, nous ajoutons le code suivant afin de traiter la méthode PUT dans la page `/api/welcome/`
```P
@app.route('/api/welcome/<int:index>',methods=['PUT'])
def api_put(index):
        global welcome
        data = request.get_json()
        if index > len(welcome):
                abort(404)
        else:
                #construction de la nouvelle chaine
                welcome = welcome[:index] + data + welcome[index:]
                return welcome + '\r\n'
```

## TP4 : Bus CAN

On doit maintenant envoyer diverses consignes de position à un moteur pas à pas dans le but de le faire aller alternativement à -90° et 90°. Pour cela, on utilise le bus CAN 1 du STM32.

#### Réglages CubeMx
La datasheet du driver de bus CAN spécifie que la fréquence admise par le décodeur est de 500kB/s strictement. On règle donc l'horloge du bus de donnée que l'on abaisse à 80MHz. Ce n'est pas impactant pour les autres périphériques étant donnés que nous n'utilisons aucun timer sur ce projet. 

On met ensuite le prescaler du bus à 16, puis les champs `Time Quanta bit Seglent 1` et `Time Quanta Bit Segment 2` à `2 Time`, pour se ramener finalement à 500000 bit/s. Les deux champs évoqués ci-dessus servent à régler l'instant de prise de décision sur le bit à décoder. 

**Il faut impérativement mettre la même valeur aux deux champs sous peine de voir le code ne pas fonctionner.** 

#### Code relatif au bus CAN 

On définit les structures suivantes pour construire les trames CAN:
```C
uint8_t aData[MOTOR_MODE_AUTO];
CAN_TxHeaderTypeDef pHeader;
uint32_t pTxMailbox;
```
La ligne `#define MOTOR_MODE_AUTO 2`, permet d'initialiser aData avec deux cases qui contiendront les champs à envoyer pour commander le moteur en mode automatique. 

On définit ensuite plusieurs macros dans motor.h afin de pouvoir driver le moteur plus facilement : 
```C
#define ANGLE_POSITIVE 0x00
#define ANGLE_NEGATIVE 0x01
#define ANGLE_90 0x5A
#define STD_ID 0x61					//<! Motor_ID in automatic mode
```

Nous devons ensuite initialiser le header de la trame en fixant notament la longueur du champ de données utiles, l'adresse du composant ainsi que l'objet de la trame (request data or send them to the device). Ceci est fait par la fonction `uint8_t motor_CAN_Init_Start(void)`, qui renvoie 1 si la fonction 'HAL_CAN_Start(&hcan1)' s'est bien passée, et qui affiche une erreur dans la console sinon. 

L'envoi de consignes au moteur se fait par le biais de la fonction `uint8_t motor_tourne(uint8_t angular_position, uint8_t rotation_direction)`. Cette fonction prend en argument la position angulaire désirée (position absolue), mais aussi le sens de rotation souhaité pour le moteur. De la même manière que la fonction précédante, elle renvoie 1 si la fonction `HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox)` s'est bien passée, ou affiche une erreur dans la console le cas échéant. 

##TP5 : Liason des deux

Pendant ce TP, nous avons voulu commencer par terminer les fonctions commencées précédement mais aussi les tester. Pendant que l'un essayait de faire fonctionner la compensation de pression et de temperature, l'autre a écrit la fonction `void motor_handle(void)` qui gère l'action du moteur en fonction des variations de température. 

Nous avons eu plusieurs problèmes pour rédiger cette fonction. Sans valeur sur 8 bits (la fonction de compensation n'ayant pas fonctionnée) il a fallut gérer les données réceptionnées sur 32 bits. Pour cela, nous avons détourné l'usage du coefficient K pour diviser les valeur sur 32 bits par ce coeficient. Ainsi, nous nous sommes ramenées à des valeures moins fluctantes.

Puis nous avons fait une sorte de tout ou rien pour le moteur en comparant deux valeur succesives de températures. Si la température augmente, le moteur "ouvre les vannes de la climatisation à fond" pour la baisser (angle de -90°) tandis que si la température diminue, le moteur ferme la vanne de climatisation en mettant le moteur en position +90°. Bien que limitée en application réelle, cette fonction est opérationelle.

Les fonctions suivantes constutuent l'interface entre la Rpi et la STM32. Malheureusemnt, nous n'avons pas eu le temps de la tester.



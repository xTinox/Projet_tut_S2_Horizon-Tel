/*
 * Auteur: xTinox
 * Titre: Bouton sur ESP 8266, connecté au serveur MQTT
 * Version: 0.1
 * Date: 08/03/2020
 * Commentaire(s): Bouton connecté + publication de l'état du poste (variable: RESULTAT)
 * 
 * ATTENTION: Avant de téléverser veuillez aller dans Préférences/Ajouter URL et copier/coller l'URL suivant: http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *            Puis aller dans l'onglet Croquis de l'IDE Arduino /Inclure une bibliothèque/Gérer les bibliothèques/ puis rechercher et télécharger ESP8266Wifi.h ainsi que PubSubClient.h
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Informations de la WiFi
const char* ssid = "NOM_DE_LA_BOX_WIFI";
const char* password = "MOT_DE_PASSE_WIFI";
// Informations serveur MQTT
const char* mqtt_server = "test.mosquitto.org";
int mqtt_port = 1883;

const int led_rouge = 13;         // pin D7 du ESP 8266
const int led_verte = 12;         // pin D6 du ESP 8266
const int led_bleue = 14;         // pin D5 du ESP 8266
const int pinBouton = 16;         // pin D0 du ESP 8266
int boucle=200;             // Variables:
int etatBouton;                 //
int tempo;                      //
int attente=4;                  //
int attente2=2;                 //
int compteur=6000;              //
int resultat=0;                 //  
int RESULTAT=0;                 //
int valEnregistree = 0;         //

// Declaration "espClient" pour serveur MQTT
WiFiClient espClient;
PubSubClient client(espClient);


// Fonction de connexion au Wifi
void setup_wifi() {
 
 delay(10);
 // On commence par une connexion au réseau Wifi
 Serial.println();
 Serial.print("Connexion à ");
 Serial.println(ssid);
 
 WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 
 Serial.println("");
 Serial.println("WiFi connecté");
 Serial.println("Adresse IP: ");
 Serial.println(WiFi.localIP());
}


// Fonction: Allumage de la led RGB
void led(int r, int v, int b){
  digitalWrite(led_rouge, r);
  digitalWrite(led_verte, v);
  digitalWrite(led_bleue, b);
}


// 
void Result(){
  if (resultat==0){
    led(1,1,1);
  }
  else if (resultat==1){
    led(0,0,1);
  }
  else if (resultat==2){
    led(0,1,0);
  }
  else if (resultat==3){
    led(1,0,0);
  }
  RESULTAT=resultat;
  affich();
}

// Boucle d'attente du mode En Panne
void EnPanne(){
  tempo=0;
  resultat=3;
  while (tempo<=attente2){  
    led(1,0,0);
    delay(1000);
    led(0,0,0);
    delay(400);
    tempo=tempo+1;
    etatBouton = digitalRead(pinBouton);
    if (etatBouton==1){
      DebutDeTache();
      tempo=attente2+1;
    }
  }
}

// Boucle d'attente du mode Fin de tâche
void FinDeTache(){
  tempo=0;
  resultat=2;
  while (tempo<=attente2){  
    led(0,1,0);
    delay(1000);
    led(0,0,0);
    delay(400);
    tempo=tempo+1;
    etatBouton = digitalRead(pinBouton);
    if (etatBouton==1){
      EnPanne();
      tempo=attente2+1;
    }
  }
}

// Boucle d'attente du mode Début de tâche
void DebutDeTache(){
  tempo=0;
  resultat=1;
  while (tempo<=attente2){  
    led(0,0,1);
    delay(1000);
    led(0,0,0);
    delay(400);
    tempo=tempo+1;
    etatBouton = digitalRead(pinBouton);
    if (etatBouton==1){
      FinDeTache();
      tempo=attente2+1;
    }
  }
}

// Boucle du mode Edition
void Edition(){
  tempo=0;
  while (tempo<=attente){  
    led(1,1,1);
    delay(1000);
    led(0,0,0);
    delay(400);
    tempo=tempo+1;
    etatBouton = digitalRead(pinBouton);
    if (etatBouton==1){
      DebutDeTache();
      tempo=attente+1;
    }
  }
}

// Fonction de Rappel 
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrivé [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i = 0; i<length; i++) {
  Serial.print((char)payload[i]);
 }
 Serial.println();
}
 
void reconnect() {
 // Boucle jusqu'à être reconnecté
 while (!client.connected()) {
  Serial.print("Attente d'une connection MQTT...");
  // Attent d'être connecté
  if (client.connect("ESP8266Client")) {
    Serial.println("connecté");
  } 
  else {
    Serial.print("erreur, rc=");
    Serial.print(client.state());
    Serial.println(" nouvel essai dans 5 secondes");
    // Attente de 5 secondes avant de réessayer de se connecter
    delay(5000);
  }
 }
}

void principale(){
  etatBouton = digitalRead(pinBouton);
  if (etatBouton==1){
      compteur=0;
    }
  while (compteur<=boucle-1){  
    etatBouton = digitalRead(pinBouton);
    
    if (etatBouton==1){
      compteur=compteur+1;
    }
    else{
      compteur=boucle+1;
    }
    Serial.println(compteur);
    if (compteur==boucle){  
    Edition();
    Result();
    }
    delay(10);
  }
  affich();
}

// Fonction d'affichage (PUBLISH) sur le serveur MQTT
void affich(){
  if (!client.connected()) {
    reconnect();
  }
  client.publish("le_nom_du_topic", String(RESULTAT).c_str());
  client.loop();
  delay(1000);
}

// Fonction nécessaire à l'installation (initialisation, déclaration des ports, connexion au Wifi)
void setup() { 
 Serial.begin(9600);
 
 setup_wifi();
 client.setServer(mqtt_server, mqtt_port);
 client.setCallback(callback);
 
 Serial.println("Connecté");
 Serial.print("Serveur MQTT ");
 Serial.print(mqtt_server);
 Serial.print(":");
 Serial.println(String(mqtt_port)); 
 Serial.print("ESP8266 IP ");
 Serial.println(WiFi.localIP()); 
 Serial.println("Modbus RTU Master Online");
 pinMode(led_rouge, OUTPUT);
 pinMode(led_verte, OUTPUT);
 pinMode(led_bleue, OUTPUT);

}

// Boucle infinie
void loop() {
 principale();
}

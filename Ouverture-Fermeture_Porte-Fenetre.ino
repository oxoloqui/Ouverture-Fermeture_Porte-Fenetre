//----- Declaration des bibliothèques -------//
#include <Wire.h>
#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <NTPClient.h>

//----- Paramètre wifi -----//
const char* wifi_ssid = "XXXXXXXXXXXXX";              //Identifiant de la box
const char* wifi_password = "XXXXXXXXXXXX";           //Mdp de la box

//----- Paramètre de l'heure -----//
WiFiUDP ntpUDP;
NTPClient temps(ntpUDP, "fr.pool.ntp.org", 3600, 60000);

//----- Paramètre du serveur -----//
#define MQTT_HOST "XXX.XXX.XXX.XXX"                 // Adresse IP du broker
#define MQTT_PORT 1883                              // Port du broker                   
#define MQTT_PUB "test/topic"                       // Nom du salon du broker
#define MQTT_USER "XXXXXXX"                         // Nom de l'utilisateur du broker
#define MQTT_PASS "XXXXXXX"                         // Mdp du broker
AsyncMqttClient mqttClient;

#define REED_SWITCH 4                              // Port du commutateur

//----- Connexion WiFi -----//
void connectToWifi(){
   while(WiFi.status() != WL_CONNECTED){           // Tant que la connexion WiFi n'est pas établie 
    Serial.println("Attente de la connection WiFi");  
    delay(2000);
  }
  Serial.println("WiFi Connecté");                 // Affiche ("...") sur le terminal
}

//----- Connexion au serveur MQTT -----//
void connectToMqtt() {
  while (!mqttClient.connected()){                 // Tant que la connexion MQTT n'est pas établie 
      Serial.println("Attente de la connection MQTT");
      mqttClient.connect();                        // Connexion au serveur MQTT
      delay(2000);
  }
  Serial.println("MQTT Connecté");                 // Affiche ("...") sur le terminal
}

void setup() {
  Serial.begin(115200);                            // Vitesse de transmission 
  delay(1000);                                    
  WiFi.begin(wifi_ssid, wifi_password);            // Connexion WiFi via le ssid et le password
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);      // Initialisation de la connexion MQTT (Host et port)
  mqttClient.setCredentials(MQTT_USER, MQTT_PASS); // Initialisation de la connexion MQTT (User et Mdp)
  connectToWifi();                                 // Appel la fonction pour se connecter en WiFi
  connectToMqtt();                                 // Appel la fonction pour se connecter au serveur MQTT
  pinMode(REED_SWITCH, INPUT_PULLUP);              // Initialisation du commutateur en input pullup
}

void loop() {
  
  String msg = "";                                  // Déclaration de msg en chaine de caractère 
  if ((digitalRead(REED_SWITCH) == HIGH)){          // Si la porte s'ouvre
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4,LOW);   // Initialisation du reveil lorsque le commutateur se ferme
    msg = "Porte Ouverte !";                          
  }
  else if ((digitalRead(REED_SWITCH) == LOW)){      // Si la porte se ferme
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4,HIGH);  // Initialisation du reveil lorsque le commutateur s'ouvre
    msg = "Porte Fermée !";                            
  }
  
  delay(1000);                                      // Delay nessaire pour l'envoie du message
  temps.update();                                   // Mets à jour l'heure
  String heure = temps.getFormattedTime();          // Heure = l'heure actuelle
  msg += heure;                                     // Fusionne le msg et l'heure en une phrase
  uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB, 1, true, String(msg).c_str());  // Envoie le msg sur le serveur
  Serial.println(msg);                              // Affiche le msg sur le terminal                                       
  Serial.println("Mise en DEEPSLEEP");              // Affiche ("...") sur le terminal
  esp_deep_sleep_start();                           // Met l'ESP en deepsleep
}

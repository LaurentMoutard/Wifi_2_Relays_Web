// Type de carte : ESP32 Dev Module

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define brocheRelay1  23
#define brocheRelay2  19
// Connecter le oled 0.91 sur SDA<-> broche 21 et SCL <-> broche 22
#include "oled.h"
String message;

int etatRelay1;
int etatRelay2;

int previousMillis = 0;
int requestInterval = 10000;


char ssid[] = "XXXXXXXXXXXXX";
char pass[] = "XXXXXXXXXXXXXXXXXXX";

String serverName = "yourURLRoot";


String jsonFromServer, jsonToServer;

void gestionRelays() {
  digitalWrite(brocheRelay1, etatRelay1);
  digitalWrite(brocheRelay2, etatRelay2);
}

// envoi des informations en ligne
void sendToServer() {

  if (WiFi.status() == WL_CONNECTED) {

    Serial.print("Envoi au réseau : ");

    // création d'un objet appelé http réutilisable et sorti du moule HTTPClient
    HTTPClient http;

    // On Prépare un PATCH avec la valeur 180 au serveur TOM
    Serial.println("[HTTP] begin...");
    http.begin(serverName+"set.php");
    Serial.print("[HTTP] POST ... payload:");
    Serial.println(jsonToServer);

    // démarrer la connexion et envoyer les entêtes HTTP
    http.addHeader("Content-type", "application/json");
    int httpCode = http.POST(jsonToServer);   //Send the request
    String payload = http.getString();        //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection
  }
  else {
    Serial.print("DECONNECTE");
    ESP.restart();
  }
}

String getFromServer() {
  if (WiFi.status()== WL_CONNECTED) {
    
    HTTPClient http;
    
    Serial.println("[HTTP] begin...");
    http.begin(serverName+"get.php");
    
    Serial.println("[HTTP] GET...");
    // démarrer la connexion et envoyer les entêtes HTTP
    int httpCode = http.GET();
    
    // httpCode sera négatif si on rencontre une erreur
    if (httpCode > 0) {
      // les entêtes HTTP ont été envoyés et
      
      // si le serveur TOM répond par OK
      if (httpCode == HTTP_CODE_OK) {
        // alors on récupère la réponse sous forme de chaîne de caractères
        String payload = http.getString();
        //    Serial.println("[HTTP] GET... payload:");
        //    Serial.println(payload);
        return payload;
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      return ("");
    }
    http.end();
  }else{
    Serial.print("DECONNECTE");
    ESP.restart();
  }
}



void setup() {
  Wire.begin();
  u8g2.begin();
  oledPrint("Boot");

  // put your setup code here, to run once:
  Serial.begin ( 115200 );
  while (!Serial) {} //wait for serial
  Serial.println ("OK");
  
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
  
  Serial.println("Connected to wifi");
  printWifiStatus();
  oledPrint("Connecte !");
  pinMode(brocheRelay1, OUTPUT);
  pinMode(brocheRelay2, OUTPUT);


}

void loop() {
  processServer();
  
}

void processServer(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > requestInterval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    jsonFromServer = getFromServer();
    Serial.print("json récupéré : ");
    Serial.println(jsonFromServer);
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonFromServer);
    if (error){
      return;
    }
    String equipement2 = doc["equipement2"];
    String equipement3 = doc["equipement3"];
    
    Serial.print("equipement2 : ");
    Serial.println(equipement2);
    Serial.print("equipement3 : ");
    Serial.println(equipement3);

    if (equipement2 == "off"){
      etatRelay1 = 1;
    }else{
      etatRelay1 = 0;
    }
    if (equipement3 == "off"){
      etatRelay2 = 1;
    }else{
      etatRelay2 = 0;
    }
    gestionRelays();
    jsonToServer = "";
    DynamicJsonDocument doc2(1024);

    doc2["equipement2Set"] = doc["equipement2"];
    doc2["equipement3Set"] = doc["equipement3"];
    
    serializeJson(doc2, jsonToServer);
    sendToServer(); // envoi des informations en ligne

    message = "1:" + equipement2 + " - 2:" + equipement3;
    Serial.print("message : ");
    Serial.println(message);
    oledPrint(message);
   
  }
}
    

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
        

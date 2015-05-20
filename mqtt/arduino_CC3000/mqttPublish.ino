/****************************************************************

   MQTT : 
    publish on "sujet"
    repeat on "sujet" from "commande"
    "
Resources:
Include SPI.h, SFE_CC3000.h, and SFE_CC3000_Client.h

Tested with Arduino UNO R3 with CC3000 sparkfun Shield
****************************************************************/

#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <PubSubClient.h>

// Pins
#define CC3000_INT      2   
#define CC3000_EN       7   
#define CC3000_CS       10  

#define IP_ADDR_LEN     4   // Length of IP address in bytes

// Constants
char ap_ssid[] = "cbrun_devnet";             
char ap_password[] = "monfilsquentin";       
unsigned int ap_security = WLAN_SEC_WPA2; 
unsigned int timeout = 30000;            

// MQTT server : on linux "mosquitto -v"
byte server[] = {192, 168, 50, 200 };


void callback(char* topic, byte* payload, unsigned int length);

SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);
PubSubClient mqttclient(server, 1883, callback, client);

// Callback function pour le subscribe
void callback(char* topic, byte* payload, unsigned int length) {
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length);
  memcpy(p,payload,length);
  mqttclient.publish("sujet", p, length);
  free(p);
}

void setup() {
  ConnectionInfo connection_info;
  int i;
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println("-- MQTT and Wifi connection --");
  
  if ( wifi.init() ) {
    Serial.println("CC3000 initialization complete");
  } else {
    Serial.println("Something went wrong during CC3000 init!");
  }
  
  // Connect using DHCP
  Serial.print("Connecting to SSID: ");
  Serial.println(ap_ssid);
  if(!wifi.connect(ap_ssid, ap_security, ap_password, timeout)) {
    Serial.println("Error: Could not connect to AP");
  }
  
  // Gather connection details and print IP address
  if ( !wifi.getConnectionInfo(connection_info) ) {
    Serial.println("Error: Could not obtain connection details");
  } else {
    Serial.print("IP Address: ");
    for (i = 0; i < IP_ADDR_LEN; i++) {
      Serial.print(connection_info.ip_address[i]);
      if ( i < IP_ADDR_LEN - 1 ) {
        Serial.print(".");
      }
    }
    Serial.println();
  }
  
  if(mqttclient.connect("arduinoClient")) {
    mqttclient.publish("sujet","Bonjour le monde");
    mqttclient.subscribe("commande");   
  }
}

void loop() {
   mqttclient.loop();  
}

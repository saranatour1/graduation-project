
#include <WiFi.h> 
const char* ssid     = "ssid";
const char* password = "password";

//unit8_t masterCustomMac=[]={0x24,0xC4,0x9A,0xA5,0xA1};
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
delay(10);
Serial.println();
WiFi.disconnect(true);
WiFi.mode(WIFI_STA);
//esp_wifi_set_mac(ESP_IF_WIFI_STA,&masterCustomMac[0]);
Serial.print("MAC >>");
Serial.println(WiFi.macAddress());
}

void loop() {
 delay(1);
  // put your main code here, to run repeatedly:

}

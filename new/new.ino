#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <esp_wpa2.h>
#include <esp_wifi.h>

#ifndef SECRETS

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char EAP_IDENTITY[] = SECRET_EAP_IDENTITY;   // your network password
char EAP_PASSWORD[] = SECRET_EAP_PASSWORD;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

static const char incommon_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIF+TCCA+GgAwIBAgIQRyDQ+oVGGn4XoWQCkYRjdDANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQx
MDA2MDAwMDAwWhcNMjQxMDA1MjM1OTU5WjB2MQswCQYDVQQGEwJVUzELMAkGA1UE
CBMCTUkxEjAQBgNVBAcTCUFubiBBcmJvcjESMBAGA1UEChMJSW50ZXJuZXQyMREw
DwYDVQQLEwhJbkNvbW1vbjEfMB0GA1UEAxMWSW5Db21tb24gUlNBIFNlcnZlciBD
QTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJwb8bsvf2MYFVFRVA+e
xU5NEFj6MJsXKZDmMwysE1N8VJG06thum4ltuzM+j9INpun5uukNDBqeso7JcC7v
HgV9lestjaKpTbOc5/MZNrun8XzmCB5hJ0R6lvSoNNviQsil2zfVtefkQnI/tBPP
iwckRR6MkYNGuQmm/BijBgLsNI0yZpUn6uGX6Ns1oytW61fo8BBZ321wDGZq0GTl
qKOYMa0dYtX6kuOaQ80tNfvZnjNbRX3EhigsZhLI2w8ZMA0/6fDqSl5AB8f2IHpT
eIFken5FahZv9JNYyWL7KSd9oX8hzudPR9aKVuDjZvjs3YncJowZaDuNi+L7RyML
fzcCAwEAAaOCAW4wggFqMB8GA1UdIwQYMBaAFFN5v1qqK0rPVIDh2JvAnfKyA2bL
MB0GA1UdDgQWBBQeBaN3j2yW4luHS6a0hqxxAAznODAOBgNVHQ8BAf8EBAMCAYYw
EgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUH
AwIwGwYDVR0gBBQwEjAGBgRVHSAAMAgGBmeBDAECAjBQBgNVHR8ESTBHMEWgQ6BB
hj9odHRwOi8vY3JsLnVzZXJ0cnVzdC5jb20vVVNFUlRydXN0UlNBQ2VydGlmaWNh
dGlvbkF1dGhvcml0eS5jcmwwdgYIKwYBBQUHAQEEajBoMD8GCCsGAQUFBzAChjNo
dHRwOi8vY3J0LnVzZXJ0cnVzdC5jb20vVVNFUlRydXN0UlNBQWRkVHJ1c3RDQS5j
cnQwJQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnVzZXJ0cnVzdC5jb20wDQYJKoZI
hvcNAQEMBQADggIBAC0RBjjW29dYaK+qOGcXjeIT16MUJNkGE+vrkS/fT2ctyNMU
11ZlUp5uH5gIjppIG8GLWZqjV5vbhvhZQPwZsHURKsISNrqOcooGTie3jVgU0W+0
+Wj8mN2knCVANt69F2YrA394gbGAdJ5fOrQmL2pIhDY0jqco74fzYefbZ/VS29fR
5jBxu4uj1P+5ZImem4Gbj1e4ZEzVBhmO55GFfBjRidj26h1oFBHZ7heDH1Bjzw72
hipu47Gkyfr2NEx3KoCGMLCj3Btx7ASn5Ji8FoU+hCazwOU1VX55mKPU1I2250Lo
RCASN18JyfsD5PVldJbtyrmz9gn/TKbRXTr80U2q5JhyvjhLf4lOJo/UzL5WCXED
Smyj4jWG3R7Z8TED9xNNCxGBMXnMete+3PvzdhssvbORDwBZByogQ9xL2LUZFI/i
eoQp0UM/L8zfP527vWjEzuDN5xwxMnhi+vCToh7J159o5ah29mP+aJnvujbXEnGa
nrNxHzu+AGOePV8hwrGGG7hOIcPDQwkuYwzN/xT29iLp/cqf9ZhEtkGcQcIImH3b
oJ8ifsCnSbu0GB9L06Yqh7lcyvKDTEADslIaeSEINxhO2Y1fmcYFX/Fqrrp1WnhH
OjplXuXE0OPa0utaKC25Aplgom88L2Z8mEWcyfoB7zKOfD759AN7JKZWCYwk
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw
MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV
BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU
aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy
dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B
3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY
tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/
Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2
VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT
79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6
c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT
Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l
c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee
UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE
Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd
BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G
A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF
Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO
VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3
ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs
8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR
iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze
Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ
XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/
qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB
VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB
L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG
jjxDah2nGN59PRbxYvnKkKj9
-----END CERTIFICATE-----
)EOF";

// Obtain the latest from :
// https://documentation.its.umich.edu/content/wifi-manually-configuring-wpa2-enterprise-other-wifi-enabled-devices-unsupported-devices
// Use the "Intermediate CA" file: http://www.itcom.itd.umich.edu/downloads/wifi/incommon_ras_server_ca.cer ^^^^^^^^^^^^^^^^^^^^^^^^^^^
// NO need for "Root CA: UserTrust RSA Cerification Authority" file

#endif

// this sample code provided by www.programmingboss.com
#define RXp2 16
#define TXp2 17
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

uint8_t counter = 0;
unsigned long previousMillisWiFi = 0;
char timeStringBuff[50];
//uint8_t masterCustomMac[] = {0x24, 0x0A, 0xC4, 0x9A, 0xA5, 0xA1}; // 24:0a:c4:9a:a5:a1

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
String values,data;

String myStatus = "";

void printLocalTime(bool printToSerial = false)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.print(F("NTP sync failed "));
    return;
  }
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  if(printToSerial) Serial.println(timeStringBuff);
}


void setup() {
  //Initializes the serial connection at 9600 get sensor data from arduino.
   Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  delay(1000);
  Serial.println();
  WiFi.disconnect(true); //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);   //init wifi mode
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &masterCustomMac[0]);
  Serial.print("MAC >> ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Connecting to WiFi: %s ", ssid);
  esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)incommon_ca, strlen(incommon_ca) + 1);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&config);
  WiFi.begin(ssid);
  if (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
    counter++;
    if (counter >= 60)
    { //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println(F(" connected!"));
  Serial.print(F("IP address set: "));
  Serial.println(WiFi.localIP()); //print LAN I
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
void loop() {
 if ((millis() - lastTime) > timerDelay) {
 data=Serial2.readString();
 unsigned long currentMillis = millis();
  if (WiFi.status() == WL_CONNECTED)
  {              //if we are connected to Eduroam network
    counter = 0; //reset counter
    if (currentMillis - previousMillisWiFi >= 15 * 1000)
    {
      printLocalTime(true);
      previousMillisWiFi = currentMillis;
      Serial.print(F("Wifi is still connected with IP: "));
      Serial.println(WiFi.localIP()); //inform user about his IP address
    }
  }
  else if (WiFi.status() != WL_CONNECTED)
  { //if we lost connection, retry
    WiFi.begin(ssid);
    Serial.printf("Connecting to WiFi: %s ", ssid);
    while (WiFi.status() != WL_CONNECTED)
    { //during lost connection, print dots
      delay(500);
      Serial.print(F("."));
      counter++;
      if (counter >= 60)
      { //30 seconds timeout - reset board
        ESP.restart();
      }
    }
    Serial.println(F(" connected!"));
  }
  values=data;
  //get comma indexes from values variable
  int  fristCommaIndex = values.indexOf(','); //tds value 
  float  secondCommaIndex = values.indexOf(',', fristCommaIndex+1); // water tempreture 
  float  thirdCommaIndex = values.indexOf(',', secondCommaIndex + 1); //pH value 
  float  fourthCommaIndex = values.indexOf(',', thirdCommaIndex + 1);// surrounding tempreture 
  int  fifthCommaIndex = values.indexOf(',', fourthCommaIndex + 1); //humidity 
  int  sixthCommaIndex = values.indexOf(',', fifthCommaIndex + 1); //moisture
  int  seventhCommaIndex = values.indexOf(',', sixthCommaIndex + 1);//water level
  //get sensors data from values variable by  spliting by commas and put in to variables  
  String num1 = values.substring(0,fristCommaIndex);
  String num2 = values.substring(fristCommaIndex+1, secondCommaIndex);
  String num3 = values.substring(secondCommaIndex+1,thirdCommaIndex);
  String num4 = values.substring(thirdCommaIndex+1,fourthCommaIndex);
  String num5 = values.substring(fourthCommaIndex+1,fifthCommaIndex);
  String num6 = values.substring(fifthCommaIndex+1,sixthCommaIndex);
  String num7 = values.substring(sixthCommaIndex+1,seventhCommaIndex);
  int TDS_value=num1.toInt(); 
  float water_temp_value= num2.toFloat();
  float pH_value=num3.toFloat();
  float surrounding_temp_value= num4.toFloat();
  float humidity_value =num5.toInt();
  int moisture_level=num6.toInt();
  int water_level=num7.toInt();
   Serial.println(data); 
  Serial.println(TDS_value);
  Serial.println(water_temp_value);
  Serial.println(pH_value);
  Serial.println(surrounding_temp_value);
  Serial.println(humidity_value);
  Serial.println(moisture_level);
  Serial.println(water_level);
    // write to the ThingSpeak channel
  ThingSpeak.setField(1, TDS_value);
  ThingSpeak.setField(2, water_temp_value);
  ThingSpeak.setField(3, pH_value);
  ThingSpeak.setField(4, surrounding_temp_value);
  ThingSpeak.setField(5, humidity_value);
  ThingSpeak.setField(6, moisture_level);
  ThingSpeak.setField(7, water_level);
  
if(TDS_value>512 &&TDS_value<840 )
{
  myStatus = String("No need to add more nutirents");
  }
  else if (TDS_value<512)
  {
    myStatus = String("nutrients must be added to the tank");
    }
    else if((TDS_value>512))
    {
         myStatus = String("more nutrients are present, please add more water"); 
      }
if ( pH_value < 6.5 &&  pH_value> 5.5)
{
  myStatus = String("No need to control the values of pH!");
  } 
else if (pH_value<5.5 )
{
  myStatus = String("the pH level is low, please check the water level");
  }
  else if(pH_value<6.5)
  {
    myStatus = String("the pH level is high, please add pH down.");
    }
 ThingSpeak.setStatus(myStatus);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
 }
  }

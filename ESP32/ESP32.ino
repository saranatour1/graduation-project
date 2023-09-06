#include <OneWire.h>     
#include <DallasTemperature.h>
#include <DHT.h>
#include <EEPROM.h>
//#include <GravityTDS.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "secrets.h"
#include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros
#include <WiFiClientSecure.h>
#include "esp_wpa2.h"
#include <Wire.h>
char ssid[] = SECRET_SSID;   // your network SSID (name) 
//char pass[] = SECRET_PASS;   // your network password
char EAP_IDENTITY[] = SECRET_EAP_IDENTITY;   // your network SSID (name) 
char EAP_PASSWORD[] = SECRET_EAP_PASSWORD;
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIEsTCCA5mgAwIBAgIQCKWiRs1LXIyD1wK0u6tTSTANBgkqhkiG9w0BAQsFADBh\n" \
                           "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
                           "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
                           "QTAeFw0xNzExMDYxMjIzMzNaFw0yNzExMDYxMjIzMzNaMF4xCzAJBgNVBAYTAlVT\n" \
                           "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
                           "b20xHTAbBgNVBAMTFFJhcGlkU1NMIFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n" \
                           "AQEFAAOCAQ8AMIIBCgKCAQEA5S2oihEo9nnpezoziDtx4WWLLCll/e0t1EYemE5n\n" \
                           "+MgP5viaHLy+VpHP+ndX5D18INIuuAV8wFq26KF5U0WNIZiQp6mLtIWjUeWDPA28\n" \
                           "OeyhTlj9TLk2beytbtFU6ypbpWUltmvY5V8ngspC7nFRNCjpfnDED2kRyJzO8yoK\n" \
                           "MFz4J4JE8N7NA1uJwUEFMUvHLs0scLoPZkKcewIRm1RV2AxmFQxJkdf7YN9Pckki\n" \
                           "f2Xgm3b48BZn0zf0qXsSeGu84ua9gwzjzI7tbTBjayTpT+/XpWuBVv6fvarI6bik\n" \
                           "KB859OSGQuw73XXgeuFwEPHTIRoUtkzu3/EQ+LtwznkkdQIDAQABo4IBZjCCAWIw\n" \
                           "HQYDVR0OBBYEFFPKF1n8a8ADIS8aruSqqByCVtp1MB8GA1UdIwQYMBaAFAPeUDVW\n" \
                           "0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n" \
                           "BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n" \
                           "MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n" \
                           "OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n" \
                           "YWxSb290Q0EuY3JsMGMGA1UdIARcMFowNwYJYIZIAYb9bAECMCowKAYIKwYBBQUH\n" \
                           "AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAEBMAgG\n" \
                           "BmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcNAQELBQADggEBAH4jx/LKNW5ZklFc\n" \
                           "YWs8Ejbm0nyzKeZC2KOVYR7P8gevKyslWm4Xo4BSzKr235FsJ4aFt6yAiv1eY0tZ\n" \
                           "/ZN18bOGSGStoEc/JE4ocIzr8P5Mg11kRYHbmgYnr1Rxeki5mSeb39DGxTpJD4kG\n" \
                           "hs5lXNoo4conUiiJwKaqH7vh2baryd8pMISag83JUqyVGc2tWPpO0329/CWq2kry\n" \
                           "qv66OSMjwulUz0dXf4OHQasR7CNfIr+4KScc6ABlQ5RDF86PGeE6kdwSQkFiB/cQ\n" \
                           "ysNyq0jEDQTkfa2pjmuWtMCNbBnhFXBYejfubIhaUbEv2FOQB3dCav+FPg5eEveX\n" \
                           "TVyMnGo=\n" \
                           "-----END CERTIFICATE-----\n";
// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client
WiFiClientSecure client;
int counter = 0;
String myStatus = "";

//LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTPIN 15 
#define DHTTYPE DHT11
float h , t , f;
DHT dht(DHTPIN, DHTTYPE);
// TDS AND TEMPRETURE SENSOR 
const byte tds_sensor = 2; const byte one_wire_bus = 16; // Dallas Temperature Sensor
float aref = 4.3;
float ec = 0;  int tds = 0; float waterTemp = 0; float ecCalibration = 1;  
OneWire oneWire(one_wire_bus);
DallasTemperature dallasTemperature(&oneWire);

int Level_sensor_pin = 17;
int Pump_pin = 12;
int moisture_pin = 4;

#define pHsensor 0           //pH meter Analog output to Arduino Analog Input 0
#define Offset -1.5         //deviation compensate // check ASAP 
#define ArrayLenth  40        //times of collection
int pHArray[ArrayLenth];      //Store the average value of the sensor feedback
int pHArrayIndex = 0;

//some variables for the timing
bool eng = false;
bool dateENG = false;

String s;
int days = 0;
int day = 13;
int month = 4;
int year = 2022;
int hour =14;
int minit = 8;
int sec = 58;

int GROW_pin = 14;

int pumpState = LOW;// on 
int Grow_light_state =LOW;// on 

unsigned long previousMillis = 0;
const long interval = 5000; // for 1 minitue for pump starting

unsigned long Mon_milles = 0;
const long Mon_INTERVAL = 1000;

static float pHValue, voltage;
int Water_level;
int moisture=0;

unsigned long prevMillis = 0;
unsigned long curMillis;
// timing for the lcd display

unsigned long prevMillis_2 = 0;
unsigned long curMillis_2;

unsigned long previousMil = 0;
unsigned long currentMil = 0;
const long period = 3000;

unsigned long previousMil_1 = 0;
unsigned long currentMil_1 = 0;
const long period_1 = 40000;

void setup()
{
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.init();
  pinMode(Level_sensor_pin, INPUT);
  pinMode(Pump_pin, OUTPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(GROW_pin , OUTPUT);
  pinMode(moisture_pin,INPUT);
    pinMode(tds_sensor,INPUT);
  pinMode(one_wire_bus,INPUT);

   pinMode(pHsensor,INPUT);
 // pinMode(,);
  //pinMode(,);
  //pinMode(,);
  //pinMode(,);
 
if (millis() - previousMillis >= 10000){
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
 // esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&config);
  WiFi.begin(ssid); //connect to wifi
}
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    counter++;
  }
  client.setCACert(test_root_ca);
  //client.setCertificate(test_client_key); // for client verification - certificate
  //client.setPrivateKey(test_client_cert);  // for client verification - private key
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP

  ThingSpeak.begin(client);  // Initialize ThingSpeak
   if (!Serial) {
   myStatus = String("Program has started ");  
   ThingSpeak.setStatus(myStatus);
    // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  dallasTemperature.begin();
  dht.begin();
}

void loop()
{
 if (WiFi.status() == WL_CONNECTED) { //if we are connected to eduroam network
    counter = 0; //reset counter
    Serial.println("Wifi is still connected with IP: ");
    Serial.println(WiFi.localIP());   //inform user about his IP address
  } else if (WiFi.status() != WL_CONNECTED) { //if we lost connection, retry
    WiFi.begin(ssid);
  }
  if (millis() - previousMillis >= 5000){
  while (WiFi.status() != WL_CONNECTED) { //during lost connection, print dots
    Serial.print(".");
    counter++;
    if (counter >= 60) { //30 seconds timeout - reset board
      ESP.restart();
    }
  }
  }
  Water_level = digitalRead(Level_sensor_pin);
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
//pump repeatance part

  moisture = analogRead(moisture_pin); 
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) // to repeat every hour for the interval of 1 minitue
  {
    previousMillis = currentMillis; // save the last time the pump worked
    if (pumpState == LOW && minit == 59 && Water_level == 1 )
    { pumpState = HIGH; digitalWrite(Pump_pin, pumpState); Serial.println(" time to pump water for 1 minitue");}
    else if (minit !=59 || Water_level == 0)
    { pumpState = LOW; digitalWrite(Pump_pin, pumpState); Serial.println(" time to stop pumping");}
  }
  if (currentMillis - previousMillis >= 80000)
  {
    Serial.println(moisture); 
    if (moisture < 200)
    {
      Serial.println("the Growing medium area is dry, check pump settings and water level");
      }
      else if(moisture < 600)
      {
      Serial.println("the Growing medium area is mid-dry,setting is OK");
      }
      else if(moisture < 800)
      {
     Serial.println("the Growing medium area is ok");
      }
    }
  
  //Grow lights
  if (currentMillis - previousMillis >= 7000){
  if (Grow_light_state== LOW || hour==20 || hour==21 || hour==22 || hour==23 || hour==00 ||hour==1 || hour==2 || hour==3 || hour==4 || hour==5 || hour==6   )
  { Grow_light_state== HIGH; Serial.println("the grow light is on" ); }
  else if ( hour== 7 && minit==15 && Grow_light_state== HIGH) 
  { Grow_light_state== LOW;  Serial.println("the grow light is off" ); }
  digitalWrite(GROW_pin,Grow_light_state);  }
 //level alarm
  /*if (Water_level == LOW)
  { LED_safe_State = HIGH; LED_unsafe_State = LOW; ALARM_start_State = LOW;}
  else if (Water_level == HIGH)
  { LED_safe_State = LOW; LED_unsafe_State = HIGH; ALARM_start_State = HIGH; }
  digitalWrite(LED_safe, LED_safe_State);  digitalWrite(LED_unsafe, LED_unsafe_State);  digitalWrite(ALARM_start, ALARM_start_State);
*/
  //sensors monitering, the TDS and pH with no
  if (currentMillis - Mon_milles >= Mon_INTERVAL )
  {
    pH_fun();
   Serial.print("Voltage:");
   Serial.print(voltage,2);
   Serial.print("    pH value: ");
   Serial.println(pHValue,2);
   Serial.println(" time to start monitering");
    readTdsQuick(); // tempreture and TDS and then pH sensor readings.
    surr_temp();
    //LDR_FUN();
  }
  //check other syntax, add the other two pumps
  if (sec == 60) {
    minit += 1;
    sec = 0;
    if (minit == 60) {
      hour += 1;
      minit = 0;
      if (hour == 24) {
        day += 1;
        days += 1;
        hour = 0;
        if (day == 31) {
          month += 1;
          day = 1;
          if (month == 13) {
            year += 1;
            month = 1;
          }
        }
      }
    }
  }
  if (millis() - previousMillis >= 4938.5 && sec% 5 == 0)
  {
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minit);
    Serial.print(":");
    Serial.print(sec);
    Serial.print("________");
    Serial.println("\n");
    Serial.print(day);
    Serial.print(":");
    Serial.print(month);
    Serial.print(":");
    Serial.print(year);
    Serial.println("\n");
  }
  
  if (eng == false) { eng = true; }
  currentMil = millis();
  //3 sn lik period gerçekleşti mi kontrolü
  if (currentMil - previousMil >= period) {
    previousMil = currentMil;
    if (eng == true) {
      s = DOW("ENG", days);
      showDefaultENG(s);
    }
    delay(1000);
    sec += 1;
  }
//LCD display code 
curMillis_2 = millis();
  if ((curMillis_2 - prevMillis_2) >=5000 && minit==0|| minit==8 || minit==9 ||minit==11||minit==21 || minit==31 ||minit==41 || minit==51  )
  {
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print("Water level=");
    Serial.println("1");
    if (Water_level == 1)
    {
      lcd.setCursor(12, 1);
      lcd.print("HI");
      lcd.setCursor(14, 1); lcd.print("       ");
    }
    else if (Water_level == 0)
    {
      lcd.setCursor(12, 1);
      lcd.print("LOW");
      lcd.setCursor(15, 1); lcd.print("    ");
    }
  }
  if (( curMillis_2- prevMillis_2) >= 5000 && minit==1||  minit==12||minit==22 || minit==10 ||minit==18 || minit==32 ||minit==42 || minit==52  )
  {
    //issues fixed
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("Humidity:"));
    lcd.setCursor(9, 1);
    lcd.print(h);
    lcd.setCursor(11, 1);
    lcd.print("%"); 
    lcd.setCursor(12, 1); 
    lcd.print("    "); 
    Serial.println("2");
  }
  if (( curMillis_2- prevMillis_2) >= 5000 && minit==2|| minit==13||minit==23 || minit== 19 || minit==20 || minit==33 ||minit==43 || minit==53)
  {
    prevMillis_2 = curMillis_2;
   lcd.setCursor(0, 1);
    lcd.print(F("Temp:"));
   lcd.setCursor(5, 1);
    lcd.print(t);
    lcd.setCursor(10, 1);
    lcd.print((char)223);
    lcd.print("C");
    Serial.println("3");
  }
  if (( curMillis_2- prevMillis_2) >= 5000 &&minit==9||  minit==3 && minit==14|| minit== 28|| minit== 29|| minit==24||minit==34 || minit==44 ||minit==54)
  {
   //issues fixed but the lcd does not print the pH on virtual lvd 
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print("pH value:");
    lcd.setCursor(9,1);
    lcd.print(pHValue,2);
    Serial.println("4");
  }
  if ((curMillis_2 - prevMillis_2) >= 5000 &&minit==7 || minit==4 ||minit==59 || minit== 30|| minit==38||  minit==15||minit==25 || minit==35 ||minit==45 || minit==55)
  {
  //issues fixed 
   prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("TDS:")); 
    lcd.setCursor(4, 1);
    lcd.print(tds);
    lcd.setCursor(7, 1);
    lcd.print("     ");
    lcd.setCursor(12, 1);
    lcd.print(" ");
    lcd.setCursor(13, 1);
    lcd.print("ppm");
    Serial.println("5");
  }
  if (( curMillis_2- prevMillis_2) >= 5000 && minit==5|| minit==16|| minit ==58 ||minit==26 || minit== 39|| minit==40 || minit==36 ||minit==46 || minit==56)
  {
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("EC:")); 
    lcd.setCursor(3, 1);
    lcd.print(ec,2);
    lcd.setCursor(12, 1);
    lcd.print("uS/m"); 
    Serial.println("6");
   
  }
  if (( curMillis_2 - prevMillis_2) >= 5000  && minit==6|| minit==17||minit==27 || minit==48 || minit==49 || minit==37 ||minit==47 || minit==57 ||minit==50 )
  {
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("Tempe 2:")); 
    lcd.setCursor(8, 1);
    lcd.print(waterTemp, 2);
    lcd.setCursor(13, 1);
    lcd.print("    ");
    //lcd.setCursor(15, 1);
    //lcd.print("   ");
    Serial.println("7");
  }
if( (tds)<560){
    myStatus = String("more nutrients need to be added"); 
  }
  else if((tds)> 840)
  {
    myStatus = String("more water needs to be added,check the water level. "); 
  }
    if(pHValue< 5.5){
    myStatus = String("the water is acidic, please check the water level ");
  }
  else if (pHValue> 6.5 ) {
    myStatus = String("the water is alkinite, please add pH down");
  }
  else 
  {
  myStatus = String("everything is normal");
  }
  ThingSpeak.setStatus(myStatus);
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3,(tds));
  ThingSpeak.setField(4,(waterTemp, 2));
  ThingSpeak.setField(5,(pHValue,2));
  ThingSpeak.setField(6,(ec,2));
}


void showDefaultENG(String dow) {
  int hourtemp = 0;
  currentMil_1 = millis();
  //3 sn lik period gerçekleşti mi kontrolü
  if (currentMil_1 - previousMil_1 >= period_1) {
    previousMil_1 = currentMil_1;
    lcd.setCursor(0, 0); lcd.print(0);
    if (day < 10) {
      lcd.setCursor(10, 0); lcd.print(" ");
      lcd.setCursor(0, 0); lcd.print(0);
      lcd.setCursor(1, 0); lcd.print(day);
    }
    else {
      lcd.setCursor(10, 0); lcd.print(" ");
      lcd.setCursor(0, 0); lcd.print(day);
    }
    lcd.setCursor(2, 0); lcd.print(":");

    if (month < 10) {
      lcd.setCursor(10, 0); lcd.print(" ");
      lcd.setCursor(3, 0); lcd.print(0);
      lcd.setCursor(4, 0); lcd.print(month);
    }
    else {
      lcd.setCursor(10, 0); lcd.print(" ");
      lcd.setCursor(3, 0); lcd.print(month);
    }
    lcd.setCursor(5, 0); lcd.print(":");
    lcd.setCursor(6, 0); lcd.print(year);
    lcd.setCursor(13, 0); lcd.print(dow);
  }
  else {
    if (hour > 12) {
      hourtemp = (hour - 12);
    }
    else if (hour == 0) {
      hourtemp = 12; //12.00 AM
    }
    if (hourtemp < 10) {
      lcd.setCursor(0, 0); lcd.print(0);
      lcd.setCursor(1, 0); lcd.print(hourtemp);
    }
    else {
      lcd.setCursor(0, 0); lcd.print(hourtemp);
    }
    lcd.setCursor(2, 0); lcd.print(":");

    if (minit < 10) {
      lcd.setCursor(3, 0); lcd.print(0);
      lcd.setCursor(4, 0); lcd.print(minit);
    }
    else {
      lcd.setCursor(3, 0); lcd.print(minit);
    }
    lcd.setCursor(5, 0); lcd.print(":");

    if (sec < 10) {
      lcd.setCursor(6, 0); lcd.print(0);
      lcd.setCursor(7, 0); lcd.print(sec);
    }
    else {
      lcd.setCursor(6, 0); lcd.print(sec);
    }
    if (hour < 12) {
      lcd.setCursor(8, 0); lcd.print(" ");
      lcd.setCursor(9, 0); lcd.print("AM");

    }
    else {
      lcd.setCursor(8, 0); lcd.print(" ");
      lcd.setCursor(9, 0); lcd.print("PM");
    }

    lcd.setCursor(11, 0); lcd.print(" ");
    lcd.setCursor(13, 0); lcd.print("ENG");
  }
}

String DOW(String lng, int days) {
  String thisDay;
  if (lng == "ENG") {
    if (days % 7 == 0) {
      thisDay = "WED";
    }
    else if (days % 7 == 1) {
      thisDay = "THU";
    }
    else if (days % 7 == 2) {
      thisDay = "FRI";
    }
    else if (days % 7 == 3) {
      thisDay = "SAT";
    }
    else if (days % 7 == 4) {
      thisDay = "SUN";
    }
    else if (days % 7 == 5) {
      thisDay = "MON";
    }
    else if (days % 7 == 6) {
      thisDay = "TUE";
    }
  }
  return thisDay;
}

void surr_temp()
{
  // Compute heat index in Fahrenheit (the default)
  //surrounding sensors
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}
double avergearray(int* arr, int number) //pH function 
{
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;      //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}
void pH_fun()
{
   pHArray[pHArrayIndex++]=analogRead(pHsensor);
   if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
   voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
   pHValue = 3.5*voltage+Offset;
  }
void readTdsQuick() {
  dallasTemperature.requestTemperatures();
  waterTemp = dallasTemperature.getTempCByIndex(0);
  float rawEc = analogRead(tds_sensor) * aref / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float temperatureCoefficient = 1.0 + 0.02 * (waterTemp - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  ec = (rawEc / temperatureCoefficient) * ecCalibration; // temperature and calibration compensation
  tds = (133.42 * pow(ec, 3) - 255.86 * ec * ec + 857.39 * ec) * 0.5; //convert voltage value to tds value
  Serial.println(F("TDS:")); Serial.println(tds);
  Serial.print(F("EC:")); Serial.println(ec, 2);
  Serial.print(F("Temperature:")); Serial.println(waterTemp, 2);
}

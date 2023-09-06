#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

// this sample code provided by www.programmingboss.com
#define RXp2 16
#define TXp2 17
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
String values,data;

String myStatus = "";
void setup() {
  //Initializes the serial connection at 9600 get sensor data from arduino.
   Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  delay(1000);
  if (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
void loop() {
 if ((millis() - lastTime) > timerDelay) {
    data=Serial2.readString();
if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
    } 
    Serial.println("\nConnected.");
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

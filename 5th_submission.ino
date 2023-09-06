/*
 * DHT11==> pin 13 
 * LCD==>20+21 SDA +SCL 
 * Relay IN1 pump==> 2 
 *       IN2 Growlights ==>3
 * tempreture sensor ==>  4       
 * TDS ==> A0 
 * pH ==>A1 
 * level==>A2 
 * LDR==>A3 
 * moisture=12 
 * LED green ==> 7 
 * LED yellow==>6 
 * LED RED ==> 5
 */
#include <OneWire.h>     
#include <DallasTemperature.h>
#include <DHT.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <LiquidCrystal_I2C.h>

//LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTPIN 52 
#define DHTTYPE DHT11
float h , t , f;
DHT dht(DHTPIN, DHTTYPE);
// TDS AND TEMPRETURE SENSOR 
namespace pin {const byte tds_sensor = A0; const byte one_wire_bus = 53;}
 // Dallas Temperature Sensor
namespace device {float aref = 4.3;}
namespace sensor {float ec = 0; unsigned int tds = 0; float waterTemp = 0; float ecCalibration = 1; } 
OneWire oneWire(pin::one_wire_bus);
DallasTemperature dallasTemperature(&oneWire);

int Level_sensor_pin = 9;
int Pump_pin = 41;
int moisture_pin = A4;

#define pHsensor A1           //pH meter Analog output to Arduino Analog Input 0
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
int min = 05;
int sec = 58;

int GROW_pin = 43;

int pumpState = HIGH;
int LED_safe_State = LOW;
int LED_unsafe_State = LOW;
int ALARM_start_State = LOW;
int Grow_light_state =HIGH;

int LED_safe = 7;
int LED_unsafe = 6;
int ALARM_start = 5;

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
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.init();
  pinMode(Level_sensor_pin, INPUT);
  pinMode(Pump_pin, OUTPUT);
  //pinMode(Buzzer_pin,OUTPUT);
  pinMode(LED_safe, OUTPUT);
  pinMode(ALARM_start, OUTPUT);
  pinMode(LED_unsafe, OUTPUT);
  dallasTemperature.begin();
  dht.begin();
  Serial.begin(9600);
}

void loop()
{
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
    if (pumpState == HIGH && min == 59 && Water_level == 1 )
    { pumpState = LOW; digitalWrite(Pump_pin, pumpState); Serial.println(" time to pump water for 1 minitue");}
    else if (min !=59 || Water_level == 0)
    { pumpState = HIGH; digitalWrite(Pump_pin, pumpState); Serial.println(" time to stop pumping");}
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
  if (Grow_light_state== HIGH || hour==20 || hour==21 || hour==22 || hour==23 || hour==00 ||hour==1 || hour==2 || hour==3 || hour==4 || hour==5 || hour==6   )
  { Grow_light_state== LOW; Serial.println("the grow light is on" ); }
  else if ( hour== 7 && min==15 && Grow_light_state== LOW) 
  { Grow_light_state== HIGH;  Serial.println("the grow light is off" ); }
  digitalWrite(GROW_pin,Grow_light_state);  }
 //level alarm
  if (Water_level == LOW)
  { LED_safe_State = HIGH; LED_unsafe_State = LOW; ALARM_start_State = LOW;}
  else if (Water_level == HIGH)
  { LED_safe_State = LOW; LED_unsafe_State = HIGH; ALARM_start_State = HIGH; }
  digitalWrite(LED_safe, LED_safe_State);  digitalWrite(LED_unsafe, LED_unsafe_State);  digitalWrite(ALARM_start, ALARM_start_State);

  //sensors monitering, the TDS and pH with no
  if (currentMillis - Mon_milles >= Mon_INTERVAL )
  {
    pH_fun();
   Serial.print("Voltage:");
   Serial.print(voltage,2);
   Serial.print("    pH value: ");
   Serial.println(pHValue,2);
   Serial.print(" time to start monitering");
    readTdsQuick(); // tempreture and TDS and then pH sensor readings.
    surr_temp();
    //LDR_FUN();
  }
  //check other syntax, add the other two pumps
  if (sec == 60) {
    min += 1;
    sec = 0;
    if (min == 60) {
      hour += 1;
      min = 0;
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
    Serial.print(min);
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
  if ((curMillis_2 - prevMillis_2) >=400 && min==0|| min==8 || min==9 ||min==11||min==21 || min==31 ||min==41 || min==51  )
  {
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print("Water level=");
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
  if (( curMillis_2- prevMillis_2) >= 400 && min==1||  min==12||min==22 || min==10 ||min==18 || min==32 ||min==42 || min==52  )
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
  }
  if (( curMillis_2- prevMillis_2) >= 400 && min==2|| min==13||min==23 || min== 19 || min==20 || min==33 ||min==43 || min==53)
  {
    prevMillis_2 = curMillis_2;
   lcd.setCursor(0, 1);
    lcd.print(F("Temp:"));
   lcd.setCursor(5, 1);
    lcd.print(t);
    lcd.setCursor(10, 1);
    lcd.print((char)223);
    lcd.print("C");
  }
  if (( curMillis_2- prevMillis_2) >= 400 &&min==9||  min==3 && min==14|| min== 28|| min== 29|| min==24||min==34 || min==44 ||min==54)
  {
   //issues fixed but the lcd does not print the pH on virtual lvd 
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print("pH value:");
    lcd.setCursor(9,1);
    lcd.print(pHValue,2);
  }
  if ((curMillis_2 - prevMillis_2) >= 400 &&min==7 || min==4 ||min==59 || min== 30|| min==38||  min==15||min==25 || min==35 ||min==45 || min==55)
  {
  //issues fixed 
   prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("TDS:")); 
    lcd.setCursor(4, 1);
    lcd.print(sensor::tds);
    lcd.setCursor(7, 1);
    lcd.print("     ");
    lcd.setCursor(12, 1);
    lcd.print(" ");
    lcd.setCursor(13, 1);
    lcd.print("ppm");
  }
  if (( curMillis_2- prevMillis_2) >= 400 && min==5|| min==16|| min ==58 ||min==26 || min== 39|| min==40 || min==36 ||min==46 || min==56)
  {
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("EC:")); 
    lcd.setCursor(3, 1);
    lcd.print(sensor::ec,2);
    lcd.setCursor(12, 1);
    lcd.print("uS/m"); 
   
  }
  if (( curMillis_2 - prevMillis_2) >= 400  && min==6|| min==17||min==27 || min==48 || min==49 || min==37 ||min==47 || min==57 ||min==50 )
  {
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print(F("Tempe 2:")); 
    lcd.setCursor(8, 1);
    lcd.print(sensor::waterTemp, 2);
    lcd.setCursor(13, 1);
    lcd.print("    ");
    //lcd.setCursor(15, 1);
    //lcd.print("   ");
  }
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

    if (min < 10) {
      lcd.setCursor(3, 0); lcd.print(0);
      lcd.setCursor(4, 0); lcd.print(min);
    }
    else {
      lcd.setCursor(3, 0); lcd.print(min);
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
  sensor::waterTemp = dallasTemperature.getTempCByIndex(0);
  float rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float temperatureCoefficient = 1.0 + 0.02 * (sensor::waterTemp - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  sensor::ec = (rawEc / temperatureCoefficient) * sensor::ecCalibration; // temperature and calibration compensation
  sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5; //convert voltage value to tds value
  Serial.println(F("TDS:")); Serial.println(sensor::tds);
  Serial.print(F("EC:")); Serial.println(sensor::ec, 2);
  Serial.print(F("Temperature:")); Serial.println(sensor::waterTemp, 2);
}

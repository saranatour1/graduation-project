// New code 
#include <OneWire.h>     
#include <DallasTemperature.h>
#include <DHT.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <LiquidCrystal_I2C.h>
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

int Level_sensor_pin = 13;
int Pump_pin = 8;
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
int day_n=0;
int days = 0;
int day = 7;
int month = 6;
int year = 2022;
int hour =6;
int min = 2;
int sec = 30;

int pumpA=5;
int pumpB=4;
int pHpump=3; 

int Growlight_pin= 2; 

int pumpState = HIGH;  //relays are active low 
int GrowlightState=HIGH;             // GrowlightStateused to set the LED
int pH_state=HIGH;
int TDS_state=HIGH;//for both pumps


static float pHValue, voltage;
int Water_level=1;
int moisture=0;
String values; 

//all millies definition
unsigned long previousMillis_pH=0;
unsigned long previousMillis_tds=0;
unsigned long previousMillis_pump=0;
unsigned long previousMillis_Monitering=0;
unsigned long previousMillis_time=0;
unsigned long previousMil=0;
unsigned long previousMil_1=0;
unsigned long prevMillis_2=0;
unsigned long previousMillis_3=0; 
const long period = 3000;
const long period_1=4000;


float pH_min=5.5; 
float pH_max=6.5;

int TDS_min=512;
int TDS_max=840; 

int time_to_start_Grow_lights=20;
int time_to_turn_off_Grow_lights=6;

const long Interval=20UL*1000UL;
const long Interval_pH=1UL*1000UL;
const long Interval_tds=2UL*1000UL;
unsigned long previousMillis_Growlights = 0;        // will store last time LED was updated


void setup()
{
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.init();
  pinMode(Level_sensor_pin, INPUT);
  pinMode(Pump_pin, OUTPUT);
   pinMode(Growlight_pin, OUTPUT);
  pinMode(pHpump, OUTPUT);
  pinMode(pumpA, OUTPUT);
  pinMode(pumpB, OUTPUT);
  //pinMode(Buzzer_pin,OUTPUT);  
  dallasTemperature.begin();
  dht.begin();
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop(){
  moisture = analogRead(moisture_pin);   
 // Water_level = digitalRead(Level_sensor_pin);
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  //pump repeatance code
  unsigned long currentMillis_pump = millis();  
  if (currentMillis_pump - previousMillis_pump >= 5000) // to repeat every hour for the interval of 1 minitue
  {
    previousMillis_pump = currentMillis_pump; // save the last time the pump worked
    if (pumpState == HIGH && min == 59 && Water_level == 1 )
    { pumpState = LOW; digitalWrite(Pump_pin, pumpState); Serial.println(" time to pump water for 1 minitue");}
    else if (min !=59 || Water_level == 0)
    { pumpState = HIGH; digitalWrite(Pump_pin, pumpState); Serial.println(" time to stop pumping");}
  }
  
  unsigned long currentMillis_growlights = millis();
//  Serial.println(Interval);
  boolean running_time_of_grow_light=false; 
  if(hour<=time_to_turn_off_Grow_lights|| hour>=time_to_start_Grow_lights)
   {  //Serial.println("we are first here");
   running_time_of_grow_light=true;}
   else 
   {
     running_time_of_grow_light=false; 
   }
  // Serial.println(running_time_of_grow_light);
   if(running_time_of_grow_light==true){
   if (currentMillis_growlights - previousMillis_Growlights >= Interval) {
    // save the time you should have toggled the LED
   previousMillis_Growlights += Interval;
   if (GrowlightState==HIGH)
    // if the LED is off turn it on and vice-versa:
      GrowlightState=LOW;
     Serial.println("we are here");
    } 
    }
   else 
    {
    GrowlightState=HIGH ;
    }
   digitalWrite(Growlight_pin, GrowlightState); 
    
   boolean pH_starting=false;    
   if(pHValue >=pH_max)
   {
   pH_starting=true;  
   }
   else if(pHValue <=pH_min)
   {
   pH_starting=false;
   }
   else 
   {
   pH_starting=false;
   }

  unsigned long currentMillis_pH=millis();
  if ( (pH_starting==true&& min%30==0 && ((sec==41||sec==42|| sec==43|| sec==44||sec==45 ||sec==46))))
  {
  if (currentMillis_pH - previousMillis_pH >= Interval_pH) {
    // save the time you should have toggled the LED
  previousMillis_pH += Interval_pH;
  if (pH_state ==HIGH){
    // if the LED is off turn it on and vice-versa:
    pH_state=LOW;
     // Serial.println("we are here");
  }
  else 
  {
  pH_state=HIGH;
  }
  }
  }
else if ((sec!=41||sec!=46))
{
   pH_state=HIGH;
  }

  
    digitalWrite(pHpump,pH_state); 
    
  boolean tds_starting_time=false;
  if ((sensor::tds)<TDS_min)
  {
  tds_starting_time=true;
  }
  else
  {
  tds_starting_time=false;
  }
  unsigned long currentMillis_tds=millis();
  if(tds_starting_time==true && day_n%7==0 && hour%6==0&& min%30==0 &&(sec==18||sec==19|| sec==20))
  {
  if (currentMillis_tds - previousMillis_tds >= Interval_tds) {
    // save the time you should have toggled the LED
  previousMillis_tds += Interval_tds;
  if (TDS_state ==HIGH){
    // if the LED is off turn it on and vice-versa:
  TDS_state=LOW;
     // Serial.println("we are here");
  }
  else
  {
  TDS_state=HIGH; 
  }
  }
  }
  else if ((sec!=19|| sec!=20))
  {
    TDS_state=HIGH;
    }
  digitalWrite(pumpA,TDS_state);
  digitalWrite(pumpB,TDS_state);
  
  unsigned long currentMillis_Monitering=millis();
  if (currentMillis_Monitering - previousMillis_Monitering >= 7000 )
  {
    pH_fun();
   Serial.print("Voltage:");
   Serial.print(voltage,2);
   Serial.print("    pH value: ");
   Serial.println(pHValue,2);
   Serial.print(" time to start monitering");
   readTdsQuick(); // tempreture and TDS and then pH sensor readings.
   surr_temp();
  }
  if (sec == 60) {
    min += 1;
    sec = 0;
    if (min == 60) {
      hour += 1;
      min = 0;
      if (hour == 24) {
        day += 1;
        days += 1;
        day_n+=1;
        if(day_n==7)
        {
          day_n=0; 
          }
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
  unsigned long currentMillis_time=millis();
  if (currentMillis_time - previousMillis_time >= 4938.5 )
  {
  previousMillis_time=  currentMillis_time;
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
unsigned long  currentMil = millis();
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
unsigned long   curMillis_2 = millis();
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
  if (( curMillis_2- prevMillis_2) >= 400 &&min==9||  min==3 || min==14|| min== 28|| min== 29|| min==24||min==34 || min==44 ||min==54)
  {
   //issues fixed but the lcd does not print the pH on virtual lvd 
    prevMillis_2 = curMillis_2;
    lcd.setCursor(0, 1);
    lcd.print("pH value:");
    lcd.setCursor(9,1);
    lcd.print(pHValue,2);
    lcd.setCursor(13, 1);
    lcd.print("    "); 

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
    lcd.setCursor(7, 1);
    lcd.print("     ");
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
  unsigned long currentMillis_3=millis();
 
/*if (currentMillis_3 - previousMillis_3 >= 8000)
  {
   Serial1.flush();
  }*/
  if(currentMillis_3 - previousMillis_3 >= 2500)
  {
  previousMillis_3=currentMillis_3;
  String NEW_TDS=String(sensor::tds); 
  String NEW_Water_temp=String(sensor::waterTemp);
  String New_pH_Value=String(pHValue);
  String New_Temp=String(t);
  String New_HUMID_Value=String(h);
  String NEW_moisture=String(moisture);
  String New_water_level=String(Water_level);
  values= (NEW_TDS+','+ NEW_Water_temp +','+ New_pH_Value +','+New_Temp+','+New_HUMID_Value+','+NEW_moisture+','+New_water_level+',');
  Serial1.print(values);
  Serial.print("i'm here");
  }   
}
 
void showDefaultENG(String dow) 
{
  int hourtemp = 0;
unsigned long  currentMil_1 = millis();
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
      lcd.setCursor(1, 0); lcd.print(hour);
    }
    else {
      lcd.setCursor(0, 0); lcd.print(hour);
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
void readTdsQuick()
 {
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
  

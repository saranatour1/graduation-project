#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long prevMillis = 0;
unsigned long curMillis;

unsigned long previousMil = 0;
unsigned long currentMil =0;
const long period = 3000;

unsigned long previousMil_1 = 0;
unsigned long currentMil_1 =0;
const long period_1 =40000;



bool eng=false;
bool dateENG=false;



String s;
int days=0;
int day=8;
int month=4;
int year=2020;
int hour=23;
int min=59;
int sec=0;




void setup()
{
 Serial.begin(9600);
 lcd.init();
 lcd.backlight();
}

void loop()
{ 
  if(sec==60){
    min+=1;
    sec=0;
    if(min==60){
      hour+=1;
      min=0;
      if(hour==24){
        day+=1;
        days+=1;
        hour=0;
        if(day==31){
          month+=1;
          day=1;
          if(month==13){
            year+=1;
            month=1;
          }  
        }
      }
    }
  }
   if(eng==false){ //sadece ilk durum
     eng=true;
  }
 
 currentMil = millis();
  //3 sn lik period gerçekleşti mi kontrolü
  if (currentMil - previousMil >= period) {
    previousMil = currentMil;
 if(eng==true){
      s=DOW("ENG",days);
      showDefaultENG(s);
  }

  delay(1000); 
  sec+=1;
}
}
void showDefaultENG(String dow){
  
    int hourtemp=0;
    currentMil_1 = millis();
  //3 sn lik period gerçekleşti mi kontrolü
    if (currentMil_1 - previousMil_1>= period_1) {
    previousMil_1 = currentMil_1;
     lcd.setCursor(0,0); lcd.print(0);
     if(day<10){
      lcd.setCursor(10,0);lcd.print(" ");
      lcd.setCursor(0,0); lcd.print(0);
      lcd.setCursor(1,0); lcd.print(day); 
    }
    else {
      lcd.setCursor(10,0);lcd.print(" ");
      lcd.setCursor(0,0); lcd.print(day); 
    }
    lcd.setCursor(2,0); lcd.print(":");
  
    if(month<10){
      lcd.setCursor(10,0);lcd.print(" ");
      lcd.setCursor(3,0); lcd.print(0);
      lcd.setCursor(4,0); lcd.print(month);
    }
    else {
      lcd.setCursor(10,0);lcd.print(" ");
      lcd.setCursor(3,0); lcd.print(month);
    }
    lcd.setCursor(5,0); lcd.print(":");
    lcd.setCursor(6,0); lcd.print(year);
    lcd.setCursor(13,0); lcd.print(dow);

    }
   else {
        if(hour>12) {hourtemp=(hour-12);}
    else if(hour==0){hourtemp=12;} //12.00 AM
  
    if(hourtemp<10){
      lcd.setCursor(0,0); lcd.print(0);
      lcd.setCursor(1,0); lcd.print(hourtemp);
    }
    else {
      lcd.setCursor(0,0); lcd.print(hourtemp);
    }
    lcd.setCursor(2,0); lcd.print(":");
  
    if(min<10){
      lcd.setCursor(3,0); lcd.print(0);
      lcd.setCursor(4,0); lcd.print(min);
    }
    else {
      lcd.setCursor(3,0); lcd.print(min);
    }
    lcd.setCursor(5,0); lcd.print(":");
  
    if(sec<10){
      lcd.setCursor(6,0); lcd.print(0);
      lcd.setCursor(7,0); lcd.print(sec);
    }
    else {
      lcd.setCursor(6,0); lcd.print(sec);
    }
   
    if(hour<12){
        lcd.setCursor(8,0);lcd.print(" ");
       lcd.setCursor(9,0); lcd.print("AM");

    }
    else{
        lcd.setCursor(8,0);lcd.print(" ");
      lcd.setCursor(9,0); lcd.print("PM");
    }
   
    lcd.setCursor(11,0); lcd.print(" ");
    lcd.setCursor(13,0); lcd.print("ENG");
   
}
}

String DOW(String lng,int days){
  String thisDay;
  if(lng=="ENG"){
    if(days%7==0){thisDay="WED";}
    else if(days%7==1){thisDay="THU";}  
    else if(days%7==2){thisDay="FRI";}
    else if(days%7==3){thisDay="SAT";} 
    else if(days%7==4){thisDay="SUN";}  
    else if(days%7==5){thisDay="MON";}
    else if(days%7==6){thisDay="TUE";}
  }
  return thisDay;
}

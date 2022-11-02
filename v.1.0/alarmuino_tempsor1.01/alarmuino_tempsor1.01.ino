/* Project by:
*                      __  __    ___          
*    _______  ______  / /_/ /_  / (_)___  ___ 
*   / ___/ / / / __ \/ __/ __ \/ / / __ \/ _ \
*  (__  ) /_/ / / / / /_/ / / / / / / / /  __/
* /____/\__, /_/ /_/\__/_/ /_/_/_/_/ /_/\___/ 
*      /____/                                 
*      
* The project is a combination of several projects. Here are the original projects and authors:
* 
* 1. LCD Screens and the Arduino Uno, by Aidan
* Link: https://core-electronics.com.au/guides/use-lcd-arduino-uno/
*
* 2. Arduino DS3231 Real Time Clock Module Tutorial, by Dejan Nedelkovski
* Link: https://howtomechatronics.com/tutorials/arduino/arduino-ds3231-real-time-clock-tutorial/
* 
* 2.1 Download the DS3231 libary from http://www.rinkydinkelectronics.com
* 
* 3. How to Make an Arduino Alarm Clock Using a Real-Time Clock and LCD Screen
* Link: https://maker.pro/arduino/projects/arduino-alarm-clock-using-real-time-clock-lcd-screen
* 
* 4. Arduino 2 Led Blink
* Link: https://create.arduino.cc/projecthub/sumeyye-varmis/arduino-2-led-blink-24c93c
* 
* 5. Digital Clock with Alarm Function, by Michalis Vasilakis
* Link: http://www.ardumotive.com/
* 
* 6. Simple Pomodoro 
* Link:
*
*  Home Screen        Temp. & Hum.       Total Numbers/Day  Alarm Display      Alarm Pomodoro    
*  +----------------+ +----------------+ +----------------+ +----------------+ +----------------+
*  |HH:MM:SS|A HH:MM| |  Temp: XX $C   | | Totals:  P: XX | |    Alarm!!     | |  Time to take  |
*  |DD/MM/YY|P XXXX | |  Humid: XX %   | | sB: XX  lB: XX | |    X X X X     | |   a break $    |
*  +----------------+ +----------------+ +----------------+ +----------------+ +----------------+
*
*  Setup              Setup Alarm        Setup Pomodoro     Setup Break        Setup Long Break
*  +----------------+ +----------------+ +----------------+ +----------------+ +----------------+
*  |    >HH :>MM    | |   Set Alarm    | | Pomodoro Time: | |   Break Time:  | |Long Break Time:|
*  |>DD />MM />YYYY | |   >HH :>MM     | |      >MM       | |      >MM       | |      >MM       |
*  +----------------+ +----------------+ +----------------+ +----------------+ +----------------+
*
*/

//Libraries
#include <Wire.h>
#include <LiquidCrystal.h>
#include <DS3231.h>
#include "RTClib.h"
#include <EEPROM.h>
#include "Time.h"
#include "DHT.h"

// Digital pin connected to the DHT sensor
#define DHTPIN 6
#define DHTTYPE DHT11
// The pins the LED is connected to
#define green_led 8
#define red_led 9
// The buzzer
#define buz 10

// Lcd
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int numRows = 2;
const int numCols = 16;
// Clock Module
RTC_DS3231  rtc;
// Temperature & humidity sensor
DHT dht(DHTPIN, DHTTYPE);

char daysOfTheWeek[7][12] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Variables
int DD,MM,YY,H,M,S,set_state, adjust_state, alarm_state,AH,AM;
int sensorDetect=0;
int i =0;
int btnCount = 0;
String sDD;
String sMM;
String sYY;
String sH;
String sM;
String sS;
String aH="12";
String aM="00";
String alarm = "     ";
long previousMillis = 0;    

// Special degree symbol
byte cdegrees[8] = {B00000, B01110, B01010, B01110, B00000, B00000, B00000, B00000}; 
// Special alarm symbol
byte asymb[8] = {B00000, B00100, B01010, B01010, B11111, B00100, B00000, B00000};
// Smiley symbol
byte smiley[8] = {B00000, B10001, B00000, B00000, B10001, B011110, B00000, B00000};
// text for start message 
const char textString1[] = "Alarmuino Tensor";
const char textString2[] = "version 1.0";
const char textString3[] = "Designed by";
const char textString4[] = "synthline";

// Buttons, buzzer, motions sensor
const int btSet = A1;
const int btAdj = A2;
const int btAlarm = A3;
const int buzzer = 10;
const int motionSensor = A0;
long interval = 300;  
int melody[] = { 600, 800, 1000,1200 };

//Boolean flags
boolean setupScreen = false;
boolean alarmON=false;
boolean turnItOn = false;

void setup() {
// Put your setup code here, to run once
  // Setup Serial connection
  Serial.begin(9600);
  Wire.begin();

  
  // Declare the LEDs as an output
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  
  // Set up the LCD's number of columns and rows:
  lcd.begin(numCols, numRows);
  // Create special characters
  lcd.createChar(0, cdegrees);
  lcd.createChar(0, asymb);
  lcd.createChar(0, smiley);
  
  // Initialise the clock module
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(1, 1, 22);   // Set the date to January 1st, 2014
  
  // Initialise the temp & humid sensor
  dht.begin();

  //Initialise the buttons
  pinMode(btSet,INPUT_PULLUP);
  pinMode(btAdj,INPUT_PULLUP);
  pinMode(btAlarm, INPUT_PULLUP);

  // Initialise the buzzer
  pinMode(buz, OUTPUT);

  /*
  // Start screen message:
  lcd.setCursor(0, 0);
  lcd.print(textString1);
  lcd.setCursor(0, 1);
  lcd.print(textString2);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(textString3);
  lcd.setCursor(0, 1);
  lcd.print(textString4);
  delay(3000);
  lcd.clear();
  */

  delay(55);
}


void loop() {
// put your main code here, to run repeatedly:

  // DHT
  // Read Humidity
  float h = dht.readHumidity();
  
  // Read Temperature
  float temp = dht.readTemperature();
  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again)
  if (isnan(h) || isnan(temp) || isnan(f)) {
    lcd.setCursor(0,0);
    lcd.print("Failed to read ");
    lcd.setCursor(0,1);
    lcd.print("from DHT sensor!");
    delay(500);
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temp, h, false);
  
  // RTC
  ti = rtc.getTime();
  Hor = ti.hour;
  Min = ti.min;
  Sec = ti.sec;

  
    int displayt = 2000;
    for (i = 0; i = displayt; i++) {
      lcd.setCursor(0,0);
      lcd.print(rtc.getTimeStr());
      lcd.setCursor(8,0);
      lcd.print("|");
 
      lcd.setCursor(0,1);
      lcd.print(rtc.getDateStr(1));
      lcd.setCursor(8,2);
      lcd.print("|");
      lcd.clear();
      delay(1000)
      }
    
 
 delay (5000);

 // Display the Temperature and Humidity:
 
  lcd.setCursor(0,0);

  lcd.print("Humid. ");

  lcd.print(h);

  lcd.print(" %");

  lcd.setCursor(0,1);

  lcd.print("Temp. ");

  lcd.print(temp);

  lcd.print(strcat(cdegrees,"C"));

  //Here you could also add a Heat Index, both in Celcius and Fahrerheit. Check the original project (#3) on further instructions.

  delay (5000);
 
  //Comparing the current time with the Alarm time 
  if( Hor == 13 && (Min == 36 || Min == 00)) {

  // Buzzer();

  // Buzzer();

  lcd.clear();

  lcd.print("Alarm ON");

  lcd.setCursor(0,1);

  lcd.print("Alarming!!");

  // Buzzer();

  // Buzzer();

  } 

 delay(500); 

}

  // Buzzer() {

  digitalWrite(buz,HIGH);
  
  // Turn the LED on
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);
  delay(500);
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, HIGH);
  delay(500);
  digitalWrite(buz, LOW);


 // Wait half a second before repeating
  delay(500);

}

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
* 1. LCD Screens and the Arduino Uno article by Aidan
* Link: https://core-electronics.com.au/guides/use-lcd-arduino-uno/
*
* 2. Arduino DS3231 Real Time Clock Module Tutorial, created by Dejan Nedelkovski --> www.HowToMechatronics.com
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
DS3231  rtc(SDA, SCL);
// Temperature & humidity sensor
DHT dht(DHTPIN, DHTTYPE);

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


void setup() {
// put your setup code here, to run once
  // Setup Serial connection
  Serial.begin(9600);
  Wire.begin();
  // set up the LCD's number of columns and rows:
  lcd.begin(numCols, numRows);
  // Initialise the clock module
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(1, 1, 22);   // Set the date to January 1st, 2014
  // Initialise the temp & humid sensor
  dht.begin();

  // Create special characters
  lcd.createChar(0, cdegrees);
  lcd.createChar(0, asymb);
  lcd.createChar(0, smiley);

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
}


void loop() {
// put your main code here, to run repeatedly:

  //      Values Input
  //  1. DHT
  // Read Humidity
  float h = dht.readHumidity();
  
  // Read Temperature
  float temp = dht.readTemperature();
  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  char hch = (char)h;
  char tempch = (char)temp;

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
  // float hif = dht.computeHeatIndex(f, h);

  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(temp, h, false);

  //    Display Screens
 // Display 1---> Home Screen 
 lcd.setCursor(0,0);
 lcd.print(rtc.getTimeStr());
 lcd.setCursor(8,0);
 lcd.print("|");
 
 lcd.setCursor(0,1);
 lcd.print(rtc.getDateStr(1));
 lcd.setCursor(8,2);
 lcd.print("|");
 delay(3000);
 lcd.clear();
 // Display 2 ---> Temp & humid
 lcd.setCursor(0,0);
 lcd.print(strcat(strcat("Humid. ", hch), "%"));
 lcd.setCursor(0,1);
 lcd.print(strcat(strcat(strcat("Temp. ", tempch), cdegrees), "C"));
 delay(3000);
 lcd.clear();

  
 //delay(1000); 
}

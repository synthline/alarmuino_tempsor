#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //LCD
const int numRows = 2;
const int numCols = 16;

const char textString[] = "Alarmuino Tensor by synthline";
const int textLen = sizeof(textString) -1; // the number of characters

int dtime = textLen; 


void setup() {
  // put your setup code here, to run once:
  
  // set up the LCD's number of columns and rows:
lcd.begin(numCols, numRows);
// some time function that changes the dtime
lcd.print(textString);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(dtime != 0)
  {
    for (int position = 0; position < textLen; position++)
    {
    lcd.scrollDisplayLeft();
    delay(150);
    dtime--;
    } 
  }
}

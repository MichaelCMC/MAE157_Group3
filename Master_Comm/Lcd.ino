// include the library code:
#include <LiquidCrystal.h>

int i(0);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(0, 1, 2, 3, 4, 5);


void setup(){
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("157 Sucks");
} //end "setup()"

void loop(){
lcd.clear();
lcd.print(i++);
delay(1000);
}

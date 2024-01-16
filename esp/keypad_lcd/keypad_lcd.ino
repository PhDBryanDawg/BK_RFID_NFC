#include <Keypad.h>
#include <LiquidCrystal_I2C.h>


#define ROW_NUM     4 // four rows
#define COLUMN_NUM  4 // four columns

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM]      = {13, 12, 14, 27}; // GPIO19, GPIO18, GPIO5, GPIO17 connect to the row pins
byte pin_column[COLUMN_NUM] = {26, 25, 33, 32};   // GPIO16, GPIO4, GPIO0, GPIO2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password = "7890"; 
String input_password;


void setup() {
  Serial.begin(9600);
  input_password.reserve(5);


  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();              // clear display
   lcd.setCursor(0, 0);      // move cursor to   (0, 0)
        lcd.print("NEIN!");       // print message at (0, 0)
        lcd.setCursor(0, 1);      // move cursor to   (2, 1)
        lcd.print("Raus hier !");
   }

void loop() {
  char key = keypad.getKey();

  if (key) {
    //Serial.println(key);
  
    if (key == '*') {
      input_password = ""; // clear input password
    } else if (key == '#') {
      if (password == input_password) {
        Serial.println("The password is correct, ACCESS GRANTED!");
        lcd.clear();              // clear display
        lcd.setCursor(0, 0);      // move cursor to   (0, 0)
        lcd.print("The password is correct");       // print message at (0, 0)
        lcd.setCursor(2, 1);      // move cursor to   (2, 1)
        lcd.print("ACCESS GRANTED!"); // print message at (2, 1)
        delay(200);

      } else {
        Serial.println("The password is incorrect, ACCESS DENIED!");
        lcd.clear();              // clear display
        lcd.setCursor(0, 0);      // move cursor to   (0, 0)
        lcd.print("The password is incorrect");       // print message at (0, 0)
        lcd.setCursor(2, 1);      // move cursor to   (2, 1)
        lcd.print("ACCESS DENIED!"); // print message at (2, 1)
        delay(200);
      }

      input_password = ""; // clear input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
}
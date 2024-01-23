/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-mysql
 */
//einbinden der Bibliotheken für das
//ansteuern des MFRC522 Moduls
#include <SPI.h>
#include <MFRC522.h>

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

byte pin_rows[ROW_NUM]      = {13, 12, 14, 27}; 
byte pin_column[COLUMN_NUM] = {26, 25, 33, 32};   

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

#include <WiFi.h>
#include <HTTPClient.h>

//definieren der Pins  RST & SDA für den ESP32
#define RST_PIN     0
#define SS_PIN      4

//erzeugen einer Objektinstanz
MFRC522 mfrc522(SS_PIN, RST_PIN);

const char WIFI_SSID[] = "wahofr";
const char WIFI_PASSWORD[] = "wahofr24";

String HOST_NAME = "http://l3onh.xyz"; // change to your PC's IP address
String PATH_NAME   = "/rfidscan";
String query = "?id="                 // 
String queryString = "?id=100000";    // test query

const String password = "7890"; //definie PIN
String input_password;

void setup() {
  Serial.begin(115200); 

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  input_password.reserve(5);
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();              // clear display
  lcd.setCursor(0, 0);      // move cursor to   (0, 0)
  lcd.print("Bitte Karte");       // print message at (0, 0)
  lcd.setCursor(0, 1);      // move cursor to   (2, 1)
  lcd.print("System aktiv");

  delay(50);
  //begin der SPI Kommunikation
  SPI.begin();
  //initialisieren der Kommunikation mit dem RFID Modul
  mfrc522.PCD_Init();
  

  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME + queryString); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void loop() {


  if (mfrc522.PICC_ReadCardSerial()) {
   //Lesen, DB abfragen, schalten.
    String newRfidId = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
    newRfidId.concat(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    newRfidId.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  //alle Buchstaben in Großbuchstaben umwandeln
  newRfidId.toUpperCase();

  Serial.print(" gelesene RFID-ID :");
  Serial.println(newRfidId);
  Serial.println();


  // DB Abfrage
  String queryString = query+newRfidId;

  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME + queryString); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  if(){  //if Zugang gewährt
    door_closed=true;
    while(door_closed){
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
        door_closed=false;
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
    }
  }

//anfrage an server is da, antwort?
  }

  














}

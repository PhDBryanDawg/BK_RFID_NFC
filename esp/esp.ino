#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>


// Keypad definitions
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


// Network defitions
const char SSID[] = "FritzFrost";
const char Password[] = "wlannetzvonsektor7g";
const String server = "http://theyseeme.win";

HTTPClient http;
String MAC;

TaskHandle_t Heartbeat;
const int heartbeatInterval = 10000;


// Define Reader init
#define RST_PIN     0
#define SS_PIN      4

MFRC522 mfrc522(SS_PIN, RST_PIN);


String ConnectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, Password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[0]);

  return ipStr;
}

void setup() {

  Serial.begin(115200);

  MAC = WiFi.macAddress();
  String ip = ConnectWiFi();

  if (ip != NULL) {
    Serial.println("");
    Serial.println(MAC);
    Serial.printf("Connected to WiFi network with IP Address: %s\n", ip);
  }

  // Send inital heartbeat
  SendHeartbeat(&http, &MAC, true);

  xTaskCreatePinnedToCore(
    loop2,       /* Function to implement the task */
    "Heartbeat", /* Name of the task */
    10000,       /* Stack size in words */
    NULL,        /* Task input parameter */
    0,           /* Priority of the task */
    &Heartbeat,  /* Task handle. */
    0);          /* Core where the task should run */

  // init lcd
  lcd.init(); // initialize the lcd
  lcd.backlight();
  int pos[] = {0,0};
  LCDMessage("Test", pos, true);


  // init RFID
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String RFID= "";
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    RFID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    RFID.concat(String(mfrc522.uid.uidByte[i], HEX));
    if (i < mfrc522.uid.size-1)
    {
      RFID.concat(String(":"));
    }
  }

  RFID.toUpperCase();
  Serial.println(RFID);

  bool accepted = VerifyKey(&http, &RFID, &MAC);

  if (accepted)
  {
    Serial.println("ACCEPTED");
  }
  else
  {
    Serial.println("DECLINED");
  }

  delay(5000);

}

void loop2(void *parameter) {
  for (;;) {
    SendHeartbeat(&http, &MAC, false);
    delay(heartbeatInterval);
  }
}


void SendHeartbeat(HTTPClient *http, const String *MAC, bool output) {
  const String uri = "/esp32/heartbeat.php";

  http->begin(server + uri);
  http->addHeader("Content-Type", "application/x-www-form-urlencoded");
  String params = "MAC='" + String(*MAC) + "'";

  int httpCode = http->POST(params);


  if (output == true) {
    if (httpCode > 0 && output == true) {
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http->getString();
        Serial.println(payload);
      } else {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http->errorToString(httpCode).c_str());
    }
  }

  http->end();
}


bool VerifyKey(HTTPClient *http, String *key, const String *MAC) {
  const String uri = "/esp32/verify.php";

  http->begin(server + uri);
  http->addHeader("Content-Type", "application/x-www-form-urlencoded");
  String params = "MAC='" + String(*MAC) + "'&RFID=" + String(*key);

  int httpCode = http->POST(params);


  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String result = http->getString();
      
      char inputCopy[200];
      strcpy(inputCopy, result.c_str());

      char tokens[3][50];

      char *token = strtok(inputCopy, "***");
      token = strtok(NULL, "***");

      if (strcmp(token, "1") == 0 ){
        return true;
      } else {
        return false;
      }

    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      return false;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http->errorToString(httpCode).c_str());
  }

  http->end();

  return false;
}


void LCDMessage(String message, int* cur, bool clearOnRun)
{
  if (clearOnRun) {
    lcd.clear();
  }
  lcd.setCursor(cur[0], cur[1]);
  lcd.print(message);
}

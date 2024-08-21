#include <Keypad.h>
#include "SPI.h"
#include "MFRC522.h"
#include <Wire.h>
#include <avr/wdt.h>

#define SS_PIN 10
#define RST_PIN 9
#define SP_PIN 8
#define RECEIVER_ADDRESS 8 // I2C address of the receiver

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const char number_of_rows = 4;
const char number_of_columns = 4;
char row_pins[number_of_rows] = {2, 3, 4, 5};
char column_pins[number_of_columns] = {6, 7, 8, 2};

char key_array[number_of_rows][number_of_columns] = {  
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad k = Keypad(makeKeymap(key_array), row_pins, column_pins, number_of_rows, number_of_columns);

char enteredNumber[6] = ""; // To store the 5-digit number
char dataToSend[64]; // Buffer to hold the data to send

int Reset = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize I2C communication
  SPI.begin();
  rfid.PCD_Init();
}

void loop() {
  // Check for keypad input
  Reset++;
  if(Reset == 20){
    //Serial.println("Reset");
    //wdt_enable(WDTO_15MS);
    //while (1) {}
  }
  char keychar = k.getKey();
  //char keychar = 'A';
  if (keychar && isDigit(keychar)) {
    strncat(enteredNumber, &keychar, 1); // Add digit to enteredNumber

    if (strlen(enteredNumber) == 5) { // Once 5 digits are entered
      snprintf(dataToSend, sizeof(dataToSend), "Key:%s; ", enteredNumber);

      Wire.beginTransmission(RECEIVER_ADDRESS);
      Wire.write(dataToSend);
      Wire.endTransmission();

      Serial.println(dataToSend); // For debugging

      memset(enteredNumber, 0, sizeof(enteredNumber)); // Reset the entered number
    }
  }
  /*
  if (keychar == 'A') {
      // Send string "A" if key "A" is pressed
      snprintf(dataToSend, sizeof(dataToSend), "Key:A;");

      Wire.beginTransmission(RECEIVER_ADDRESS);
      Wire.write(dataToSend);
      Wire.endTransmission();

      Serial.println(dataToSend); // For debugging
  } 
*/
  // Check for RFID card presence and read it
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

    if (piccType == MFRC522::PICC_TYPE_MIFARE_MINI ||
        piccType == MFRC522::PICC_TYPE_MIFARE_1K ||
        piccType == MFRC522::PICC_TYPE_MIFARE_4K) {

      char strID[32] = "";
      for (byte i = 0; i < 4; i++) {
        sprintf(strID + strlen(strID), "%02X:", rfid.uid.uidByte[i]);
      }
      strID[strlen(strID) - 1] = '\0'; // Remove the last colon

      snprintf(dataToSend, sizeof(dataToSend), "RFID:%s; ", strID);

      Wire.beginTransmission(RECEIVER_ADDRESS);
      Wire.write(dataToSend);
      Wire.endTransmission();

      Serial.println(dataToSend); // For debugging

    } else {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  delay(300);
}

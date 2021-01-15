// RFID reader + LCD + RELAY (+ LED) +

// TODO: EEPROM (saving cards IDs) - detection of cards and control of access

// RELAY has reversed states > LOW is ON (meaning opened) and HIGH is OFF (meaning closed)

// To DELETE_EEPROM you mast set this bool to true - ONLY for 1 run!!!
bool I_WANT_TO_DELETE_EEPROM = false;

// libraries for RFID reader
#include <SPI.h>
#include <MFRC522.h>

// libraries for LCD (I2C)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// library for EEPROM (storing cards)
#include <EEPROM.h>

// map pins on board (UNO R3) for SDA a RST pins on reader (for SPI communication)
#define RST_PIN 9
#define SDA_PIN 10

// map pin on board to RELAY
#define RELAY 5

// time limits after reading a card
#define STAY_OPEN 5000
#define STAY_LOCKED 2000

// instantiate RFID reader (from lib.)
MFRC522 rfid(SDA_PIN, RST_PIN);
// instantiate LCD (I2C address is 0x27)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// variables for new functions...
bool masterMode = false;
bool replaceMaster = false;
byte masterCardID;  // choose max lenght
byte cardID;

byte successfulRead; // from Reader
void setup() {
  // serial monitor, communication speed: 9600 baud
  Serial.begin(9600);

  // SPI communication
  SPI.begin();
  // RFID reader initialize communication
  rfid.PCD_Init();

  // setup LCD
  lcd.begin();
  lcd.backlight();
  lcd.noBlink();
  setScreanHello();

  // setup RELAY
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  // Only iF the boolean I_WANT_TO_DELETE_EEPROM is set to true {at the beginning of the code)!!!
  DELETE_EEPROM();
}

void loop() {

  // search for RFID tags - if not return (= again)
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // check data received from tag (if correct)
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  // Print info - tag version
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // char name[] = rfid.PICC_GetTypeName(piccType);
  Serial.print("Type: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  //  //Show info - tag version
  //  lcd.clear();
  //  lcd.setCursor ( 0, 0 );
  //  lcd.print("Type:");
  //  lcd.setCursor ( 0, 1 );
  //  lcd.print(rfid.PICC_GetTypeName(piccType));

  // Check supported Types
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("This RFID card is not (fully) supported!\n(Type of MIFARE Classic?)");

    // Potential problems ahead if not supported and return disabled! (Must try if you want to read ID of unknown (type) card.)
    //   return;
  }

  // Print tag ID (hexadecimal)
  Serial.print("ID:  ");
  printIDHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  // My (known) cards
  if (rfid.uid.uidByte[0] == 0x79 & rfid.uid.uidByte[1] == 0xCA & rfid.uid.uidByte[2] == 0xD1 & rfid.uid.uidByte[3] == 0x98) {
    Serial.println("Found card #1");
    handlingOfCardsAndDoors(true);
  }
  else if (rfid.uid.uidByte[0] == 0x76 & rfid.uid.uidByte[1] == 0xCF & rfid.uid.uidByte[2] == 0x60 & rfid.uid.uidByte[3] == 0x29) {
    Serial.println("Found card #2");
    handlingOfCardsAndDoors(true);
  }
  else if (rfid.uid.uidByte[0] == 0x86 & rfid.uid.uidByte[1] == 0x8E & rfid.uid.uidByte[2] == 0x53 & rfid.uid.uidByte[3] == 0x29) {
    Serial.println("Found card #3");
    handlingOfCardsAndDoors(true);
  }
  else if (rfid.uid.uidByte[0] == 0xF4 & rfid.uid.uidByte[1] == 0x1A & rfid.uid.uidByte[2] == 0xFE & rfid.uid.uidByte[3] == 0x28) {
    Serial.println("Found card #4");
    handlingOfCardsAndDoors(true);
  }
  else if (rfid.uid.uidByte[0] == 0xB9 & rfid.uid.uidByte[1] == 0xE3 & rfid.uid.uidByte[2] == 0xBE & rfid.uid.uidByte[3] == 0x98) {
    Serial.println("Found card #5");
    handlingOfCardsAndDoors(true);
  }
  //  else if(rfid.uid.uidByte[0] == 0xF9 & rfid.uid.uidByte[1] == 0x23 & rfid.uid.uidByte[2] == 0x79 & rfid.uid.uidByte[3] == 0x98) {
  //    Serial.println("Found card #6");
  //    handlingOfCardsAndDoors(true);
  //  }
  else if (rfid.uid.uidByte[0] == 0x04 & rfid.uid.uidByte[1] == 0x11 & rfid.uid.uidByte[2] == 0x6F & rfid.uid.uidByte[3] == 0xDA & rfid.uid.uidByte[4] == 0x8E & rfid.uid.uidByte[5] == 0x55 & rfid.uid.uidByte[6] == 0x80) {
    Serial.println("Found card #7");
    handlingOfCardsAndDoors(true);
  }
  //  else if (rfid.uid.uidByte[0] == 0x04 & rfid.uid.uidByte[1] == 0x53 & rfid.uid.uidByte[2] == 0x75 & rfid.uid.uidByte[3] == 0x7A & rfid.uid.uidByte[4] == 0x89 & rfid.uid.uidByte[5] == 0x48 & rfid.uid.uidByte[6] == 0x80) {
  //    Serial.println("Found card #8");
  //    handlingOfCardsAndDoors(true);
  //  }
  else {
    handlingOfCardsAndDoors(false);
  }

  // end of communication
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void handlingOfCardsAndDoors(bool known) {
  if (known == true) {
    Serial.println("Found KNOWN card");
    Serial.println();
    setScreanEnter();
    digitalWrite(RELAY, LOW);
    delay(STAY_OPEN);
    digitalWrite(RELAY, HIGH);
  } else {
    Serial.println("Found UNKNOWN card!");
    Serial.println();
    setScreanNoEnter();
    delay(STAY_LOCKED);
  }
  setScreanHello();
}

// print of ID in hexadecimal format
void printIDHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void DELETE_EEPROM() {
  if (I_WANT_TO_DELETE_EEPROM == true) {
    for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) {
      if (EEPROM.read(x) != 0) {
        // writes 0 to clear memory
        EEPROM.write(x, 0);
      }
      Serial.println("EEPROM was deleted!");
      setScreanDeletedMemory();
    }
  }
}

void setScreanHello() {
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("-------------------");
  lcd.setCursor ( 7, 1 );
  lcd.print("HELLO");
  lcd.setCursor ( 0, 2 );
  lcd.print("Waiting for a card.");
  lcd.setCursor ( 0, 3 );
  lcd.print("-------------------");
}

void setScreanEnter() {
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("-------------------");
  lcd.setCursor ( 6, 1 );
  lcd.print("WELCOME");
  lcd.setCursor ( 3, 2 );
  lcd.print("You may enter.");
  lcd.setCursor ( 0, 3 );
  lcd.print("-------------------");
}

void setScreanNoEnter() {
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("-------------------");
  lcd.setCursor ( 3, 1 );
  lcd.print("UNKNOWN card.");
  lcd.setCursor ( 3, 2 );
  lcd.print("Entry denied!");
  lcd.setCursor ( 0, 3 );
  lcd.print("-------------------");
}

void setScreanDeletedMemory() {
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("EEPROM was deleted!");
  lcd.setCursor ( 0, 2 );
  lcd.print("Disable this");
  lcd.setCursor ( 0, 3 );
  lcd.print("function !!!");
}

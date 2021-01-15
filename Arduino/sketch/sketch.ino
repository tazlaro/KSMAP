// RFID reader - detection of my cards

// libraries
#include <SPI.h>
#include <MFRC522.h>

// map pins on board (UNO R3) for SDA a RST pins on reader (for SPI communication)
#define SDA_PIN 10
#define RST_PIN 9

// instantiate (from lib.)
MFRC522 rfid(SDA_PIN, RST_PIN);

void setup() {
  // serial communication - speed: 9600 baud
  Serial.begin(9600);
  // SPI communication
  SPI.begin();
  // RFID reader initialize communication
  rfid.PCD_Init();
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
  Serial.print("Type: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

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
  }
  else if (rfid.uid.uidByte[0] == 0x76 & rfid.uid.uidByte[1] == 0xCF & rfid.uid.uidByte[2] == 0x60 & rfid.uid.uidByte[3] == 0x29) {
    Serial.println("Found card #2");
  }
  else if (rfid.uid.uidByte[0] == 0x86 & rfid.uid.uidByte[1] == 0x8E & rfid.uid.uidByte[2] == 0x53 & rfid.uid.uidByte[3] == 0x29) {
    Serial.println("Found card #3");
  }
  else if (rfid.uid.uidByte[0] == 0xF4 & rfid.uid.uidByte[1] == 0x1A & rfid.uid.uidByte[2] == 0xFE & rfid.uid.uidByte[3] == 0x28) {
    Serial.println("Found card #4");
  }
  else if (rfid.uid.uidByte[0] == 0xB9 & rfid.uid.uidByte[1] == 0xE3 & rfid.uid.uidByte[2] == 0xBE & rfid.uid.uidByte[3] == 0x98) {
    Serial.println("Found card #5");
  }
  //  else if(rfid.uid.uidByte[0] == 0xF9 & rfid.uid.uidByte[1] == 0x23 & rfid.uid.uidByte[2] == 0x79 & rfid.uid.uidByte[3] == 0x98) {
  //    Serial.println("Found card #6");
  //  }
  else {
    Serial.println("Found UNKNOWN card!");
  }

  Serial.println();

  // end of communication
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// print of ID in hexadecimal format
void printIDHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

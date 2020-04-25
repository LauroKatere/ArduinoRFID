#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
#define LED 4

MFRC522 mfrc522(SS_PIN, RST_PIN); // Erzeugen des MFRC522 Objekts

MFRC522::MIFARE_Key key;

byte dataBlock[]    = {
        0x70, 0x68, 0x69, 0x6c, //  1,  2,   3,  4,
        0x69, 0x70, 0x70, 0xff, //  5,  6,   7,  8,
        0x00, 0x00, 0x00, 0x00, //  9, 10, 255, 11,
        0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
    };


byte sector         = 13;
byte blockAddr      = 54;
byte trailerBlock   = 55;
MFRC522::StatusCode status;
byte buffer[18];
byte size = sizeof(buffer);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
 Serial.begin(9600);
 SPI.begin(); //SPI-Verbindung aufbauen
 mfrc522.PCD_Init(); // Initialisierung des MFRC522 Objekts
 delay(5);
 for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }  
   }



void loop() {
  // put your main code here, to run repeatedly:
  if ( ! mfrc522.PICC_IsNewCardPresent()) { // wartet auf neu Karte
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) { // wartet auf RFID-Sender
    return;
  }
  byte rfiduid[4]={0, 0, 0 ,0};     //bytearray in dem die UID Gespeichert wird
  byte rfiduidcheck[4]={0x49, 0x3B, 0x08 ,0xA4};

  // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
  
   // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }
  
  for(byte i = 0; i < mfrc522.uid.size; i++){ // abtasten der einzelnen Bytes der UID
    rfiduid[i]= mfrc522.uid.uidByte[i]; // Speichern des Bytes im Array
    }


   if(rfiduid[0]==rfiduidcheck[0]){
    if(rfiduid[1]==rfiduidcheck[1]){
    if(rfiduid[2]==rfiduidcheck[2]){
    if(rfiduid[3]==rfiduidcheck[3]){

    



    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Lesen der Daten vom Block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    if(buffer[9]==0xff){
      dataBlock[8]= buffer[8]+0x01;
      dataBlock[9]= 0x00;
      }
    else{
      dataBlock[9]= buffer[9]+0x01;
    }

    if(buffer[10]==0xff){
    dataBlock[10]= 0xaa;
    }
    else{
      dataBlock[10]= 0xff;
      }

    if(buffer[10]==0xff){
      digitalWrite(LED, HIGH);
      }
      else{
        digitalWrite(LED, LOW);
        }
     



    

    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();

    // Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    Serial.println(F("Checking result..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Success :-)"));
    } else {
        Serial.println(F("Failure, no match :-("));
        Serial.println(F("  perhaps the write didn't work properly..."));
    }
    Serial.println();

    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}


    
    }
    }
    }
    }


void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }}

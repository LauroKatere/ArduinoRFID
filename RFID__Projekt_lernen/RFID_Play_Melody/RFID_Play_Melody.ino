#include <SPI.h>
#include <MFRC522.h>
#include <pitches.h>

#define RST_PIN 9
#define SS_PIN 10
#define Buzzer 2 


MFRC522 mfrc522(SS_PIN, RST_PIN); // Erzeugen des MFRC522 Objekts

MFRC522::MIFARE_Key key;

byte dataBlock[]    = {
        0xb3, 0xb3, 0x00, 0xb3, // b1 NOTE_C5, b2 NOTE_D5,  b3 NOTE_E5, b4 NOTE_F5,
        0x00, 0xb8, 0xb3, 0x00, // b5 NOTE_G5, b6 NOTE_A5,  b7 NOTE_B5, b8 NOTE_C6,
        0xb5, 0x00, 0x00, 0x00, //  9, 10, 255, 11,
        0xb5, 0x00, 0x00, 0x00  // 12, 13, 14, 15
    };

int melody[8];


byte sector         = 13;
byte blockAddr      = 54;
byte blockAddr2     =53;
byte trailerBlock   = 55;
MFRC522::StatusCode status;
byte buffer[18];
byte size = sizeof(buffer);
int duration = 500;



void setup() {
  // put your setup code here, to run once:
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
    

    


   /* // Write melody to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr2);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr2, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }*/

     // Lesen der Daten vom Block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr2);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr2, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }



    for(int x; x<16; x++){
      if(buffer[x]==0xb1){
        melody[x]=NOTE_C5;
        }
        if(buffer[x]==0xb2){
        melody[x]=NOTE_D5;
        }
        if(buffer[x]==0xb3){
        melody[x]=NOTE_E5;
        }
        if(buffer[x]==0xb4){
        melody[x]=NOTE_F5;
        }
        if(buffer[x]==0xb5){
        melody[x]=NOTE_G5;
        }
        if(buffer[x]==0xb6){
        melody[x]=NOTE_A5;
        }
        if(buffer[x]==0xb7){
        melody[x]=NOTE_B5;
        }
        if(buffer[x]==0xb7){
        melody[x]=NOTE_C6;
        }
        if(buffer[x]==0x00){
          melody[x]==0;
          }
      }


    for(int thisNote = 0; thisNote < 16; thisNote++){
      if(!melody[thisNote]==0){
      tone(Buzzer, melody[thisNote], duration);
      }
      delay(1000);
      }


    

    

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

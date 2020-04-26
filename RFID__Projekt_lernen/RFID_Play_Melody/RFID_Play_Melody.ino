#include <SPI.h>
#include <MFRC522.h>
#include <pitches.h>

#define RST_PIN 9
#define SS_PIN 10
#define Buzzer 2 


MFRC522 mfrc522(SS_PIN, RST_PIN); // Erzeugen des MFRC522 Objekts

MFRC522::MIFARE_Key key;

byte dataBlock[]    = {
        0x00, 0x00, 0x00, 0x00, // b1 NOTE_C5, b2 NOTE_D5,  b3 NOTE_E5, b4 NOTE_F5,
        0x00, 0xb8, 0xbff, 0x07, // b5 NOTE_G5, b6 NOTE_A5,  b7 NOTE_B5, b8 NOTE_C6,
        0x80, 0x69, 0xff, 0xff, //  9, 10, 255, 11,
        0xff, 0xff, 0xff, 0xff  // 12, 13, 14, 15 testing if branch works
    };

int melody[]={
  NOTE_C5, NOTE_D5,NOTE_E5, NOTE_F5,
  NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6,
  NOTE_C5, NOTE_D5,NOTE_E5, NOTE_F5,
  NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6
  };


byte sector         = 13;
byte blockAddr      = 53;
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
    
    
    


   /*// Write melody to the block
   melody_to_byte_array(melody);
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }*/

     // Lesen der Daten vom Block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }


    byte_array_to_melody(buffer);
    


    for(int thisNote = 0; thisNote < 16; thisNote++){
      tone(Buzzer, melody[thisNote], duration);
      delay(1000);
      }


    
    write_to_card(12,15,2);
    

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}


    
    }
    }
    }
    }


void write_to_card(byte blockaddress, byte trailer, int sectorcount){
  for(int count=0; count<sectorcount; count++){

    for(int x= 0; x<3; x++){
      
      // Authenticate using key B
    Serial.println(F("Authenticating using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailer, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    
    
   // Write melody to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockaddress);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockaddress, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
      blockaddress= blockaddress +1;
      
      }
      blockaddress= blockaddress+1;
      trailer= trailer+4;
    
    
    }
  Serial.print("Write successfull");
  }


void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void melody_to_byte_array(int *melody){
  for(int x; x<16; x++){
if(melody[x]==NOTE_B0){dataBlock[x]=0x01;}
if(melody[x]==NOTE_C1){dataBlock[x]=0x02;}
if(melody[x]==NOTE_CS1){dataBlock[x]=0x03;}
if(melody[x]==NOTE_D1){dataBlock[x]=0x04;}
if(melody[x]==NOTE_DS1){dataBlock[x]=0x05;}
if(melody[x]==NOTE_E1){dataBlock[x]=0x06;}
if(melody[x]==NOTE_F1){dataBlock[x]=0x07;}
if(melody[x]==NOTE_FS1){dataBlock[x]=0x08;}
if(melody[x]==NOTE_G1){dataBlock[x]=0x09;}
if(melody[x]==NOTE_GS1){dataBlock[x]=0x0A;}
if(melody[x]==NOTE_A1 ){dataBlock[x]=0x0B;}
if(melody[x]==NOTE_AS1 ){dataBlock[x]=0x0C;}
if(melody[x]==NOTE_B1  ){dataBlock[x]=0x0D;}
if(melody[x]==NOTE_C2  ){dataBlock[x]=0x0E;}
if(melody[x]==NOTE_CS2 ){dataBlock[x]=0x0F;}
if(melody[x]==NOTE_D2  ){dataBlock[x]=0x10;}
if(melody[x]==NOTE_DS2 ){dataBlock[x]=0x11;}
if(melody[x]==NOTE_E2  ){dataBlock[x]=0x12;}
if(melody[x]==NOTE_F2  ){dataBlock[x]=0x13;}
if(melody[x]==NOTE_FS2 ){dataBlock[x]=0x14;}
if(melody[x]==NOTE_G2  ){dataBlock[x]=0x15;}
if(melody[x]==NOTE_GS2 ){dataBlock[x]=0x16;}
if(melody[x]==NOTE_A2  ){dataBlock[x]=0x17;}
if(melody[x]==NOTE_AS2 ){dataBlock[x]=0x18;}
if(melody[x]==NOTE_B2  ){dataBlock[x]=0x19;}
if(melody[x]==NOTE_C3  ){dataBlock[x]=0x1A;}
if(melody[x]==NOTE_CS3 ){dataBlock[x]=0x1B;}
if(melody[x]==NOTE_D3  ){dataBlock[x]=0x1C;}
if(melody[x]==NOTE_DS3 ){dataBlock[x]=0x1D;}
if(melody[x]==NOTE_E3  ){dataBlock[x]=0x1E;}
if(melody[x]==NOTE_F3  ){dataBlock[x]=0x1F;}
if(melody[x]==NOTE_FS3 ){dataBlock[x]=0x20;}
if(melody[x]==NOTE_G3  ){dataBlock[x]=0x21;}
if(melody[x]==NOTE_GS3 ){dataBlock[x]=0x22;}
if(melody[x]==NOTE_A3  ){dataBlock[x]=0x23;}
if(melody[x]==NOTE_AS3 ){dataBlock[x]=0x24;}
if(melody[x]==NOTE_B3  ){dataBlock[x]=0x25;}
if(melody[x]==NOTE_C4  ){dataBlock[x]=0x26;}
if(melody[x]==NOTE_CS4 ){dataBlock[x]=0x27;}
if(melody[x]==NOTE_D4  ){dataBlock[x]=0x28;}
if(melody[x]==NOTE_DS4 ){dataBlock[x]=0x29;}
if(melody[x]==NOTE_E4  ){dataBlock[x]=0x2A;}
if(melody[x]==NOTE_F4  ){dataBlock[x]=0x2B;}
if(melody[x]==NOTE_FS4 ){dataBlock[x]=0x2C;}
if(melody[x]==NOTE_G4  ){dataBlock[x]=0x2D;}
if(melody[x]==NOTE_GS4 ){dataBlock[x]=0x2E;}
if(melody[x]==NOTE_A4  ){dataBlock[x]=0x2F;}
if(melody[x]==NOTE_AS4 ){dataBlock[x]=0x30;}
if(melody[x]==NOTE_B4  ){dataBlock[x]=0x31;}
if(melody[x]==NOTE_C5  ){dataBlock[x]=0x32;}
if(melody[x]==NOTE_CS5 ){dataBlock[x]=0x33;}
if(melody[x]==NOTE_D5  ){dataBlock[x]=0x34;}
if(melody[x]==NOTE_DS5 ){dataBlock[x]=0x35;}
if(melody[x]==NOTE_E5  ){dataBlock[x]=0x36;}
if(melody[x]==NOTE_F5  ){dataBlock[x]=0x37;}
if(melody[x]==NOTE_FS5 ){dataBlock[x]=0x38;}
if(melody[x]==NOTE_G5  ){dataBlock[x]=0x39;}
if(melody[x]==NOTE_GS5 ){dataBlock[x]=0x3A;}
if(melody[x]==NOTE_A5  ){dataBlock[x]=0x3B;}
if(melody[x]==NOTE_AS5 ){dataBlock[x]=0x3C;}
if(melody[x]==NOTE_B5  ){dataBlock[x]=0x3D;}
if(melody[x]==NOTE_C6  ){dataBlock[x]=0x3E;}
if(melody[x]==NOTE_CS6 ){dataBlock[x]=0x3F;}
if(melody[x]==NOTE_D6  ){dataBlock[x]=0x40;}
if(melody[x]==NOTE_DS6 ){dataBlock[x]=0x41;}
if(melody[x]==NOTE_E6  ){dataBlock[x]=0x42;}
if(melody[x]==NOTE_F6  ){dataBlock[x]=0x43;}
if(melody[x]==NOTE_FS6 ){dataBlock[x]=0x44;}
if(melody[x]==NOTE_G6  ){dataBlock[x]=0x45;}
if(melody[x]==NOTE_GS6 ){dataBlock[x]=0x46;}
if(melody[x]==NOTE_A6  ){dataBlock[x]=0x47;}
if(melody[x]==NOTE_AS6 ){dataBlock[x]=0x48;}
if(melody[x]==NOTE_B6){dataBlock[x]=0x49;}
if(melody[x]==NOTE_C7 ){dataBlock[x]=0x4A;}
if(melody[x]==NOTE_CS7 ){dataBlock[x]=0x4B;}
if(melody[x]==NOTE_D7  ){dataBlock[x]=0x4C;}
if(melody[x]==NOTE_DS7 ){dataBlock[x]=0x4D;}
if(melody[x]==NOTE_E7  ){dataBlock[x]=0x4E;}
if(melody[x]==NOTE_F7  ){dataBlock[x]=0x4F;}
if(melody[x]==NOTE_FS7 ){dataBlock[x]=0x50;}
if(melody[x]==NOTE_G7  ){dataBlock[x]=0x51;}
if(melody[x]==NOTE_GS7 ){dataBlock[x]=0x52;}
if(melody[x]==NOTE_A7  ){dataBlock[x]=0x53;}
if(melody[x]==NOTE_AS7 ){dataBlock[x]=0x54;}
if(melody[x]==NOTE_B7  ){dataBlock[x]=0x55;}
if(melody[x]==NOTE_C8  ){dataBlock[x]=0x56;}
if(melody[x]==NOTE_CS8 ){dataBlock[x]=0x57;}
if(melody[x]==NOTE_D8  ){dataBlock[x]=0x58;}
if(melody[x]==NOTE_DS8 ){dataBlock[x]=0x59;}
    }
  }
void byte_array_to_melody(byte *buffer){
      for(int x; x<16; x++){
if(buffer[x]==0x01){melody[x]=NOTE_B0;}
if(buffer[x]==0x02){melody[x]=NOTE_C1;}
if(buffer[x]==0x03){melody[x]=NOTE_CS1;}
if(buffer[x]==0x04){melody[x]=NOTE_D1;}
if(buffer[x]==0x05){melody[x]=NOTE_DS1;}
if(buffer[x]==0x06){melody[x]=NOTE_E1;}
if(buffer[x]==0x07){melody[x]=NOTE_F1;}
if(buffer[x]==0x08){melody[x]= NOTE_FS1;}
if(buffer[x]==0x09){melody[x]= NOTE_G1;}
if(buffer[x]==0x0A){melody[x]= NOTE_GS1;}
if(buffer[x]==0x0B){melody[x]= NOTE_A1 ;}
if(buffer[x]==0x0C){melody[x]= NOTE_AS1 ;}
if(buffer[x]==0x0D){melody[x]= NOTE_B1  ;}
if(buffer[x]==0x0E){melody[x]= NOTE_C2  ;}
if(buffer[x]==0x0F){melody[x]= NOTE_CS2 ;}
if(buffer[x]==0x10){melody[x]= NOTE_D2  ;}
if(buffer[x]==0x11){melody[x]= NOTE_DS2 ;}
if(buffer[x]==0x12){melody[x]= NOTE_E2  ;}
if(buffer[x]==0x13){melody[x]= NOTE_F2  ;}
if(buffer[x]==0x14){melody[x]= NOTE_FS2 ;}
if(buffer[x]==0x15){melody[x]= NOTE_G2  ;}
if(buffer[x]==0x16){melody[x]= NOTE_GS2 ;}
if(buffer[x]==0x17){melody[x]= NOTE_A2  ;}
if(buffer[x]==0x18){melody[x]= NOTE_AS2 ;}
if(buffer[x]==0x19){melody[x]= NOTE_B2  ;}
if(buffer[x]==0x1A){melody[x]= NOTE_C3  ;}
if(buffer[x]==0x1B){melody[x]= NOTE_CS3 ;}
if(buffer[x]==0x1C){melody[x]= NOTE_D3  ;}
if(buffer[x]==0x1D){melody[x]= NOTE_DS3 ;}
if(buffer[x]==0x1E){melody[x]= NOTE_E3  ;}
if(buffer[x]==0x1F){melody[x]= NOTE_F3  ;}
if(buffer[x]==0x20){melody[x]= NOTE_FS3 ;}
if(buffer[x]==0x21){melody[x]= NOTE_G3  ;}
if(buffer[x]==0x22){melody[x]= NOTE_GS3 ;}
if(buffer[x]==0x23){melody[x]= NOTE_A3  ;}
if(buffer[x]==0x24){melody[x]= NOTE_AS3 ;}
if(buffer[x]==0x25){melody[x]= NOTE_B3  ;}
if(buffer[x]==0x26){melody[x]= NOTE_C4  ;}
if(buffer[x]==0x27){melody[x]= NOTE_CS4 ;}
if(buffer[x]==0x28){melody[x]= NOTE_D4  ;}
if(buffer[x]==0x29){melody[x]= NOTE_DS4 ;}
if(buffer[x]==0x2A){melody[x]= NOTE_E4  ;}
if(buffer[x]==0x2B){melody[x]= NOTE_F4  ;}
if(buffer[x]==0x2C){melody[x]= NOTE_FS4 ;}
if(buffer[x]==0x2D){melody[x]= NOTE_G4  ;}
if(buffer[x]==0x2E){melody[x]= NOTE_GS4 ;}
if(buffer[x]==0x2F){melody[x]= NOTE_A4  ;}
if(buffer[x]==0x30){melody[x]= NOTE_AS4 ;}
if(buffer[x]==0x31){melody[x]= NOTE_B4  ;}
if(buffer[x]==0x32){melody[x]= NOTE_C5  ;}
if(buffer[x]==0x33){melody[x]= NOTE_CS5 ;}
if(buffer[x]==0x34){melody[x]= NOTE_D5  ;}
if(buffer[x]==0x35){melody[x]= NOTE_DS5 ;}
if(buffer[x]==0x36){melody[x]= NOTE_E5  ;}
if(buffer[x]==0x37){melody[x]= NOTE_F5  ;}
if(buffer[x]==0x38){melody[x]= NOTE_FS5 ;}
if(buffer[x]==0x39){melody[x]= NOTE_G5  ;}
if(buffer[x]==0x3A){melody[x]= NOTE_GS5 ;}
if(buffer[x]==0x3B){melody[x]= NOTE_A5  ;}
if(buffer[x]==0x3C){melody[x]= NOTE_AS5 ;}
if(buffer[x]==0x3D){melody[x]= NOTE_B5  ;}
if(buffer[x]==0x3E){melody[x]= NOTE_C6  ;}
if(buffer[x]==0x3F){melody[x]= NOTE_CS6 ;}
if(buffer[x]==0x40){melody[x]= NOTE_D6  ;}
if(buffer[x]==0x41){melody[x]= NOTE_DS6 ;}
if(buffer[x]==0x42){melody[x]= NOTE_E6  ;}
if(buffer[x]==0x43){melody[x]= NOTE_F6  ;}
if(buffer[x]==0x44){melody[x]= NOTE_FS6 ;}
if(buffer[x]==0x45){melody[x]= NOTE_G6  ;}
if(buffer[x]==0x46){melody[x]= NOTE_GS6 ;}
if(buffer[x]==0x47){melody[x]= NOTE_A6  ;}
if(buffer[x]==0x48){melody[x]= NOTE_AS6 ;}
if(buffer[x]==0x49){melody[x]=NOTE_B6;}
if(buffer[x]==0x4A){melody[x]= NOTE_C7 ;}
if(buffer[x]==0x4B){melody[x]= NOTE_CS7 ;}
if(buffer[x]==0x4C){melody[x]= NOTE_D7  ;}
if(buffer[x]==0x4D){melody[x]= NOTE_DS7 ;}
if(buffer[x]==0x4E){melody[x]= NOTE_E7  ;}
if(buffer[x]==0x4F){melody[x]= NOTE_F7  ;}
if(buffer[x]==0x50){melody[x]= NOTE_FS7 ;}
if(buffer[x]==0x51){melody[x]= NOTE_G7  ;}
if(buffer[x]==0x52){melody[x]= NOTE_GS7 ;}
if(buffer[x]==0x53){melody[x]= NOTE_A7  ;}
if(buffer[x]==0x54){melody[x]= NOTE_AS7 ;}
if(buffer[x]==0x55){melody[x]= NOTE_B7  ;}
if(buffer[x]==0x56){melody[x]= NOTE_C8  ;}
if(buffer[x]==0x57){melody[x]= NOTE_CS8 ;}
if(buffer[x]==0x58){melody[x]= NOTE_D8  ;}
if(buffer[x]==0x59){melody[x]= NOTE_DS8 ;}


               }
      }

  

    
    
    
    
    
    

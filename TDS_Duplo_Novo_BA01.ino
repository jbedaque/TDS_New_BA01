#include "SoftwareSerial.h"

SoftwareSerial mytds(5, 4); // RX, TX ( Escolha aqui dois pinos digitais pra RX e TX pra comunicar com o módulo. Deve ser conectado ao modulo conforme indicado no silk screen)
const int ProtocolHeaderByteTDS = 0xAA; // TDS???
const int ProtocolHeaderByteTemp = 0xAB; // ?????
const int ProtocolHeaderByteError = 0xAC; // ???????
const int ProtocolHeaderLength = 1; // TDS?????
const int ProtocolBodyLength = 4; // TDS?????
const int ProtocolChecksumLength = 1; // ?????

bool appearToHaveValidMessage;
byte command[]={0xA0,0x00,0x00,0x00,0x00,0xA0}; // ????
byte receivedMessage[ProtocolBodyLength];

int tds1 = 0;
int tds2 = 0;
float temp1 = 0.00;
float temp2 = 0.00;

unsigned long oldTime = 0;
unsigned long previousTime = 0;

void setup() {
  Serial.begin(9600);
  mytds.begin(9600);
}

// converter Hex to DEC
int hexToDec (int high, int low) {
  int z = 0;
  z = (high<<8)|low;
  return z;
}

void checkTDS() {

  if ((millis() - oldTime) > 3000) {

    byte ProtocolHeader = 0x00;
    // ??????
    mytds.write(command,6);
    int availableBytes = mytds.available();
    while (availableBytes>0) {

      if (!appearToHaveValidMessage) {

        // ?????
        if (availableBytes >= ProtocolHeaderLength) {

          // ??1byte??
          byte firstByte = mytds.read();

          if (firstByte == ProtocolHeaderByteTDS ||
              firstByte == ProtocolHeaderByteTemp ||
              firstByte == ProtocolHeaderByteError) {
              // ??????
              appearToHaveValidMessage = true;
              ProtocolHeader = firstByte;
              availableBytes = mytds.available();
          }
        }
        else
        {
           Serial.print("not found protocol header!");
        }
      }

      if (availableBytes >= (ProtocolBodyLength + ProtocolChecksumLength) && appearToHaveValidMessage) {

        // ?????,???????????
        byte calculatedChecksum = 0x00;
        calculatedChecksum += ProtocolHeader;

        for (int i = 0; i < ProtocolBodyLength; i++) {
          receivedMessage[i] = mytds.read();
          calculatedChecksum += receivedMessage[i];
        }
        // ?????
        byte receivedChecksum = mytds.read();
        //  ?????
        if (receivedChecksum == calculatedChecksum) {
          // ??TDS??
          if (ProtocolHeader == ProtocolHeaderByteTDS)
          {
            tds1 = hexToDec(receivedMessage[0],receivedMessage[1]);
            tds2 = hexToDec(receivedMessage[2],receivedMessage[3]);
            Serial.print("tds1:");
            Serial.println(tds1);
            Serial.print("tds2:");
            Serial.println(tds2);
          }
          // ????????
          if (ProtocolHeader == ProtocolHeaderByteTemp)
          {
            temp1 = hexToDec(receivedMessage[0],receivedMessage[1])/100.00;
            temp2 = hexToDec(receivedMessage[2],receivedMessage[3])/100.00;
            Serial.print("temp1:");
            Serial.println(temp1);
            Serial.print("temp2:");
            Serial.println(temp2);
          }
          
          if (ProtocolHeader == ProtocolHeaderByteError)
          {
            Serial.print("error:");
            Serial.println(receivedMessage[0]);
          }
        } else {
          Serial.print("checksum error!receivedMessage:");
          Serial.println(receivedMessage[0]);
          Serial.println(receivedMessage[1]);
          Serial.println(receivedMessage[2]);
          Serial.println(receivedMessage[3]);
        }
      appearToHaveValidMessage = false;
      }
    // ??????12byte??,?? 6 byte TDS ??? 6 byte ????,??????????
    availableBytes -= 6;
    }
  oldTime = millis();
  }
 }


void loop() {
  checkTDS();
  // desenvolva aqui seu código.   temp1 temp2 tds1  tds2 :  são suas variáveis de trabalho.
}

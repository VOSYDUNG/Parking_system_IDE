#ifndef MyMFRC522_h
#define MyMFRC522_h
#include <MFRC522.h>
#include <SPI.h>
#define RST_PIN 9
#define SS_PIN 10

#define RST_PIN_2  1
#define SS_PIN_2   2

#define SENSOR_1  4
#define SENSOR_2  5
#define SENSOR_3  6
#define SENSOR_4  7


/* Create an instance of MFRC522 */
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN_2);
/* Create an instance of MIFARE_Key */
MFRC522::MIFARE_Key key;          

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNum = 2;  
int blockNum2 = 2; 
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
byte blockData [8] = {"20521215"};

/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[8];
byte readBlockData2[8];
String ID_Staff = "";
String UID_Card = "";

MFRC522::StatusCode status;
void WriteDataToBlock(int blockNum, byte blockData[]) ;
void IsNewCardPresent();
void ReadDataFromBlock(int blockNum, byte readBlockData[]) ;
void ReadCardData(String &uidString, byte staffID[]);
void ConvertToString(byte readBlockData[], String& str, int count);
void StopCard();
#endif
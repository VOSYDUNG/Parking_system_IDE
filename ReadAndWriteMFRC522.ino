#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include "MyMFRC522.h"
#include <string.h>
#include <SoftwareSerial.h>
const byte RX = 2;
const byte TX = 3;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
const byte RX2 = 7; // 15
const byte TX2 = 8;  //14
SoftwareSerial mySerial2 = SoftwareSerial(RX2, TX2);

bool IsStaff;
String CheckStaff = "11111111";
int value_1;
int value_2;
int value_3;
int value_4;
int Full = 1;
bool flag1 = 0;
bool flag2 = 0;
bool flag3 = 0;
bool flag4 = 0;
String DataSend = "";
void setup() 
{
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial2.begin(9600);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  mfrc522_2.PCD_Init();
  pinMode(SENSOR_1, INPUT);
}

void loop()
{
 // if(Full < 4){
  //Serial.println(Full);
  ID_Staff = "";
  UID_Card = "";
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  // Wait until detect card 
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){
  // Detech card
  Serial.print("\n");
  Serial.println("**Card Detected 2**");
 // WriteDataToBlock(blockNum,blockData);
  ReadCardData1(UID_Card, readBlockData, mfrc522);
 // ReadCardData(UID_Card, readBlockData);
  Serial.print("\nUID Card: ");
  Serial.println(UID_Card);
  Serial.print("ID staff: ");
  Serial.println(ID_Staff);

  DynamicJsonDocument doc(200);
  Serial.println(strcmp(ID_Staff.c_str(), CheckStaff.c_str()));

  if(!strcmp(ID_Staff.c_str(), CheckStaff.c_str())){    // is cus
    doc["UID_Card"] = UID_Card;
    doc["ID_Staff"] = CheckStaff;

  // Chuyển đối tượng JSON thành chuỗi JSON
  //  String jsonString;
  //  serializeJson(doc, jsonString);

  // In ra chuỗi JSON
  //  Serial.println(jsonString);
  }
  else {    // is staff
    doc["UID_Card"] = UID_Card;
    doc["ID_Staff"] = ID_Staff;

  // Chuyển đối tượng JSON thành chuỗi JSON
  //   String jsonString;
  //   serializeJson(doc, jsonString);

  // // In ra chuỗi JSON
  //   Serial.println(jsonString);
  //   mySerial.println(jsonString);
  }

  serializeJsonPretty(doc, Serial);
  serializeJsonPretty(doc, mySerial);

  // ===================================================================================== 
  /* If you want to print the full memory dump, uncomment the next line */
 // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
 // StopCard();
  StopCard1(mfrc522);
  }

  // if(mfrc522_2.PICC_IsNewCardPresent() && mfrc522_2.PICC_ReadCardSerial()){
  // // Detech card
  // Serial.print("\n");
  // Serial.println("**Card Detected**");

  //  ReadCardData1(UID_Card, readBlockData2, mfrc522_2);
  //  /* Authenticating the desired data block for Read access using Key A */
  // byte status = mfrc522_2.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522_2.uid));

  // if (status != MFRC522::STATUS_OK)
  // {
  //    Serial.print("Authentication failed for Read: ");
  //    Serial.println(mfrc522_2.GetStatusCodeName(status));
  //    return;
  // }
  // else
  // {
  //   Serial.println("Authentication success");
  // }

  // /* Reading data from the Block */
  // status = mfrc522_2.MIFARE_Read(blockNum2, readBlockData2, &bufferLen);
  // if (status != MFRC522::STATUS_OK)
  // {
  //   Serial.print("Reading failed: ");
  //   Serial.println(mfrc522_2.GetStatusCodeName(status));
  //   return;
  // }
  // else
  // {
  //   Serial.println("Block was read successfully");  
  // }
  // // Serial.print("\nUID Card: ");
  // // Serial.println(UID_Card);
  // // Serial.print("ID staff: ");
  // // Serial.println(ID_Staff);

  // DynamicJsonDocument doc(200);
  // Serial.println(strcmp(ID_Staff.c_str(), CheckStaff.c_str()));

  // if(!strcmp(ID_Staff.c_str(), CheckStaff.c_str())){    // is staff
  //   doc["UID_Card"] = UID_Card;
  //   doc["ID_Staff"] = CheckStaff;

  // // Chuyển đối tượng JSON thành chuỗi JSON
  // //   String jsonString;
  // //   serializeJson(doc, jsonString);

  // // // In ra chuỗi JSON
  // //   Serial.println(jsonString);
  // }
  // else {    // is customer
  //   doc["UID_Card"] = UID_Card;
  //   doc["ID_Staff"] = ID_Staff;

  // // // Chuyển đối tượng JSON thành chuỗi JSON
  // //   String jsonString;
  // //   serializeJson(doc, jsonString);

  // // // In ra chuỗi JSON
  // //   Serial.println(jsonString);
  // }

  // // ===================================================================================== 
  // /* If you want to print the full memory dump, uncomment the next line */
  // //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  // serializeJsonPretty(doc, Serial);
  // serializeJsonPretty(doc, mySerial);
  // StopCard1(mfrc522_2);
  // }

  value_1 = digitalRead(SENSOR_1);
  if(value_1 == 0 && flag1 == 0){
    DataSend = "{\"vt1\":\"1\"}";
    StaticJsonDocument<200> JSON1;
    DeserializationError error1 = deserializeJson(JSON1, DataSend);
    if (error1)
    {
      Serial.println("JSON1 error!!!");
      return;
    }
    else
    {
      Serial.println("Đã gửi lệnh!!!");
      serializeJsonPretty(JSON1, mySerial);
      serializeJsonPretty(JSON1, Serial);
    }
    flag1 = 1;
    Full++;
  }

  if(value_1 == 1 && flag1 == 1){
    DataSend = "{\"vt1\":\"0\"}";
    StaticJsonDocument<200> JSON1;
    DeserializationError error1 = deserializeJson(JSON1, DataSend);
    if (error1)
    {
      Serial.println("JSON1 error!!!");
      return;
    }
    else
    {
      Serial.println("Đã gửi lệnh!!!");
      serializeJsonPretty(JSON1, mySerial);
      serializeJsonPretty(JSON1, Serial);
    }
    flag1 = 0;
    Full--;
  }
 // ===============================VALUE 2========================================================
  value_2 = digitalRead(SENSOR_2);
  if(value_2 == 0 && flag2 == 0){
    DataSend = "{\"vt2\":\"1\"}";
    StaticJsonDocument<200> JSON1;
    DeserializationError error1 = deserializeJson(JSON1, DataSend);
    if (error1)
    {
      Serial.println("JSON1 error!!!");
      return;
    }
    else
    {
      Serial.println("Đã gửi lệnh!!!");
      serializeJsonPretty(JSON1, mySerial);
      serializeJsonPretty(JSON1, Serial);
    }
    flag2 = 1;
    Full++;
  }
  if(value_2 == 1 && flag2 == 1){
    DataSend = "{\"vt2\":\"0\"}";
    StaticJsonDocument<200> JSON1;
    DeserializationError error1 = deserializeJson(JSON1, DataSend);
    if (error1)
    {
      Serial.println("JSON1 error!!!");
      return;
    }
    else
    {
      Serial.println("Đã gửi lệnh!!!");
      serializeJsonPretty(JSON1, mySerial);
      serializeJsonPretty(JSON1, Serial);
    }
    flag2 = 0;
    Full--;
  }

  // ===============================VALUE 3========================================================
  value_3 = digitalRead(SENSOR_3);
  if(value_3 == 0 && flag3 == 0){
    DataSend = "{\"vt3\":\"1\"}";
    StaticJsonDocument<200> JSON1;
    DeserializationError error1 = deserializeJson(JSON1, DataSend);
    if (error1)
    {
      Serial.println("JSON1 error!!!");
      return;
    }
    else
    {
      Serial.println("Đã gửi lệnh!!!");
      serializeJsonPretty(JSON1, mySerial);
      serializeJsonPretty(JSON1, Serial);
    }
    flag3 = 1;
    Full++;
  }
  if(value_3 == 1 && flag3 == 1){
    DataSend = "{\"vt3\":\"0\"}";
    StaticJsonDocument<200> JSON1;
    DeserializationError error1 = deserializeJson(JSON1, DataSend);
    if (error1)
    {
      Serial.println("JSON1 error!!!");
      return;
    }
    else
    {
      Serial.println("Đã gửi lệnh!!!");
      serializeJsonPretty(JSON1, mySerial);
      serializeJsonPretty(JSON1, Serial);
    }
    flag3 = 0;
    Full--;
  }
  // // ===============================VALUE 4========================================================
  //   value_4 = digitalRead(SENSOR_4);
  // if(value_4 == 0 && flag4 == 0){
  //   DataSend = "{\"vt4\":\"1\"}";
  //   StaticJsonDocument<200> JSON1;
  //   DeserializationError error1 = deserializeJson(JSON1, DataSend);
  //   if (error1)
  //   {
  //     Serial.println("JSON1 error!!!");
  //     return;
  //   }
  //   else
  //   {
  //     Serial.println("Đã gửi lệnh!!!");
  //     serializeJsonPretty(JSON1, mySerial);
  //     serializeJsonPretty(JSON1, Serial);
  //   }
  //   flag4 = 1;
  // }
  // if(value_4 == 1 && flag4 == 1){
  //   DataSend = "{\"vt4\":\"0\"}";
  //   StaticJsonDocument<200> JSON1;
  //   DeserializationError error1 = deserializeJson(JSON1, DataSend);
  //   if (error1)
  //   {
  //     Serial.println("JSON1 error!!!");
  //     return;
  //   }
  //   else
  //   {
  //     Serial.println("Đã gửi lệnh!!!");
  //     serializeJsonPretty(JSON1, mySerial);
  //     serializeJsonPretty(JSON1, Serial);
  //   }
  //   flag4 = 0;
  // }
  //}
  // else {
  //   Serial.println("FULL");
  // }

    if(mySerial2.available()){
    Serial.println("CHAY VO HAM NAY");
    const size_t capacity = JSON_OBJECT_SIZE(2) + 256;
    DynamicJsonDocument JSON2(capacity);

    DeserializationError error1 = deserializeJson(JSON2, mySerial2); // check lỗi gói dữ liệu
    //error  = true or false , true => dữ liệu JSON error
    if (error1)
    {
      Serial.println("Data JSON Error .....!!!");

      return;
    }
    else{
      if(JSON2["object"] == "1"){
        Serial.println("Xe Vo");
      }
      else if(JSON2["object"] == "0"){
        Serial.println("Xe ra");
      }
    }
  }
}



void WriteDataToBlock1(int blockNum, byte blockData[], MFRC522& rfid)
{
  /* Authenticating the desired data block for write access using Key A */
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Write data to the block */
  status = rfid.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
}

void ReadDataFromBlock1(int blockNum, byte readBlockData[], MFRC522& rfid)
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(rfid.uid));

  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Read: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  byte bufferLen = 18; // Đảm bảo buffer đủ lớn để chứa dữ liệu
  status = rfid.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");
  }
}

void ReadCardData1(String &uidString, byte staffID[], MFRC522& rfid){

  // Read UID still hex
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidString += String(rfid.uid.uidByte[i], HEX);
  }

  // Read if have ID staff (ID_staff convert to Char)
  ReadDataFromBlock1(blockNum, staffID, rfid);
  ConvertToString(staffID, ID_Staff, 8);
}

void StopCard1(MFRC522& rfid){
    // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void ReadDataFromBlock2(int blockNum, byte readBlockData[]) 
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522_2.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522_2.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522_2.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522_2.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522_2.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
  }
  
}



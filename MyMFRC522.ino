// #include "MyMFRC522.h"
void WriteDataToBlock(int blockNum, byte blockData[]) 
{
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  
  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
  
 }
// void IsNewCardPresent(){
//   for (byte i = 0; i < 6; i++)
//   {
//     key.keyByte[i] = 0xFF;
//   }
//   if ( ! mfrc522.PICC_IsNewCardPresent())
//   {
//     return;
//   }
  
//   /* Select one of the cards */
//   if ( ! mfrc522.PICC_ReadCardSerial()) 
//   {
//     return;
//   }
// }  
// void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
// {
//   /* Authenticating the desired data block for Read access using Key A */
//   byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

//   if (status != MFRC522::STATUS_OK)
//   {
//      Serial.print("Authentication failed for Read: ");
//      Serial.println(mfrc522.GetStatusCodeName(status));
//      return;
//   }
//   else
//   {
//     Serial.println("Authentication success");
//   }

//   /* Reading data from the Block */
//   status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
//   if (status != MFRC522::STATUS_OK)
//   {
//     Serial.print("Reading failed: ");
//     Serial.println(mfrc522.GetStatusCodeName(status));
//     return;
//   }
//   else
//   {
//     Serial.println("Block was read successfully");  
//   }
  
// }
// void ReadCardData(String &uidString, byte staffID[]){

//   // Read UID still hex
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     uidString += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
//     uidString += String(mfrc522.uid.uidByte[i], HEX);
//   }

//   // Read if have ID staff (ID_staff convert to Char)
//   ReadDataFromBlock(blockNum, staffID);
//   ConvertToString(staffID, ID_Staff, 8);
// }

void ConvertToString(byte readBlockData[], String& str, int count){
  for (int i = 0; i < count; i++) {
    str = str + (char)readBlockData[i];    // str store the ID of staff
  }
}
// void StopCard(){
//     // Halt PICC
//   mfrc522.PICC_HaltA();
//   // Stop encryption on PCD
//   mfrc522.PCD_StopCrypto1();
// }


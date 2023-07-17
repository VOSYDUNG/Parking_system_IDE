#include <ArduinoJson.h>
#include <SoftwareSerial.h>
const byte RX2 = 14;
const byte TX2 = 15;
SoftwareSerial mySerial(RX2, TX2);
void In_signal(){
    String DataSend = "{\"object\":\"1\"}";
    StaticJsonDocument<200> JSON_signal;
    DeserializationError error1 = deserializeJson(JSON_signal, DataSend);
    if (error1)
    {
      Serial.println("JSON signal error!!!");
      return;
    }
    else
    {
      serializeJsonPretty(JSON_signal, Serial);
    }
}
void Out_signal(){
    String DataSend = "{\"object\":\"0\"}";
    StaticJsonDocument<200> JSON_signal;
    DeserializationError error1 = deserializeJson(JSON_signal, DataSend);
    if (error1)
    {
      Serial.println("JSON signal error!!!");
      return;
    }
    else
    {
      serializeJsonPretty(JSON_signal, mySerial);
    }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {

  Out_signal();
  delay(1000);
  // put your main code here, to run repeatedly:
}

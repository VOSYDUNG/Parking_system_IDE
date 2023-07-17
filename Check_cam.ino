#include <map>
#include "WiFi.h"
#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include <FS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>
#include <SoftwareSerial.h>
const byte RX2 = 14;
const byte TX2 = 15;
SoftwareSerial mySerial(RX2, TX2);
//Replace with your network credentials
const char* ssid ="Circle K"; //"Ph
const char* password = "0395510527"; 
//time
const char* ntpServer = "pool.ntp.org";
const int timeZone = 7;


// Insert Firebase project API Key
#define API_KEY "AIzaSyC1ExaMgSRLz9PrCRZfp9i4a9J-mcPdWs4"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "parkingsystemesp32@gmail.com"
#define USER_PASSWORD "Parking123@@"

// Insert Firebase storage bucket ID e.g bucket-name.appspot.com
#define STORAGE_BUCKET_ID "parkingsystem-36300.appspot.com"
#define DATABASE_URL "https://parkingsystem-36300-default-rtdb.asia-southeast1.firebasedatabase.app" 
// Photo File Name to save in SPIFFS
//#define FILE_PHOTO "/data/photo.jpg"

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

boolean takeNewPhoto = true;

//Define Firebase Data objects
FirebaseData fbdo,fbdo_img;
FirebaseAuth auth;
FirebaseConfig configF;
String FILE_PHOTO,FILE_PHOTO_OUT;
bool taskCompleted = false;
int day,month,year,hour,minute,second;
String Date,Time;
WiFiUDP ntpUDP;
std::map<String, int> uidCountMap;
std::map<String, int>ledPin;
FirebaseData firebaseData;
NTPClient timeClient(ntpUDP, ntpServer, timeZone * 3600, 60000);
// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}
void timeinit(){
  timeClient.begin();
  timeClient.update();
}
// void getTime(){
//   struct tm timeinfo;
//   if(!getLocalTime(&timeinfo)){
//     Serial.println("Failed to obtain time");
//     return;
//   }
//     day = timeinfo.tm_mday;
//     month = timeinfo.tm_mon + 1; // Tháng được đánh số từ 0 đến 11, nên cộng thêm 1
//     year = timeinfo.tm_year + 1900; // Năm được tính từ 1900
//     hour = timeinfo.tm_hour;
//     minute = timeinfo.tm_min;
//     second = timeinfo.tm_sec;
//     Date = String(day)+"-"+String(month)+"-"+String(year);
//     Time = String(hour)+":"+String(minute)+":"+String(second);
    
// }
// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs(String FILE_PHOTO) {
  // Take a photo with the camera
  Serial.println("Taking a photo...");

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Photo file name
  Serial.printf("Picture file name: %s\n", FILE_PHOTO.c_str());
  File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
    esp_camera_fb_return(fb);
    return;
  }

  // Write the photo data to the file
  file.write(fb->buf, fb->len);
  Serial.print("The picture has been saved in ");
  Serial.print(FILE_PHOTO);
  Serial.print(" - Size: ");
  Serial.print(file.size());
  Serial.println(" bytes");

  // Close the file
  file.close();
  esp_camera_fb_return(fb);
  return;
}
void In_signal(String uid){
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
      serializeJsonPretty(JSON_signal, mySerial);
      digitalWrite(ledPin[uid], HIGH);
//      Serial.println(ledPin[uid]);
    }
}
void Out_signal(String uid){
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
      digitalWrite(ledPin[uid], LOW);
//      Serial.println(ledPin[uid]);
    }
}
void initWiFi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void initSPIFFS(){
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
}

void initCamera(){
 // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  } 
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);
  initWiFi();
  initSPIFFS();
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  initCamera();
//  getTime();
  timeinit();
  ///pinMode(ledPin, OUTPUT);
  //Firebase
  // Assign the api key
  configF.api_key = API_KEY;
  configF.database_url = DATABASE_URL;
  //Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  //Assign the callback function for the long running token generation task
  configF.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&configF, &auth);
  Firebase.reconnectWiFi(true);
  uidCountMap["007ea11b"]=0;
  uidCountMap["a175650d"]=0;
  uidCountMap["2159371d"]=0;
  uidCountMap["53f60d96"]=0;
  uidCountMap["b37c2996"]=0;
  uidCountMap["007ea11a"]=0;
  ledPin["007ea11b"]=13;
  ledPin["a175650d"]=13;
  ledPin["2159371d"]=13;
  ledPin["53f60d96"]=14;
  ledPin["b37c2996"]=15;
  ledPin["007ea11a"]=12;
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  digitalWrite(13, HIGHT);
}

void loop() {
  if(Serial.available()) {
    
//    digitalWrite(ledPin, LOW);
    timeClient.update();
      // Thêm các thành phần dữ liệu
      // dữ liệu từ UART của Arduino
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + 256;
//  StaticJsonDocument<bufferSize> jsonDoc;
    DynamicJsonDocument JSON1(bufferSize);
    String JSONSTRING = "{\"UID_Card\": \"007ea11b\", \"ID_Staff\": \"20521945\"}";
//    String JSONSTRING = "{\"vt1\":\"1\"}";
    DeserializationError error = deserializeJson(JSON1, Serial);
    if (error) {
        Serial.print("Deserialization failed: ");
        Serial.println(error.c_str());
        return;
    }
    const char* uid = JSON1["UID_Card"]; // Lấy giá trị trường UID_Card
    const char* id = JSON1["ID_Staff"];  // Lấy giá trị trường ID_Staff
    const char* vt1 = JSON1["vt1"];
    const char* vt2 = JSON1["vt2"];
    const char* vt3 = JSON1["vt3"];
    const char* vt4 = JSON1["vt4"];
    //Gửi lên firebase 
    FirebaseJson json;
//    json.add("UID", String(uid));
    if (Firebase.ready() && !taskCompleted){
    taskCompleted = true;
    Serial.println("kiem tra uid và vt");
         
    if (JSON1.containsKey("UID_Card")) {
        if (Firebase.RTDB.getString(&firebaseData, "/data/customer/"+String(uid))) {
              uidCountMap[String(uid)]++;
              json.add("ID", String(id));
              json.add("time",String(timeClient.getFormattedTime()));
              FILE_PHOTO_OUT = "/Images_out/"+String(uid)+"."+String(uidCountMap[String(uid)])+".jpg";
              if (takeNewPhoto) {
                capturePhotoSaveSpiffs(FILE_PHOTO_OUT);
                takeNewPhoto = false;
              }
              Firebase.Storage.upload(&fbdo_img, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO_OUT /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, FILE_PHOTO_OUT /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */);
              json.add("imageURL", String(fbdo_img.downloadURL().c_str()));
              if (Firebase.RTDB.setJSON(&firebaseData, "/data/customer/"+ String(uid)+"-OUT/"+String(uid)+"-"+String(uidCountMap[String(uid)]), &json)) {
                      Serial.println("Data pushed successfully");
                      taskCompleted = false;
                      Out_signal(String(uid));
                      Firebase.RTDB.deleteNode(&firebaseData,"/data/customer/"+String(uid));
                    }
                    else {
                      Serial.println("Data push failed");
                      Serial.println(firebaseData.errorReason());
                    }
        }
        else if (Firebase.RTDB.getString(&firebaseData, "/data/staff/"+String(uid))){
              uidCountMap[String(uid)]++;
              json.add("ID", String(id));
              json.add("time",String(timeClient.getFormattedTime()));
              FILE_PHOTO_OUT = "/Images_out/"+String(uid)+"."+String(uidCountMap[String(uid)])+".jpg";
              if (takeNewPhoto) {
                capturePhotoSaveSpiffs(FILE_PHOTO_OUT);
                takeNewPhoto = false;
              }
              Firebase.Storage.upload(&fbdo_img, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO_OUT /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, FILE_PHOTO_OUT /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */);
              json.add("imageURL", String(fbdo_img.downloadURL().c_str()));
              if (Firebase.RTDB.setJSON(&firebaseData, "/data/staff/"+ String(uid)+"-OUT/"+String(uid)+"-"+String(uidCountMap[String(uid)]), &json)) {
                      Serial.println("Data pushed successfully");
                      taskCompleted = false;
                      Out_signal(String(uid));
                      Firebase.RTDB.deleteNode(&firebaseData,"/data/staff/"+String(uid));
                    }
                    else {
                      Serial.println("Data push failed");
                      Serial.println(firebaseData.errorReason());
                    }
        }
        else{
            Serial.println("UID không tồn tại trong Firebase");
            json.add("ID", String(id));
            json.add("time",String(timeClient.getFormattedTime()));
            FILE_PHOTO = "/Images/"+String(uid)+".jpg";
            if (takeNewPhoto) {
              capturePhotoSaveSpiffs(FILE_PHOTO);
              takeNewPhoto = false;
            }
            Serial.print("Uploading picture... ");
            //MIME type should be valid to avoid the download problem.
            //The file systems for flash and SD/SDMMC can be changed in FirebaseFS.h.
            if (Firebase.Storage.upload(&fbdo_img, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, FILE_PHOTO /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */)){
              Serial.printf("\nDownload URL: %s\n", fbdo_img.downloadURL().c_str());
              json.add("imageURL", String(fbdo_img.downloadURL().c_str()));
              if(JSON1["ID_Staff"]!="11111111"){
              
                if (Firebase.RTDB.setJSON(&firebaseData, "/data/staff/"+ String(uid), &json)) {
                  Serial.println("Data pushed successfully");
                  In_signal(String(uid));
                  takeNewPhoto = true;
                  taskCompleted = false;
                }
                else {
                  Serial.println("Data push failed");
                  Serial.println(firebaseData.errorReason());
                }
              }
                else{ 
               
                    if (Firebase.RTDB.setJSON(&firebaseData, "/data/customer/"+ String(uid), &json)) {
                      Serial.println("Data pushed successfully");
                      In_signal(String(uid));
                      takeNewPhoto = true;
                      taskCompleted = false;
                    }
                    else {
                      Serial.println("Data push failed");
                      Serial.println(firebaseData.errorReason());
                    }
                }
            }else{
              Serial.println(fbdo_img.errorReason());
            }
        }
    }else if ( JSON1.containsKey("vt1")){
        Serial.println("day vt");
        json.add("vt1",String(vt1));
      // Thêm các thành phần dữ liệu
      // dữ liệu từ UART của Arduino
//      if(JSON1["vt1"]=="1"){
        if (Firebase.RTDB.setJSON(&firebaseData, "/data/location/VT1", &json)) {
          Serial.println("Data pushed successfully");
          takeNewPhoto = true;
          taskCompleted = false;
        }
        else {
          Serial.println("Data push failed");
          Serial.println(firebaseData.errorReason());
        }
//      }
    }else if ( JSON1.containsKey("vt2")){
      json.add("vt2",String(vt2));
      // Thêm các thành phần dữ liệu
      // dữ liệu từ UART của Arduino
//      if(JSON1["vt1"]=="1"){
      if (Firebase.RTDB.setJSON(&firebaseData, "/data/location/VT2", &json)) {
        Serial.println("Data pushed successfully");
        takeNewPhoto = true;
        taskCompleted = false;
      }
      else {
        Serial.println("Data push failed");
        Serial.println(firebaseData.errorReason());
      }
//      }
    }else if ( JSON1.containsKey("vt3")){
      json.add("vt3",String(vt3));
      // Thêm các thành phần dữ liệu
      // dữ liệu từ UART của Arduino
//      if(JSON1["vt1"]=="1"){
      if (Firebase.RTDB.setJSON(&firebaseData, "/data/location/VT3", &json)) {
        Serial.println("Data pushed successfully");
        takeNewPhoto = true;
        taskCompleted = false;
      }
      else {
          Serial.println("Data push failed");
          Serial.println(firebaseData.errorReason());
      }
//      }
    }else if ( JSON1.containsKey("vt4")){
      json.add("vt4",String(vt4));
      // Thêm các thành phần dữ liệu
      // dữ liệu từ UART của Arduino
//      if(JSON1["vt1"]=="1"){
      if (Firebase.RTDB.setJSON(&firebaseData, "/data/location/VT4", &json)) {
        Serial.println("Data pushed successfully");
        takeNewPhoto = true;
        taskCompleted = false;
      }
      else {
        Serial.println("Data push failed");
        Serial.println(firebaseData.errorReason());
      }
    }
  }
  }
}
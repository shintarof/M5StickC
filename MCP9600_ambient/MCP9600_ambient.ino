#include <M5StickC.h>
#include "Ambient.h"

WiFiClient client;
Ambient ambient;

const char* ssid = "********";
const char* password = "********";
unsigned int channelId = ****; // AmbientのチャネルID
const char* writeKey = "**********d"; // ライトキー

void setup() {
  M5.begin();
  Wire.begin(0, 26);

  WiFi.begin(ssid, password);  //  Wi-Fi APに接続 ----A
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
    delay(500);
  }
  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化

}

void loop() {
  byte error, address;
  int nDevices;
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(4);
//  Serial.println("Scanning... Wire");
  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      //Serial.print("I2C device found at address 0x");
      if (address < 16)
      //Serial.print("0");
      //Serial.print(address, HEX);
      Wire.write(0x00); 
      Wire.endTransmission(false) ;
      Wire.requestFrom(address, 2, true);
      byte val1 = Wire.read();
      byte val2 = Wire.read();
      double temp = 0.0625*(val1*256 + val2);
      Serial.println(temp);
      M5.Lcd.setCursor(20, 20, 1);
      M5.Lcd.print(temp);

      ambient.set(1, temp);
      ambient.send();
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);  
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(1000*10);
}

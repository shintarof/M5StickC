#include <M5StickC.h>

float pitch, roll, yaw;
boolean flag = false;
float target = 84.0;

void writeRegister(byte vset,byte Hbridge){//vset 電圧設定 データシートp10、Hbridge = 0x01;//回転方向(ブリッジ制御) データシートp8 0x01 正転 0x02逆転
  Wire.beginTransmission(0x64);//I2Cの初期設定 データシートp12
  Wire.write(0x00);//
  int vdata = (vset << 2) + Hbridge;   //DRV8830に送る信号 データシートp13 D0-1 Hブリッジ D2-D7 vset の形式に変化n
  Wire.write(vdata);
  Wire.endTransmission();
}

void drawScreen() {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("target:%6.1f\n",target);
  M5.Lcd.printf("roll:%6.1f\n",roll);
  M5.Lcd.printf("now:%4.1f\n", target - roll);
}

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(4);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.MPU6886.Init();
  Wire.begin(0, 26); //SDA, SCL
  Serial.begin(115200);
}

void loop() {
  M5.update();
  M5.MPU6886.getAhrsData(&pitch,&roll,&yaw);
  drawScreen();
  if(Serial.read()==0x31){
    flag = !flag;
    if(!flag){
      writeRegister(0x00,0x03);
    }
  }   
  if(flag){ 
    // on/off control
    if(roll>=target){
       writeRegister(0x09,0x02);
    }else{
       writeRegister(0x09,0x01);
    }
    Serial.println(roll);
    delay(10);
  }  
}

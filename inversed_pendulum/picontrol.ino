#include <M5StickC.h>
#include <BlynkSimpleEsp32_BLE.h>
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT

boolean flag = false;
float pitch, roll, yaw;
float target = 84.0,p=0,i=0,d=0,output,vset;
float now_time,dt,old_time;
float kp =1.0,ki=1.0;

const uint16_t Led = 10;
char auth[] = "******************";

BLYNK_WRITE(V0){
  flag = !flag;
  i = 0;
  if(flag){
    digitalWrite(Led,0); 
  }else{
    digitalWrite(Led,1); 
  }
}
  
BLYNK_WRITE(V1){
  kp = param.asFloat();
}

BLYNK_WRITE(V2){
  ki = param.asFloat();
}

BLYNK_WRITE(V3){
  target = param.asFloat();
}

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
  M5.Lcd.printf("p:%4.2f\n", (target - roll)/target);
  M5.Lcd.printf("i:%f\n", i);
  M5.Lcd.printf("output:%4.2f\n", output);
  M5.Lcd.printf("vset:%4.2f\n", vset);
  M5.Lcd.printf("kp:%4.2f\n", kp);
  M5.Lcd.printf("ki:%4.2f\n", ki);
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
  Blynk.setDeviceName("Blynk");
  Blynk.begin(auth);
  pinMode(Led,OUTPUT);
  digitalWrite(Led,1); 
}

void loop() {
 Blynk.run();
 M5.update();
 M5.MPU6886.getAhrsData(&pitch,&roll,&yaw);
 drawScreen();

  if(!flag){
    writeRegister(0x00,0x03);
  }
  if(flag){
    //p control   
    p = (target - roll)/target; // 1 ~ 0 ~ -1 

    //i control
    dt = (micros() - old_time)/1000000;
    old_time = now_time;
    i += p * dt;
    if(i>0.6){
      i=0.0;
    }

    output = kp*p+ki*i;
    vset = abs(output);
    
    if(p>0){
      writeRegister(int(vset),0x01);
    }else{
      writeRegister(int(vset),0x02);
    }
    delay(10);
  }  
}

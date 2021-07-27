#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//----舵机部分---
#define pos_max 180
#define pos_min 90
#define left_pos_max 120
#define left_pos_min 95
#define left_pos_stop 109
#define right_pos_max 180
#define right_pos_min 0
#define right_pos_stop 93
const int numberOfServos = 8; // Number of servos
Servo servo[numberOfServos];  // 定义Servo对象来控制
int pos = pos_min;    // 角度存储变量
int left_pos = left_pos_stop;    // 角度存储变量
int right_pos = right_pos_stop;    // 角度存储变量
//----网络部分---
unsigned int localPort = 6666;      // 可以自定义端口号
int packetSize = 0;
int n;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];   
WiFiUDP Udp;

void setup() {

  Serial.begin(115200);
  Serial.println("Robot Starts Initialization");
//---------------
  WiFi.mode(WIFI_AP);
  IPAddress softLocal(192,168,1,1);                      // 设置内网WIFI IP地址
  IPAddress softGateway(192,168,1,1);
  IPAddress softSubnet(255,255,255,0);
  WiFi.softAPConfig(softLocal, softGateway, softSubnet);
   
  String apName = ("dachou_Mini_forklift");  // 设置WIFI名称 //+(String)ESP.getChipId()
  const char *softAPName = apName.c_str();
   
  WiFi.softAP(softAPName, "55588888");      //  创建wifi  名称 + 密码 
   
  IPAddress myIP = WiFi.softAPIP(); 
  Serial.println("");
  Serial.print("AP直链模式 网关IP: ");     
  Serial.println(myIP);
   
  Serial.print("AP SSID: ");   //串口打印WIFI 名称
  Serial.println(apName);
  Serial.print("AP 密码: ");        
  Serial.println("55588888");  //串口打印WIFI 密码
//---------
  Udp.begin(localPort);   //UDP服务初始化
}

void loop() { 
  packetSize = Udp.parsePacket();
  if (packetSize)  //等待接受UDP数据，一旦收到就进入处理
  {          
    n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);  
    packetBuffer[n] = 0;
    Serial.print("收到数据:");
    Serial.println(packetBuffer);
    if(!(strcmp(packetBuffer,"go")))  //收到的数据为前进
    {
      left_pos<left_pos_max?left_pos++:left_pos=left_pos_max; //角度变化，限制角度
      right_pos>right_pos_min?right_pos--:right_pos=right_pos_min;  //角度变化，限制角度
    }
    else if(!(strcmp(packetBuffer,"retreat")))
    {
      left_pos>left_pos_min?left_pos--:left_pos=left_pos_min; //角度变化，限制角度
      right_pos<right_pos_max?right_pos++:right_pos=right_pos_max;  //角度变化，限制角度
    }
    else if(!(strcmp(packetBuffer,"left")))
    {
      right_pos>right_pos_min?right_pos--:right_pos=right_pos_min;  //角度变化，限制角度
    }
    else if(!(strcmp(packetBuffer,"right")))
    {
      left_pos<left_pos_max?left_pos++:left_pos=left_pos_max; //角度变化，限制角度
    }
    else
    {
      left_pos=left_pos_stop;
      right_pos=right_pos_stop;
    }
  }
  if(!(strcmp(packetBuffer,"turnLeft")))
  {
    pos<pos_max?pos++:pos=pos_max;  //角度变化，限制角度
  }
  else if(!(strcmp(packetBuffer,"turnRight")))
  {
    pos>pos_min?pos--:pos=pos_min;  //角度变化，限制角度
  }
  servo[0].attach(12);   
  servo[0].write(left_pos); // 舵机角度写入
  servo[1].attach(13);   
  servo[1].write(right_pos);  // 舵机角度写入
  servo[2].attach(14);  
  servo[2].write(pos);  // 舵机角度写入
  delay(15);
}

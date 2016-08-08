#include<pt.h>
#include<Servo.h>

// ProtoThread.
#define PT_DELAY(pt, ms, ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

// GPIO Pins Definition.
#define MAGNO A5
#define GYROX A1
#define GYROY A2
#define GYROZ A3
#define LED1 12

Servo myservo, myservo1; 
int pos = 0, magnetR = 0;
int duration  = 0, distance = 0 ;
int x = 0, y = 0, z = 0;
int a = 0, b = 0, c = 0;
long gauss = 0;
String str = "";

//Rain
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
int range;

struct pt pt_taskSendSerial;
struct pt pt_taskSerialEvent;
struct pt pt_taskMagneto;
struct pt pt_taskGyro;
struct pt pt_taskRain;

PT_THREAD(taskSendSerial(struct pt* pt)){
  static uint32_t ts;
  String senda = "";
  PT_BEGIN(pt);
  while (1){      
      senda = String(x) + "," + String(y) + "," + String(z) + "," + String(gauss) + "," + String(range);
      //Serial.println(senda);
      Serial1.println(senda);        
      PT_DELAY(pt, 500, ts);
  }
  PT_END(pt);
}

PT_THREAD(taskRain(struct pt* pt)){
  static uint32_t ts;
  static int sensorReading;
  PT_BEGIN(pt);
  while(1){
    sensorReading = analogRead(A0);
    //a = str.substring(0,str.indexOf("/r")).toInt();
    //b = str.substring(str.indexOf(",")+1, str.indexOf(",", str.indexOf(",") + 1)).toInt();
    //c = str.substring(str.indexOf(",", str.indexOf(",") + 1), str.length()).toInt();
    range = map(sensorReading, sensorMin, sensorMax, 0, 3);
    if(range > 0 && a == 0)    
        myservo1.write(180);
    else if (range == 0 && a == 0)
        myservo1.write(90);
    PT_DELAY(pt, 500, ts);

  }
  PT_END(pt);
}

PT_THREAD(taskSerialEvent(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1){
    if(Serial1.available()) {
      str = Serial1.readStringUntil('\r');
      str.replace("\r","");
      str.replace("\n","");
      Serial.println(str);            
    }
    PT_YIELD(pt);
  }
  PT_END(pt);
}


PT_THREAD(taskMagneto(struct pt* pt)) {
    static uint32_t ts;
    PT_BEGIN(pt);
    while (1) {
        magnetR = analogRead(MAGNO);
        findNorth();
        PT_DELAY(pt, 100, ts);        
    }
    PT_END(pt);
}

void findNorth()
{
    gauss = magnetR - 525;
    if (gauss > -10)     
    {
        for (pos = 0; pos <= 180; pos += 1) { 
            myservo.write(pos);   
            delay(15);     
            if(gauss <= -10)
                break;               
        }
    }
    myservo.write(90); 
}

PT_THREAD(taskGyro(struct pt* pt)) {
    static uint32_t ts;
    PT_BEGIN(pt);
    while (1) {
        x = analogRead(GYROX) - 340;
        y = analogRead(GYROY) - 350;
        z = analogRead(GYROZ) - 410;
        PT_DELAY(pt, 500, ts);
    }
    PT_END(pt);
}

void setup() {
    Serial1.begin(115200);
    Serial.begin(9600);
    // Servo Motor Pin Initialization                                          
    myservo.attach(9);
    myservo1.attach(10);
    // Magnetic Pins Initialization.
    pinMode(MAGNO, INPUT);
    // LEDs Pins Initialization.
    pinMode(LED1, OUTPUT);    
    // Temperature Sensor Pin Initialization.
    //pinMode(TEMP_SENSOR, INPUT);
    // Initialize ProtoThreads.
    PT_INIT(&pt_taskSerialEvent);
    PT_INIT(&pt_taskSendSerial);
    PT_INIT(&pt_taskMagneto);
    PT_INIT(&pt_taskRain);
    PT_INIT(&pt_taskGyro);
}

void checkservo()
{
    a = str.substring(0,str.indexOf("/r")).toInt();
    if(a == 2)    
    {    
        myservo1.write(180);
        delay(5000);
    }
    else   
    {    
        myservo1.write(90);
    }
}

void loop() {               
    checkservo();     
    taskMagneto(&pt_taskMagneto);        
    taskGyro(&pt_taskGyro);  
    taskSerialEvent(&pt_taskSerialEvent);
    taskRain(&pt_taskRain);
    taskSendSerial(&pt_taskSendSerial);      
}

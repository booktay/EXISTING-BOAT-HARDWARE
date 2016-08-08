#include <pt.h>
#include <Servo.h>
#define PT_DELAY(pt, ms, ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

//servo
Servo servo;

//beep
#define beepPin 11

//Ultrasonic
#define trigPin 10
#define echoPin 9
long duration = 0, distance = 0, sendV = 0;
String str = "";
int a = 0, b = 0;

//Motor
#define motor 6
#define mSpeed 255

//struct
struct pt pt_taskSerialEvent;
struct pt pt_taskSendSerial;
struct pt pt_taskUltrasonic;
struct pt pt_taskBeep;

//led
#define led11 2
#define led12 4
#define led13 7
#define led14 8
int led = 0;

PT_THREAD(taskSerialEvent(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    if (Serial1.available()) {
      str = Serial1.readStringUntil('\r');
      str.replace("\r", "");
      str.replace("\n", "");
      Serial.println(str);
    }
    PT_YIELD(pt);
  }
  PT_END(pt);
}



PT_THREAD(taskSendSerial(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    sendV = distance;
    Serial1.println(sendV);
    PT_DELAY(pt, 500, ts);
  }
  PT_END(pt);
}

PT_THREAD(taskUltrasonic(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    fastDigitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    fastDigitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    fastDigitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) / 29.1;
    if (distance <= 10)
    {
      analogWrite(motor, mSpeed);
      if (distance == 0)
        distance = 11;
    }
    else
      analogWrite(motor, 0);
    PT_DELAY(pt, 50, ts);
  }
  PT_END(pt);
}

PT_THREAD(taskBeep(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    if (distance <= 20 && distance > 10)
    {
      analogWrite(beepPin, 255);
      PT_DELAY(pt, 200, ts);
      analogWrite(beepPin, 0);
      PT_DELAY(pt, 200, ts);
    }
    else if (distance <= 10 && distance > 5)
    {
      analogWrite(beepPin, 255);
      PT_DELAY(pt, 100, ts);
      analogWrite(beepPin, 0);
      PT_DELAY(pt, 100, ts);
    }
    else if (distance <= 5)
    {
      analogWrite(beepPin, 255);
      PT_DELAY(pt, 50, ts);
      analogWrite(beepPin, 0);
      PT_DELAY(pt, 50, ts);
    }
    PT_DELAY(pt, 50, ts);
  }
  PT_END(pt);
}

void taskLED(){
    a = str.substring(0,str.indexOf(",")).toInt();
    b = str.substring(str.indexOf(",")+1, str.length()).toInt();
    if (a == 2)
    {
      digitalWrite(led11, HIGH);
      digitalWrite(led12, HIGH);
      digitalWrite(led13, HIGH);
      digitalWrite(led14, HIGH);
      delay(5000);
    }
    else
    {
      digitalWrite(led11, LOW);
      digitalWrite(led12, LOW);
      digitalWrite(led13, LOW);
      digitalWrite(led14, LOW);
    }
    if (b == 2)
    {
      analogWrite(beepPin, 255);
      delay(5000);
      analogWrite(beepPin, 0);
    }
    else
    {
      digitalWrite(beepPin, LOW);
    }
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  pinMode(led11, OUTPUT);
  pinMode(led12, OUTPUT);
  pinMode(led13, OUTPUT);
  pinMode(led14, OUTPUT);
  pinMode(trigPin, OUTPUT_FAST);
  pinMode(echoPin, INPUT_FAST);
  pinMode(motor, OUTPUT_FAST);
  pinMode(beepPin, OUTPUT_FAST);
  PT_INIT(&pt_taskUltrasonic);
  PT_INIT(&pt_taskSerialEvent);
  PT_INIT(&pt_taskSendSerial);
  PT_INIT(&pt_taskBeep);
}

void loop() {
  taskLED();
  taskUltrasonic(&pt_taskUltrasonic);
  taskBeep(&pt_taskBeep);
  taskSerialEvent(&pt_taskSerialEvent);
  taskSendSerial(&pt_taskSendSerial);
}

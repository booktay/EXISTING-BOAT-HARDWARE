#define BLYNK_PRINT Serial // Enables Serial Monitor
#include <ServerExceed.h>
#include <SPI.h>
#include <BlynkSimpleEsp8266.h>
// Setting for Server
WiFiServer server(80); // nodeMCU server : port 80
char ssid[] = "nodeMCU only";
char password[] = "";
char host[] = "10.32.176.4";
int port = 80;
String group = "exist2";
ServerExceed mcu(ssid, password, host, port, group, &server);

// Setting for Blynk
char auth[] = "04a1786074494c62bc2c4c304eca73f8";
char blynk_host[] = "10.32.176.4";
int blynk_port = 18442;

void setup() {
  Serial.begin(115200);
  mcu.connectServer();
  Blynk.config(auth, blynk_host, blynk_port);
  BLYNK_PRINT.println("\n\n[- nodeMCU -] Connected.");
  BLYNK_PRINT.print("[- nodeMCU -] IPAddress : ");
  BLYNK_PRINT.println(WiFi.localIP());
  Serial.print("\n\n ip;");
  Serial.println(WiFi.localIP());
}

String data = "";

void loop() {
  if(Serial.available()) {
  	data = Serial.readStringUntil('\r');
    data.replace("\r","");
    data.replace("\n","");
  	Serial.flush();
  	mcu.sendDataFromBoardToServer(data);
    Serial.println(data);
  }
  mcu.sendDataFromServerToBoard();
  Blynk.run();
}

BLYNK_READ(V1) {
  Blynk.virtualWrite(V2, data);
}

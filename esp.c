#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

const char *ssid     = "Chen";
const char *password = "ChenYanYun";

Servo sizeSelector;
Servo slotSelector;

int values[6][3]  = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
String data;
int menum = 0;
int selectum = 0;

WiFiUDP ntpUDP;

String formattedTime;
int lastMinute = -1;

NTPClient timeClient(ntpUDP, "us.pool.ntp.org", -14400, 3600);

void setup(){
  sizeSelector.attach(12);
  slotSelector.attach(13);

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  if (currentMinute != lastMinute) {
    lastMinute = currentMinute;
    
    formattedTime = (currentHour < 10 ? "0" : "") + String(currentHour) + ":" +
                    (currentMinute < 10 ? "0" : "") + String(currentMinute);
  }

  if (Serial.available() > 0) {
    data = Serial2.readStringUntil('\n');
    if ("Slot" )
  }

  delay(1000);
}

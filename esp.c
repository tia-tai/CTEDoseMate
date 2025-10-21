#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

const char *ssid     = "Chen";
const char *password = "ChenYanYun";

Servo sizeSelector;
Servo slotSelector;

int values[6][3]  = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
String datam;
int selectum = 0;
int hourTo;

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
    Serial.println(formattedTime);
  }

  if (Serial.available() > 0) {
    datam = Serial2.readStringUntil('\n');

    int spaceIndex = datam.indexOf(' ');

    String firstWord = datam.substring(0, spaceIndex);
    String slotNumber = datam.substring(spaceIndex + 1);
   
    if (firstWord == "Slot:") {
      selectum = slotNumber.toInt();
    } else if (firstWord == "Size:") {
      values[selectum][0] = slotNumber.toInt();
    } else if (firstWord == "Interval:") {
      hourTo = (currentHour + slotNumber.toInt()) % 24;
      values[selectum][1] = (hourTo*60)+currentMinute;
    } else if (firstWord == "Amount:") {
      values[selectum][2] = slotNumber.toInt();
    }
  }

  // Task: Create a for loop through the array and find if the time is up and then find the servo controls to let one out. Sound the buzzer afterward.

  delay(1000);
}

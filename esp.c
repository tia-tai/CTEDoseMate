#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>
#include <string.h>

const char *ssid     = "Galaxy S24 Ultra 2CA9";
const char *password = "mrtakeyobi";

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

    char charBuffer[datam.length() + 1];
    datam.toCharArray(charBuffer, sizeof(charBuffer));

    char* token = strtok(charBuffer, " ");

   
    if (String(token) == "Slot:") {
      selectum = (int)strtok(NULL, " ");
    } else if (String(token) == "Size:") {
      values[selectum][0] = (int)strtok(NULL, " ");
    } else if (String(token) == "Interval:") {
      hourTo = (currentHour + (int)strtok(NULL, " ")) % 24;
      values[selectum][1] = (String)hourTo + ":" + (String)currentMinute;
      Serial.println(value[selectum][1]);
    } else if (String(token) == "Amount:") {
      values[selectum][2] = (int)strtok(NULL, " ");
    }
  }

  delay(1000);
}

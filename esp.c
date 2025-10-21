#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

const char *ssid     = "GOVERNMENTSPY";
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

  if (Serial2.available()) {
    Serial.println("Hi");
    datam = Serial2.readStringUntil('\n');
    Serial.println(datam);

    int spaceIndex = datam.indexOf(' ');

    String firstWord = datam.substring(0, spaceIndex);
    String slotNumber = datam.substring(spaceIndex + 1);
    Serial.println(firstWord);
    Serial.println(slotNumber);
   
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

  for (int i = 0; i <= 5; i++) {
    int pillsize;
    int interval;
    int amount;
    for (int x = 0; x <= 2; x++) {
      if(x==0) {
        pillsize = values[i][x];
      } else if(x==1) {
        interval = values[i][x];
      } else if(x==2) {
        amount = values[i][x];
      }
    }
    if (interval == (currentHour*60)+currentMinute) {
      //Do servo thing
    }
  }

  delay(1000);
}

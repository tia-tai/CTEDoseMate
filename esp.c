#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

const char *ssid     = "Chen";
const char *password = "ChenYanYun";

Servo sizeSelector;
Servo slotSelector;

int buttonVal;
const int slotDelay = 900;
const int sizeDelay = 800;

int values[6][4]  = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
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
  pinMode(14, OUTPUT);
  pinMode(15, INPUT_PULLUP);
  buttonVal = digitalRead(15);

  sizeSelector.write(90);

  while (buttonVal != LOW){
    slotSelector.write(0);
  }
  slotSelector.write(90);

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
  buttonVal = digitalRead(15);

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
 
  if (currentMinute != lastMinute) {
    lastMinute = currentMinute;
   
    formattedTime = (currentHour < 10 ? "0" : "") + String(currentHour) + ":" +
                    (currentMinute < 10 ? "0" : "") + String(currentMinute);
    Serial.println(formattedTime);
  }

  if (Serial2.available()) {
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
      values[selectum][2] = slotNumber.toInt();
    } else if (firstWord == "Amount:") {
      values[selectum][3] = slotNumber.toInt();
    }
  }

  for (int i = 0; i <= 5; i++) {
    int pillsize;
    int interval;
    int amount;
    for (int x = 0; x <= 3; x++) {
      if(x==0) {
        pillsize = values[i][x];
      } else if(x==1) {
        interval = values[i][x];
      } else if(x==3) {
        amount = values[i][x];
      }
    }
    if (interval <= (currentHour*60)+currentMinute) {
      slotSelector.write(180);
      delay(slotDelay*i);
      slotSelector.write(90);
      for (int z = 0; z <= amount-1; i++) {
        sizeSelector.write(180);
        delay(sizeDelay*pillsize);
        sizeSelector.write(90);
        delay(1000);
        sizeSelector.write(0);
        delay(1600); // figure out the numbers tmr
      }

      tone(14, 1000);
      delay(1000);
      noTone(14);
      hourTo = (currentHour + values[selectum][2]) % 24;
      values[selectum][1] = (hourTo*60)+currentMinute;
      while (buttonVal != LOW){
        slotSelector.write(0);
      }
      slotSelector.write(90);
    }
  }

  delay(1000);
}

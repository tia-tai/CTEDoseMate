#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

const char *ssid     = "Chen";
const char *password = "ChenYanYun";

Servo sizeSelector;
Servo slotSelector;
Servo pillProtector;

int buttonVal;
const int slotDelay = 900;
const int sizeDelay = 450;

int values[6][4]  = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
String datam;
int selectum = 0;
int hourTo;

WiFiUDP ntpUDP;

String formattedTime;
int lastMinute = -1;

NTPClient timeClient(ntpUDP, "us.pool.ntp.org", -14400, 3600);

void setup(){
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  sizeSelector.attach(27);
  slotSelector.attach(33);
  pillProtector.attach(32);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, INPUT_PULLUP);

  pillProtector.write(0); // Prevents Pills from Slipping Out

  delay(1000); // Prevents button from being read too early
  buttonVal = digitalRead(15);

  while (buttonVal == HIGH){ // Rotating Pill Wheel to Slot 1 (Calibration)
    buttonVal = digitalRead(15);
    slotSelector.write(45);
  }
  slotSelector.write(90);
  sizeSelector.write(90);
 
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
 
  if (currentMinute != lastMinute) { // Formating time to hh:mm
    lastMinute = currentMinute;
   
    formattedTime = (currentHour < 10 ? "0" : "") + String(currentHour) + ":" +
                    (currentMinute < 10 ? "0" : "") + String(currentMinute);
  }

  if (Serial2.available()) {
    datam = Serial2.readStringUntil('\n'); // Reading data from Arduino

    int spaceIndex = datam.indexOf(' ');

    String firstWord = datam.substring(0, spaceIndex); // Spliting the string into a prefix and the data
    String slotNumber = datam.substring(spaceIndex + 1);
   
    if (firstWord == "Slot:") {
      selectum = slotNumber.toInt();
    } else if (firstWord == "Size:") {
      values[selectum][0] = slotNumber.toInt();
    } else if (firstWord == "Interval:") {
      if (slotNumber.toInt() == 0) {
        values[selectum][1] = 0;
      } else {
        hourTo = (currentHour + slotNumber.toInt()) % 24;
        values[selectum][1] = (hourTo*60)+currentMinute;
      }
      values[selectum][2] = slotNumber.toInt();
    } else if (firstWord == "Amount:") {
      values[selectum][3] = slotNumber.toInt();
    }
  }

  for (int i = 0; i <= 5; i++) { // Loops through all 6 Slots
    int pillsize = values[i][0];
    int interval = values[i][1];
    int interva = values[i][2];
    int amount = values[i][3];

    if (interval == 0 || interva == 0) {
      // This represents a slot that hasn't been programmed yet
    } else {
      if (interval == (currentHour*60)+currentMinute) {
        slotSelector.write(100);
        delay(slotDelay*i);
        slotSelector.write(90);
        delay(500);
        pillProtector.write(180); // Opening protection
        for (int z = 0; z <= amount-1; z++) {
          sizeSelector.write(105); // Rotating Pill Size Wheel to the Correct Size
          delay(sizeDelay*pillsize);
          sizeSelector.write(90);
          digitalWrite(14, HIGH); // Turning On the Vibrating Motor
          delay(1000);
          digitalWrite(14, LOW);
          delay(1000);
          pillProtector.write(0);
          sizeSelector.write(80); // Rotating Pill Size Wheel to the drop Position
          delay(sizeDelay*pillsize);
          sizeSelector.write(90);
          delay(1000);
        }

        tone(12, 1000); // 3 Beeps
        delay(500);
        noTone(12);
        delay(500);
        tone(12, 1000);
        delay(500);
        noTone(12);
        delay(500);
        tone(12, 1000);
        delay(500);
        noTone(12);
        hourTo = (currentHour + interva) % 24; // Updating Timer for Next Pill
        values[i][1] = (hourTo*60)+currentMinute;
        while (buttonVal != LOW){ // Resetting Slot Wheel to Slot 1;
          buttonVal = digitalRead(15);
          slotSelector.write(84.5);
        }
        slotSelector.write(90);
      }
    }
  }

  delay(1000);
}

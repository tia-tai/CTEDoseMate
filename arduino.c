#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
const char* selection[7] = {"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5", "Slot 6", "Exit"};
const char* menu[4] = {"Pill Size", "Interval", "Amount", "Exit"};
int value[6][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};
int menum = 0;
int selectum = 0;
int buttonVal = 0;
bool buttonClick = false;
String process = "Home";
int increase = 0;

const int encoderPinA = 2;
const int encoderPinB = 3;
int encoderPos = 0;    
int lastEncoderPos = 0;


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(6, INPUT_PULLUP);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), checkEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), checkEncoder, CHANGE);
}

void loop() {
  buttonVal = digitalRead(6);

  if (process == "Home") {
    set_Home();
  } else if (process == "Selection"){
    set_Selection(selectum);
  } else if (process == "Setting") {
    set_Setting(selectum, menum);
  }

  if (buttonVal == LOW) {
    if (process == "Home") {
      process = "Selection";
    }
    else if (process == "Selection") {
      if (selectum == 6) {
        process = "Home";
        selectum = 0;
      } else {
        process = "Setting";
        Serial.println("Slot: " + String(selectum));
      }
    }
    else if (process == "Setting") {
      if (menu[menum] == "Exit"){
        process = "Selection";
        menum = 0;
      } else if (buttonClick == false) {
        buttonClick = true;
      } else {
        buttonClick = false;
      }
    }
    delay(500);
  }

  if (encoderPos != lastEncoderPos) {
    if (encoderPos > lastEncoderPos) {
      if (process == "Selection") {
        selectum = (selectum + 1) % 7;
        Serial.println("Slot: " + String(selectum));
      } else if (process == "Setting" && buttonClick == false) {
        menum = (menum + 1) % 4;
      } else if (process == "Setting" && buttonClick == true) {
        String category;
        if (menu[menum] == "Pill Size") {
          if (value[selectum][menum] < 5) {
            increase = 1;
            category = "Size: ";
          }
        } else if (menu[menum] == "Interval") {
          if (value[selectum][menum] < 48) {
            increase = 1;
            category = "Interval: ";
          }
        } else if (menu[menum] = "Amount") {
          if (value[selectum][menum] < 10) {
            increase = 1;
            category = "Amount: ";
          }
        }
        value[selectum][menum] = value[selectum][menum] + increase;
        increase = 0;
        Serial.println(category + String(value[selectum][menum]));
      }
    } else {
      if (process == "Selection") {
        selectum = (selectum + 6) % 7;
        Serial.println("Slot: " + String(selectum));
      } else if (process == "Setting" && buttonClick == false) {
        menum = (menum + 3) % 4;
      } else if (process == "Setting" && buttonClick == true) {
        value[selectum][menum] = (value[selectum][menum] > 0) ? value[selectum][menum] - 1 : 0;
        if (menum == 0){
          Serial.println("Size: " + String(value[selectum][menum]));
        } else {
          Serial.println(String(menu[menum]) + ": " + String(value[selectum][menum]));
        }
      }
    }
    
    lastEncoderPos = encoderPos;
    lcd.clear();
  }

  if (buttonVal == LOW) {
    lcd.clear();
  }

}

void set_Home() {
  lcd.setCursor(0, 0);
  lcd.print("Welcome To DoseMate");
  lcd.setCursor(0, 1);
  lcd.print("Tap for Menu");
}
void set_Selection(int selectum2) {
  lcd.setCursor(0, 0);
  lcd.print(selection[selectum2]);
}
void set_Setting(int selectum2, int menum) {
  if (menum == 3) {
    lcd.setCursor(0, 0);
    lcd.print(menu[menum]);
  } else {
    lcd.setCursor(0, 0);
    lcd.print(menu[menum]);
    lcd.setCursor(0, 1);
    lcd.print(value[selectum2][menum]);
    if (menu[menum] == "Interval") {
      lcd.setCursor(11, 1);
      lcd.print("HOURS");
    }
  }
}

void checkEncoder() {
  int a = digitalRead(encoderPinA);
  int b = digitalRead(encoderPinB);

  static int lastA = LOW;
  static int lastB = LOW;
  
  if (a != lastA || b != lastB) {
    if (lastA == LOW && a == HIGH) {
      if (b == LOW) {
        encoderPos++;
      } else {
        encoderPos--;
      }
    }
    
    lastA = a;
    lastB = b;
  }
}

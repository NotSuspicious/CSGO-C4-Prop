#include <LiquidCrystal.h>
void(* resetFunc) (void) = 0;

//did the bomb explode or get defused?
bool bombExploded = false;
bool bombDefused = false;

//pin of the piezo
const int piezo = 13;

//pin of the potentiometer
const int pot = 0;
int potVal = 0;
int ledVal = 0;

//vars for timer
int min = 0;
int sec = 40;
unsigned long oneSecond = 1000UL;
bool timerStart = false;
unsigned long startTime;
bool timeStart = false;

//numpad vars
bool isPressed = false;
int state[12] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; // the current state of the output pin      // the current reading from the input pin
char* key[12] = {"*", "7", "4", "1", "0", "8", "5", "2", "#", "9", "6", "3"};
int clickCounter = 0;

int previous = HIGH;    // the previous reading from the input pin

long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers



//password funtion
const int passLength = 7;
char* password[passLength] = {"*", "*", "*", "*", "*", "*", "*"};
char* enteredPass[passLength] = {"*", "*", "*", "*", "*", "*", "*"};
bool passwordEntered = false;
int passPos = 0;
int passPos2 = 0;
int cursor = 10;

//one time vars
bool justEnt = false;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  Serial.begin(9600);
  sec = sec + 60 * min;
  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  pinMode(31, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  lcd.print("*******");
}

void loop() {

  while (!bombExploded && !bombDefused && clickCounter <= 13) {
    numpad();
    if (passPos > 6 && !justEnt) {
      passwordEntered = true;
      justEnt = true;
      lcd.setCursor(4, 0);
      lcd.print("*******");
      cursor = 10;
      startTime = millis();
      lcd.setCursor(5, 1);
      lcd.print("00:40");
    }//one time
  if (passwordEntered) {
    timer();
  }
}
if (bombDefused) {
  
  lcd.setCursor(0, 0);
  lcd.print("  Bomb Defused!");
  lcd.setCursor(4, 1);
  lcd.print("CTs Win!");
  numpad();
  return;
}
if (bombExploded || clickCounter >= 13) {
  numpad();

  lcd.setCursor(0, 0);
  lcd.print(" Bomb Exploded!");
  lcd.setCursor(0, 1);
  lcd.print(" Terrorists Win!");
}
}



void numpad() {
  int star = digitalRead(22);  // keypad pin 2
  int seven = digitalRead(23);  // keypad pin 3
  int four = digitalRead(24);  // keypad pin 4
  int one = digitalRead(25);  // keypad pin 5
  int zero = digitalRead(26);  // keypad pin 6
  int eight = digitalRead(27);  // keypad pin 7
  int five = digitalRead(28);  // keypad pin 8
  int two = digitalRead(29);  // keypad pin 9
  int pound = digitalRead(30);  // keypad pin 10
  int nine = digitalRead(31);  // keypad pin 11
  int six = digitalRead(32);  // keypad pin 12
  int three = digitalRead(33);  // keypad pin 13
  int keyVar[12] = {star, seven, four, one, zero, eight, five, two, pound, nine, six, three};

  for (int a = 0 ; a < 12 ; a++) {
    if (state[a] == HIGH && !keyVar[a]) {
      if (!passwordEntered) {
        setPass(a);
        keyPressed();

      } else {
        enterPass(a);
        keyPressed();
      }
      if (bombDefused && key[a] == "*" || bombExploded && key[a] == "*") {
        resetFunc();
      }
      state[a] = LOW;
    }
    if (keyVar[a]) {
      state[a] = HIGH;
    }
  }
}

void keyPressed() {
  if (!passwordEntered) {

    // Cursor should start at 4 and end at 11

    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("*******");
    lcd.setCursor(cursor, 0);
    cursor--;
    for (int j = 0 ; j < passLength; j++) {
      if (cursor + j < 10) {
        lcd.print(password[j]);
      }
    }
  }
  if (passwordEntered) {
    lcd.setCursor(cursor, 0);
    cursor--;
    for (int j = 0 ; j < passLength; j++) {
      if (cursor + j < 10) {
        lcd.print(enteredPass[j]);
      }
    }
  }
}

void enterPass(int posi2) {
  bool isRight = false;
  while (!isRight) {
    if (key[posi2] != "*" && key[posi2] != "#") {
      enteredPass[passPos2] = key[posi2];
      clickCounter++;
      passPos2++;
    }
    if (key[posi2] == "*") {
      resetFunc();
    }
    for (int i = 0 ; i < passLength ; i++) {
      if (password[i] == enteredPass[i]) {
        isRight = true;
      } else {
        isRight = false;
        return;
      }
    }
  }
  if (isRight) {
    bombDefused = true;
    lcd.clear();
    lcd.setCursor(1, 1);
    return;
  }
}

void setPass(int posi) {
  if (key[posi] != "*" && key[posi] != "#") {
    password[passPos] = key[posi];
    clickCounter++;
    passPos++;
  }
  if (key[posi] == "*") {
    resetFunc();
  }
}

void timer() {
  if (millis() - startTime >= oneSecond) {
    tone(piezo, 1000);
    lcd.setCursor(5, 1);
    sec--;
    startTime += oneSecond;
    if (sec < 0 && !bombDefused) {
      lcd.clear();
      bombExploded = true;
      return;
    }
    int displayMin = sec / 60;
    if (displayMin < 10) {
      lcd.print("0");
    }
    lcd.print(displayMin);
    lcd.print(":");
    int displaySec = sec % 60;
    if (displaySec < 10) {
      lcd.print("0");
    }
    lcd.print(sec % 60);
  }
}

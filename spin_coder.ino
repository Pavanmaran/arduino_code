#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 4;

// LCD PINS
const int rs = 14, en = 15, d4 = 16, d5 = 17, d6 = 18, d7 = 19;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const int enable = 10;
const int in1 = 12;
const int in2 = 11;
int speed = 0;
int p1_t, p2_t, p3_t, p1_s, p2_s, p3_s;


bool motorRunning = false;
void setup() {
  Serial.begin(9600);
  pinMode(enable, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  //LCD
  lcd.begin(16, 2);
  lcd.print("Hello Pawan! ");

  delay(2000);
}
void bpressedFunction() {
  motorRunning = true;
  if (p1_t != 0 && p1_s != 0) {\
    lcd.clear();
    lcd.print("Running again");
    lcd.setCursor(0, 1);
    lcd.print(p1_t);
    runmotor(p1_t, p1_s, p2_t, p2_s, p3_t, p3_s);
    motorRunning = false;
    return;
  } else {
    lcd.clear();
    lcd.print("Vars are Null");
  }
}
void loop() {
  if (!motorRunning) {
    // Acceleration phase input
    lcd.clear();
    lcd.print("Acc Time: ");
    p1_t = getNumber();
    lcd.print(p1_t);

    lcd.setCursor(0, 1);
    lcd.print("Acc Speed: ");
    p1_s = getNumber();
    lcd.print(p1_s);

    delay(1000);

    // Constant speed phase input
    lcd.clear();
    lcd.print("Cons Time: ");
    p2_t = getNumber();
    lcd.print(p2_t);

    lcd.setCursor(0, 1);
    lcd.print("Cons Speed: ");
    p2_s = getNumber();
    lcd.print(p2_s);

    delay(1000);

    // Deceleration phase input
    lcd.clear();
    lcd.print("Dec Time: ");
    p3_t = getNumber();
    lcd.print(p3_t);

    lcd.setCursor(0, 1);
    lcd.print("Dec Speed: ");
    p3_s = getNumber();
    lcd.print(p3_s);


    delay(1000);
    motorRunning = true;
  }
  runmotor(p1_t, p1_s, p2_t, p2_s, p3_t, p3_s);
}
void runmotor(int xp1_t, int xp1_s, int xp2_t, int xp2_s, int xp3_t, int xp3_s) {
  if (motorRunning) {
    // Acceleration phase
    uint32_t period = xp1_t * 1000;
    front();
    int t = 0;
    uint32_t tStart = millis();
    while ((millis() - tStart) < period) {
      speedUp(xp1_s);
      updateLCD("Acc", t, speed);
      t++;
      delay(1000);
    }

    // Constant speed phase
    period = xp2_t * 1000;
    speedConst(xp2_s);
    int t2 = xp2_t;
    tStart = millis();
    while ((millis() - tStart) < period) {
      if (keypad.getKey() == 'A') {
        stop();
        motorRunning = false;
        speed = 0;
        lcd.clear();
        lcd.print("Enter Time: ");
        lcd.setCursor(0, 1);
        lcd.print("Enter Speed: ");
        return;
      }
      updateLCD("Cons", t2, speed);
      t2--;
      delay(1000);
    }

    // Deceleration phase
    period = xp3_t * 1000;
    t = xp3_t - 1;
    tStart = millis();
    while (xp3_s > 0) {
      speedDown(xp3_s);
      if (speed < 0) {
        stop();
        break;
      }
      updateLCD("Dec", t, speed);
      t--;
      delay(1000);
    }

    motorRunning = false;
  }
}
void updateLCD(const char* phase, int time, int currentSpeed) {
  lcd.clear();
  lcd.print("Time: ");
  lcd.setCursor(7, 0);
  lcd.print(time);
  lcd.setCursor(0, 1);
  lcd.print("Speed: ");
  lcd.print(currentSpeed);
  lcd.print(" ");
  lcd.print(phase);
}

void back() {
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
  analogWrite(enable, speed);
}
void front() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enable, speed);
}
void speedUp(int val) {
  speed += val;
  analogWrite(enable, speed);
}
void speedConst(int val) {
  speed = val;
  analogWrite(enable, speed);
}
void speedDown(int val) {
  speed -= val;
  analogWrite(enable, speed);
  //if(1)
  //stop();
  //if(p3<1)
  //stop();
}
void stop() {
  lcd.clear();
  lcd.print("Stopped, Press B");
  lcd.setCursor(0, 1);
  lcd.print("to run again");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(enable, LOW);
}

int getNumber() {
  int num = 0;
  char key = keypad.getKey();
  while (key != '#') {
    switch (key) {
      case '#':
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        //lcd.print(key);
        num = num * 10 + (key - '0');
        break;

      case '*':
        num = 0;
        //lcd.clear();
        break;
      case 'A':
        stop();
      case 'B':
        bpressedFunction();
    }

    key = keypad.getKey();
  }

  return num;
}

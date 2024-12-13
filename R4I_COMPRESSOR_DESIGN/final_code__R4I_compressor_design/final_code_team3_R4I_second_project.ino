#include <Wire.h>
#include <LiquidCrystal_I2C.h>

volatile bool btn_one_state = false;
volatile bool btn_two_state = false;
bool clockwise_button = false;
bool anti_clockwise_button = false;
unsigned long previous_millis_clockwise = 0;
unsigned long previous_millis_anti_clockwise = 0;
unsigned long previous_millis_direction_control = 0;
unsigned long previous_millis_current_monitor = 0;
int debounce_interval = 10;

int count = 0;

// Pin Definitions
const int currentSensorPin = A0;  // Analog pin connected to ACS712


#define btn_one 2
#define btn_two 3
#define CURRENT_SENSE A0
#define RPWM 5
#define LPWM 6
#define R_EN 7
#define L_EN 8

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void button_control();
void motor_setup();
void motor_disable();
void motor_enable();
void direction_control(int refresh_rate = 500);
float current_monitor(int refresh_rate = 100);



byte block[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};


byte down_left[8] = {
  0b11111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000,
  0b00000,
  0b00000
};


byte down_right[8] = {
  0b11111,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000,
  0b00000,
  0b00000
};

byte down_tip[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100
};

byte up_tip[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte up_left[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b11111
};

byte up_right[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11111
};
byte empty[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

/*
byte [8] =
{
0b00000,
0b10001,
0b11011,
0b11111,
0b11111,
0b11111,
0b11111,
0b11111
};
byte upward_arrow[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

byte downward_arrow[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100
};
*/

void button_one() {

  unsigned long current_millis = millis();
  if ((current_millis - previous_millis_clockwise) > debounce_interval) {
    previous_millis_clockwise = current_millis;


    btn_one_state = true;
  }
}


void button_two() {

  unsigned long current_millis = millis();
  if ((current_millis - previous_millis_anti_clockwise) > debounce_interval) {
    previous_millis_anti_clockwise = current_millis;


    btn_two_state = true;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  ///////////////////////////////////////////////////////////
  pinMode(btn_one, INPUT);
  pinMode(btn_two, INPUT);

  motor_setup();
  motor_enable();


  attachInterrupt(digitalPinToInterrupt(btn_one), button_one, CHANGE);
  attachInterrupt(digitalPinToInterrupt(btn_two), button_two, CHANGE);

  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.blink();
  lcd.setCursor(4, 1);
  lcd.print("S-COMPACTIFY");
  lcd.setCursor(0, 3);
  lcd.print("Powered By Innov8Hub");

  lcd.createChar(0, up_left);
  lcd.createChar(1, up_right);
  lcd.createChar(2, up_tip);
  lcd.createChar(3, down_left);
  lcd.createChar(4, down_right);
  lcd.createChar(5, down_tip);
  lcd.createChar(6, block);
  lcd.createChar(7, empty);
  lcd.clear();
  ////////////////////////////////////////////////////////
}

void loop() {

  // Variable to store the sum of ADC readings


  direction_control();
  button_control();
  // delay(1000);
  count++;
  if(!anti_clockwise_button && !clockwise_button) count = 0;
  Serial.println(count);
}

//////////////////////////////////////////////////////////////////////
void motor_setup() {

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(btn_one, INPUT);
  pinMode(btn_two, INPUT);
}

void motor_disable() {

  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);
}


void motor_enable() {

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
}

void direction_control(int refresh_rate = 500) {

  unsigned long current_millis = millis();
  if ((current_millis - previous_millis_direction_control) > refresh_rate) {

    if (clockwise_button) {

      if (count > 240 && (digitalRead(btn_one) == LOW)) {
        clockwise_button = false;
        count = 0;
      }


      motor_enable();

      lcd.setCursor(0, 0);
      lcd.print("DOWN BUTTON PRESSED");
      lcd.setCursor(10, 1);
      lcd.write(6);
      lcd.setCursor(10, 2);
      lcd.write(6);
      lcd.setCursor(10, 3);
      lcd.write(5);
      lcd.setCursor(9, 3);
      lcd.write(3);
      lcd.setCursor(11, 3);
      lcd.write(4);
      lcd.setCursor(9, 1);
      lcd.write(7);
      lcd.setCursor(11, 1);
      lcd.write(7);


      analogWrite(LPWM, 255);
      analogWrite(RPWM, 0);

      delay(20);

    } else if (anti_clockwise_button) {

      if (count > 240 && (digitalRead(btn_two) == LOW)) {
        anti_clockwise_button = false;
        count = 0;
      }

      motor_enable();

      lcd.setCursor(0, 0);
      lcd.print(" UP BUTTON PRESSED ");
      lcd.setCursor(9, 1);
      lcd.write(0);
      lcd.setCursor(11, 1);
      lcd.write(1);
      lcd.setCursor(10, 1);
      lcd.write(2);
      lcd.setCursor(10, 2);
      lcd.write(6);
      lcd.setCursor(10, 3);
      lcd.write(6);
      lcd.setCursor(9, 3);
      lcd.write(7);
      lcd.setCursor(11, 3);
      lcd.write(7);

      analogWrite(LPWM, 0);
      analogWrite(RPWM, 255);
      delay(20);

    } else {
      motor_disable();
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" NO BUTTON PRESSED ");
      lcd.setCursor(0, 3);
      analogWrite(LPWM, 0);
      analogWrite(RPWM, 0);
      lcd.setCursor(9, 3);
      lcd.write(7);
      lcd.setCursor(10, 3);
      lcd.write(7);
      lcd.setCursor(11, 3);
      lcd.write(7);
      lcd.setCursor(10, 2);
      lcd.write(7);
      lcd.setCursor(10, 1);
      lcd.write(7);
      lcd.setCursor(9, 1);
      lcd.write(7);
      lcd.setCursor(11, 1);
      lcd.write(7);
    }
  }
}

void button_control() {

  if (btn_one_state) {
    btn_one_state = false;
    clockwise_button = true;
    anti_clockwise_button = false;

  } else if (btn_two_state) {
    btn_two_state = false;
    anti_clockwise_button = true;
    clockwise_button = false;
  }
}

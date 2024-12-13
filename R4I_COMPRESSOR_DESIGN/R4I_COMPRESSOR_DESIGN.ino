
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

volatile bool btn_one_state = false;
volatile bool btn_two_state = false;
bool clockwise_button = false;
bool anti_clockwise_button = false;
unsigned long previous_millis_clockwise = 0;
unsigned long previous_millis_anti_clockwise = 0;
unsigned long previous_millis_direction_control= 0;
unsigned long previous_millis_current_monitor = 0;
int debounce_interval = 10;

const float sensorOffset = 2.5;      // Offset voltage at 0A (in volts, typical for ACS712)
const float sensitivity = 0.066;    // Sensitivity in V/A for ACS712 30A version
const float referenceVoltage = 4.55 ; // Reference voltage for the ADC
const int adcResolution = 1023;     // 10-bit ADC
const int numSamples = 100;         // Number of samples for averaging




int count = 0;

#define btn_one 2
#define btn_two 3
#define CURRENT_SENSE A0
#define RPWM 5
#define LPWM 6
#define R_EN  7
#define L_EN  8

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void button_control();
void motor_setup();
void motor_disable();
void  motor_enable();
void direction_control(float current_reading, int refresh_rate = 50);
float current_monitor( int refresh_rate = 100);

void button_one(){

  unsigned long current_millis = millis();
 if((current_millis -  previous_millis_clockwise) > debounce_interval){
   previous_millis_clockwise = current_millis;
   if (digitalRead(btn_one)){
      btn_one_state = true;

   }else{

     btn_one_state = false;
  }


  }

}


void button_two(){

  unsigned long current_millis = millis();
  if ( (current_millis -  previous_millis_anti_clockwise) > debounce_interval) {
    previous_millis_anti_clockwise = current_millis;

     if (digitalRead(btn_two)){
       btn_two_state = true;

    }else{
     btn_two_state = false;
    }

  
  } 

}

void setup() {
   Serial.begin(9600);

  pinMode(CURRENT_SENSE, INPUT);
  pinMode(btn_one, INPUT);
  pinMode(btn_two, INPUT);
  
  motor_setup();
  motor_enable();

 
  attachInterrupt(digitalPinToInterrupt(btn_one), button_one, CHANGE);
  attachInterrupt(digitalPinToInterrupt( btn_two), button_two, CHANGE);

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.blink();
  lcd.setCursor(4,1);
  lcd.print("S-COMPACTIFY");
  lcd.setCursor(0,3);
  lcd.print("Powered By Innov8Hub");
}



void loop(){

  float current_reading = current_monitor();
  Serial.print("Current_reading: ");
  Serial.println(current_reading);
 
  
   direction_control( current_reading);

  
 

   button_control();
 


}
  

void motor_setup(){
  
 pinMode(RPWM, OUTPUT);   
 pinMode(LPWM, OUTPUT); 
 pinMode( R_EN, OUTPUT);   
 pinMode(L_EN , OUTPUT);
 pinMode(btn_one, INPUT);
pinMode(btn_two, INPUT);

}

void motor_disable(){
  
digitalWrite(R_EN, LOW) ;
digitalWrite(L_EN, LOW) ;

}


void motor_enable(){
  
digitalWrite(R_EN, HIGH) ;
digitalWrite(L_EN, HIGH) ;

}

void direction_control(float current_reading, int refresh_rate = 50){

 unsigned long current_millis = millis();
  if ((current_millis - previous_millis_direction_control) >  refresh_rate){

    if (clockwise_button) {
    clockwise_button = false;
    motor_enable();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Button one pressed");
    lcd.setCursor(0,3);
    lcd.print("Current: ");
    lcd.print(String(current_reading));
    analogWrite(LPWM, 255);
    analogWrite(RPWM, 0);


    delay(20);
    
    } else if (anti_clockwise_button) {

   
    anti_clockwise_button = false;
    
    motor_enable();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Button two pressed");
    lcd.setCursor(0,3);
    lcd.print("Current: ");
    lcd.print(String(current_reading));
    analogWrite(LPWM, 0);
    analogWrite(RPWM, 255);
    delay(20);

    } else {
     motor_disable();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("No button pressed");
      lcd.setCursor(0,3);
      lcd.print("Current: ");
      lcd.print(String(current_reading, 1));
     analogWrite(LPWM, 0);
     analogWrite(RPWM, 0); 
   }
 }

}

void button_control(){

 if(btn_one_state ){
  clockwise_button = true;

 } else if (btn_two_state){
  anti_clockwise_button = true;
 }
  
}


float current_monitor(int refresh_rate = 100 ){ // this is a cumbersome way of doing this. I could easily remove the array. However, I'm testing my knowledge of C. 
  unsigned long current_millis = millis();
   
  if ((current_millis - previous_millis_current_monitor) > refresh_rate){

    previous_millis_current_monitor = current_millis;

    long adcSum = 0;

  for (int i = 0; i < numSamples; i++) {
    adcSum += analogRead( CURRENT_SENSE);
    delay(1); // Small delay to reduce noise
  }
  float adcAverage = adcSum / float(numSamples);

  // Convert ADC value to voltage
  float voltage = (adcAverage * referenceVoltage) / adcResolution;

  // Calculate current
  float current = (voltage - sensorOffset) / sensitivity;
  

   return    current + 0.6;

  }
  
}









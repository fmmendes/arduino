// NewPing - Version: Latest
#include <NewPing.h>

// LiquidCrystal I2C - Version: Latest
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Define pin connections and constants
const unsigned long BAUD = 9600;

// Sonar constants
const int TRIG_PIN = 5;
const int ECHO_PIN = 6;
const int MAX_DISTANCE = 400; // Maximum distance to measure (in cm)

// Servo constants
const int SERVO_PIN = 3;
const int SERVO_DELAY_MS = 30;

// LCD constants
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Variables for the duration and the distance
long duration;
int distance;
int servoPosition ;

Servo servo;
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // Create a NewPing object

void setup() {
  Serial.begin(BAUD);
  servo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servoPosition  = servo.read();

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the LCD backlight
  lcd.home();       // Set the cursor to the beginning of the first line
  lcd.print("Distance:");
  lcd.setCursor(0, 1);
  lcd.print("Angle:");
}

void loop() {
  for (servoPosition ; servoPosition  <= 180; servoPosition ++) {
    servo.write(servoPosition );
    delay(30);
    distance = calculateDistance();
    display(distance);
    printToSerial(distance);
  }

  for (servoPosition  = 179; servoPosition  >= 0; servoPosition --) {
    servo.write(servoPosition );
    delay(30);
    distance = calculateDistance();
    display(distance);
    printToSerial(distance);
  }
}

void printToSerial(int distance) {
  Serial.print(servoPosition);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(".");
}

int calculateDistance() {
  int distance = sonar.ping_cm(); // Send a ping and get the distance in centimeters
  return distance;
}

void printNumberWithLeadingSpaces(int number) {
  if (number < 10) {
    lcd.print("  ");
  } else if (number < 100) {
    lcd.print(" ");
  }
  lcd.print(number);
}

void display(int distance) {
  lcd.setCursor(13, 0);
  printNumberWithLeadingSpaces(distance);

  lcd.setCursor(13, 1);
  printNumberWithLeadingSpaces(servoPosition);
}
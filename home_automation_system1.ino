// Project: HOME AUTOMATION SYSTEM
// Developed By: Utsav Patel, Aditi Trivedi, Smit Patel, Yash Patel
// Guided By: Prof. Gurleen Kaur
// Submission On: 7th April 2024


//Hello Guys
#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

Servo myservo;

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

const int gas_sensor = A0;
const int temperature_sensor = A4;
const int ultrasonic_trigger_pin = 11;
const int ultrasonic_echo_pin = 12;
const int pir_sensor = 13; // Adjust pin according to your setup

#define Password_Length 5

int pos = 0;
int fanPin = 10;
int fanThresholdOn = 700;
int fanThresholdOff = 500;
bool fanState = false;
bool fanOn = false;

char Data[Password_Length];
char Master[Password_Length] = "1234";
byte data_count = 0;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
bool door = true;
bool passwordEntered = false;

byte rowPins[ROWS] = {1, 2, 3, 4};
byte colPins[COLS] = {5, 6, 7, 8};

Keypad customKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum State {
  ENTER_PASSWORD,
  CHECK_PASSWORD,
  CHECK_GAS_SENSOR,
  CHECK_TEMPERATURE_SENSOR,
  CHECK_ULTRASONIC_SENSOR,
  OPEN_DOOR,
  CLOSE_DOOR
};

State currentState = ENTER_PASSWORD;

void setup() {
  pinMode(fanPin, OUTPUT);
  pinMode(gas_sensor, INPUT);
  pinMode(temperature_sensor, INPUT);
  pinMode(pir_sensor, INPUT);
  pinMode(ultrasonic_trigger_pin, OUTPUT);
  pinMode(ultrasonic_echo_pin, OUTPUT);
  myservo.attach(9);
  ServoClose();
  lcd.begin(16, 2);
  lcd.print("Arduino Door");
  lcd.setCursor(0, 1);
  lcd.print("--Look project--");
  delay(3000);
  lcd.clear();
}

void loop() {
  switch (currentState) {
    case ENTER_PASSWORD:
      EnterPassword();
      break;

    case CHECK_PASSWORD:
      CheckPassword();
      break;

    case CHECK_GAS_SENSOR:
      CheckGasSensor();
      break;

    case CHECK_TEMPERATURE_SENSOR:
      CheckTemperatureSensor();
      break;

    case CHECK_ULTRASONIC_SENSOR:
      CheckUltrasonicSensor();
      break;

    case OPEN_DOOR:
      OpenDoor();
      break;

    case CLOSE_DOOR:
      CloseDoor();
      break;
  }
}

void EnterPassword() {
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");

  char customKey = customKeypad.getKey();
  if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  if (data_count == Password_Length - 1) {
    if (!strcmp(Data, Master)) {
      lcd.clear();
      digitalWrite(10, HIGH);
      ServoOpen();
      door = true;
      passwordEntered = true;
      delay(1000);
      digitalWrite(10, LOW);
      currentState = CHECK_GAS_SENSOR;
    } else {
      lcd.clear();
      lcd.print("Wrong Password");
      delay(1000);
      clearData();
      lcd.clear();
    }
  }
}

void CheckPassword() {
  currentState = CHECK_GAS_SENSOR; // Placeholder, no password check implemented
}

void CheckGasSensor() {
  int gasLevel = analogRead(gas_sensor);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas level: ");
  lcd.print(gasLevel);
  delay(3000);
  if (gasLevel > 600) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gas leak");
    lcd.setCursor(0, 1);
    lcd.println("Fan ON");
    ServoOpen();
    delay(3000);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Gas level normal");
    lcd.setCursor(0, 1);
    lcd.println("Fan OFF");
    ServoClose(); 
    delay(3000);
  }

  currentState = CHECK_TEMPERATURE_SENSOR;
}

void CheckTemperatureSensor() {
  float temperature = readTemperature(temperature_sensor);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print(temperature);
  lcd.print("C");
  delay(3000);

  currentState = CHECK_ULTRASONIC_SENSOR;
}

void CheckUltrasonicSensor() {
  int distance = 0.01723 * readUltrasonicDistance(ultrasonic_trigger_pin, ultrasonic_echo_pin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");
  delay(3000);

  if (distance < 100) {
    currentState = OPEN_DOOR;
  } 
  else {
    currentState = CLOSE_DOOR;
  }
}

void OpenDoor() {
  lcd.clear();
  lcd.print("Door Open");
  ServoOpen();
  delay(3000);
  currentState = CHECK_TEMPERATURE_SENSOR;
}

void CloseDoor() {
  lcd.clear();
  lcd.print("Door Close");
  ServoClose();
  delay(3000);
  currentState = CHECK_GAS_SENSOR;
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
}

float readTemperature(int sensorPin) {
  int sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (5.0 / 1024.0);
  float temperature = voltage * 100.0; // Adjust calibration as needed
  return temperature;
}

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);    
}

void ServoOpen() {
  for (pos = 0; pos <= 180; pos += 5) {
    myservo.write(pos);
    delay(15);
  }
}

void ServoClose() {
  for (pos = 180; pos >= 0; pos -= 5) {
    myservo.write(pos);
    delay(15);
  }
}

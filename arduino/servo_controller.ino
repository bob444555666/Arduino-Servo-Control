#include <Servo.h>

// Continuous-rotation servo signal wire connects to Mega pin 33.
const byte SERVO_PIN = 33;
const int STOP_US = 1500;

// Many continuous-rotation servos use approximately 1000-2000 us.
// Adjust these endpoints if your servo needs calibration.
const int CW_MAX_US = 2000;
const int CCW_MAX_US = 1000;

Servo motor;
int speedPercent = 70;
bool running = false;
bool clockwise = true;

void applyMotor() {
  if (!running || speedPercent == 0) {
    motor.writeMicroseconds(STOP_US);
    return;
  }

  const int endpoint = clockwise ? CW_MAX_US : CCW_MAX_US;
  const int pulse = STOP_US + (long)(endpoint - STOP_US) * speedPercent / 100;
  motor.writeMicroseconds(pulse);
}

void setup() {
  Serial.begin(9600);
  motor.attach(SERVO_PIN);
  motor.writeMicroseconds(STOP_US);
  Serial.println(F("Servo controller ready. Commands: CW, CCW, STOP, START, SPEED:0-100"));
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    if (command == "STOP") {
      running = false;
      applyMotor();
      Serial.println(F("Stopped"));
    } else if (command == "CW") {
      clockwise = true;
      running = true;
      applyMotor();
      Serial.println(F("Clockwise"));
    } else if (command == "CCW") {
      clockwise = false;
      running = true;
      applyMotor();
      Serial.println(F("Reverse"));
    } else if (command == "START") {
      running = true;
      applyMotor();
      Serial.println(F("Started"));
    } else if (command.startsWith("SPEED:")) {
      speedPercent = constrain(command.substring(6).toInt(), 0, 100);
      applyMotor();
      Serial.print(F("Speed set to "));
      Serial.println(speedPercent);
    }
  }
}
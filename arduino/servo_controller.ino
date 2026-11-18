#include <Servo.h>

// Standard positional servo signal wire connects to Arduino Mega digital pin 33.
const byte SERVO_PIN = 33;
const int START_ANGLE = 90;
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

Servo servoMotor;
int currentAngle = START_ANGLE;
int targetAngle = START_ANGLE;
int speedDegPerSecond = 60;
bool sweeping = false;
int sweepDirection = 1;
unsigned long lastStepTime = 0;
float movementRemainder = 0.0f;

void setTarget(int angle) {
  targetAngle = constrain(angle, MIN_ANGLE, MAX_ANGLE);
  sweeping = false;
}

void processCommand(String command) {
  command.trim();
  command.toUpperCase();
  if (command == "STOP") {
    sweeping = false;
    targetAngle = currentAngle; // Keep holding the current position.
    movementRemainder = 0;
    Serial.println(F("Stopped; holding current angle"));
  } else if (command == "SWEEP:START") {
    sweeping = true;
    if (currentAngle >= MAX_ANGLE) sweepDirection = -1;
    else if (currentAngle <= MIN_ANGLE) sweepDirection = 1;
    targetAngle = sweepDirection > 0 ? MAX_ANGLE : MIN_ANGLE;
    Serial.println(F("Sweep started"));
  } else if (command == "SWEEP:STOP") {
    sweeping = false;
    targetAngle = currentAngle;
    movementRemainder = 0;
    Serial.println(F("Sweep stopped"));
  } else if (command.startsWith("ANGLE:")) {
    setTarget(command.substring(6).toInt());
    Serial.print(F("Target angle: "));
    Serial.println(targetAngle);
  } else if (command.startsWith("SPEED:")) {
    speedDegPerSecond = constrain(command.substring(6).toInt(), 1, 180);
    Serial.print(F("Speed: "));
    Serial.println(speedDegPerSecond);
  }
}

void setup() {
  Serial.begin(9600);
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(currentAngle);
  lastStepTime = millis();
  Serial.println(F("Positional servo ready. Commands: ANGLE:0-180, SPEED:1-180, SWEEP:START, SWEEP:STOP, STOP"));
}

void loop() {
  while (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }
  const unsigned long now = millis();
  const unsigned long elapsed = now - lastStepTime;
  lastStepTime = now;
  if (currentAngle != targetAngle) {
    movementRemainder += (float)speedDegPerSecond * elapsed / 1000.0f;
    const int steps = (int)movementRemainder;
    if (steps > 0) {
      movementRemainder -= steps;
      if (targetAngle > currentAngle) currentAngle = min(currentAngle + steps, targetAngle);
      else currentAngle = max(currentAngle - steps, targetAngle);
      servoMotor.write(currentAngle);
    }
  } else {
    movementRemainder = 0;
    if (sweeping) {
      sweepDirection = -sweepDirection;
      targetAngle = sweepDirection > 0 ? MAX_ANGLE : MIN_ANGLE;
    }
  }
}
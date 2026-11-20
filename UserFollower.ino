/*
 * Dedicated User Follower Robot Car (Pure Human/Object Following)
 * Hardware:
 * - Arduino Uno / Nano
 * - L298N Motor Driver
 * - 1x HC-SR04 Ultrasonic Sensor (TRIG = D11, ECHO = D12)
 * - 2x IR Sensors (Left = D2, Right = D3)
 */

// --- Motor Pins (L298N) ---
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

// --- IR Sensor Pins ---
const int LEFT_SENSOR = 2;
const int RIGHT_SENSOR = 3;

// --- Ultrasonic Sensor Pins ---
const int TRIG_PIN = 11;
const int ECHO_PIN = 12;

// --- Speeds ---
const int BASE_SPEED = 140;
const int TURN_SPEED = 150;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);
}

int getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void loop() {
  int distance = getDistanceCM();
  int leftIR = digitalRead(LEFT_SENSOR);
  int rightIR = digitalRead(RIGHT_SENSOR);

  // When user is between 12cm and 35cm away
  if (distance >= 12 && distance <= 35) {
    if (leftIR == HIGH && rightIR == LOW) {
      turnLeft();   // User moved left
    } else if (leftIR == LOW && rightIR == HIGH) {
      turnRight();  // User moved right
    } else {
      moveForward(); // User in center
    }
  } 
  // Too close: stop
  else if (distance > 0 && distance < 12) {
    stopMotors();
  } 
  // Too far / lost target: stop
  else {
    stopMotors();
  }

  delay(25);
}

void moveForward() {
  analogWrite(ENA, BASE_SPEED);
  analogWrite(ENB, BASE_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

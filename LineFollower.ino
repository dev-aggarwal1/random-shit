// ==========================================
// Dual-Mode Arduino Robot Car
// 1. Line Follower Mode
// 2. User / Object Follower Mode
// Hardware: Arduino Uno/Nano, L298N, 2x IR, HC-SR04
// ==========================================

// --- Motor Driver Pins (L298N) ---
const int ENA = 5;   // Left motor speed (PWM)
const int IN1 = 6;   // Left motor forward
const int IN2 = 7;   // Left motor backward
const int IN3 = 8;   // Right motor forward
const int IN4 = 9;   // Right motor backward
const int ENB = 10;  // Right motor speed (PWM)

// --- IR Sensor Pins ---
const int LEFT_SENSOR = 2;
const int RIGHT_SENSOR = 3;

// --- Ultrasonic Sensor Pins (HC-SR04) ---
const int TRIG_PIN = 11;
const int ECHO_PIN = 12;

// --- Mode Toggle Button ---
const int MODE_BUTTON_PIN = 4; // Connect button between Pin 4 and GND

// --- Speed Settings (0 - 255) ---
const int BASE_SPEED = 140;
const int TURN_SPEED = 150;

// Sensor Detection Logic
const int DETECT_LINE = HIGH; 
const int DETECT_WHITE = LOW;

// --- Modes ---
enum RobotMode {
  MODE_LINE_FOLLOWER = 0,
  MODE_USER_FOLLOWER = 1
};

// Default starting mode
RobotMode currentMode = MODE_LINE_FOLLOWER;

// Function declarations
void runLineFollower();
void runUserFollower();
int getDistanceCM();
void checkModeButton();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopMotors();

void setup() {
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // IR Sensor pins
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  // Ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Button pin (internal pullup)
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Robot Car Initialized. Default: Line Follower");
}

void loop() {
  checkModeButton();

  if (currentMode == MODE_LINE_FOLLOWER) {
    runLineFollower();
  } else {
    runUserFollower();
  }
}

// ==========================================
//           MODE 1: LINE FOLLOWER
// ==========================================
void runLineFollower() {
  int leftReading = digitalRead(LEFT_SENSOR);
  int rightReading = digitalRead(RIGHT_SENSOR);

  if (leftReading == DETECT_WHITE && rightReading == DETECT_WHITE) {
    moveForward();
  }
  else if (leftReading == DETECT_LINE && rightReading == DETECT_WHITE) {
    turnLeft();
  }
  else if (leftReading == DETECT_WHITE && rightReading == DETECT_LINE) {
    turnRight();
  }
  else if (leftReading == DETECT_LINE && rightReading == DETECT_LINE) {
    stopMotors();
  }
}

// ==========================================
//        MODE 2: USER / OBJECT FOLLOWER
// ==========================================
void runUserFollower() {
  int distance = getDistanceCM();
  int leftIR = digitalRead(LEFT_SENSOR);
  int rightIR = digitalRead(RIGHT_SENSOR);

  if (distance >= 12 && distance <= 35) {
    if (leftIR == HIGH && rightIR == LOW) {
      turnLeft();
    }
    else if (leftIR == LOW && rightIR == HIGH) {
      turnRight();
    }
    else {
      moveForward();
    }
  } 
  else if (distance > 0 && distance < 12) {
    stopMotors();
  } 
  else {
    stopMotors();
  }

  delay(20);
}

// ==========================================
//          ULTRASONIC DISTANCE SENSOR
// ==========================================
int getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;

  int distance = duration * 0.034 / 2;
  return distance;
}

// ==========================================
//            MODE BUTTON TOGGLE
// ==========================================
void checkModeButton() {
  static unsigned long lastDebounceTime = 0;
  static int lastButtonState = HIGH;
  int reading = digitalRead(MODE_BUTTON_PIN);

  if (reading != lastButtonState && (millis() - lastDebounceTime) > 200) {
    if (reading == LOW) {
      if (currentMode == MODE_LINE_FOLLOWER) {
        currentMode = MODE_USER_FOLLOWER;
        Serial.println(">>> Mode: USER FOLLOWER <<<");
      } else {
        currentMode = MODE_LINE_FOLLOWER;
        Serial.println(">>> Mode: LINE FOLLOWER <<<");
      }
      stopMotors();
      delay(300);
    }
    lastDebounceTime = millis();
  }
  lastButtonState = reading;
}

// ==========================================
//            MOTOR HELPER FUNCTIONS
// ==========================================
void moveForward() {
  analogWrite(ENA, BASE_SPEED);
  analogWrite(ENB, BASE_SPEED);
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  analogWrite(ENA, BASE_SPEED);
  analogWrite(ENB, BASE_SPEED);
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
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

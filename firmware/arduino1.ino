#include <Wire.h>

#define SLAVE_ADDRESS 8

// Front Wheels Wiring:
// Motor A = Front Left (inverted)
// Motor B = Front Right (normal)
#define MOTOR_A_PWM    3   // Front Left speed
#define MOTOR_A_DIR    12  // Front Left direction
#define MOTOR_A_BRAKE  9   // Front Left brake

#define MOTOR_B_PWM    11  // Front Right speed
#define MOTOR_B_DIR    13  // Front Right direction
#define MOTOR_B_BRAKE  8   // Front Right brake

const uint8_t motorSpeed = 200;  // Nominal speed (0–255)
unsigned long lastReceivedTime = 0;


void stopMotors() {
  analogWrite(MOTOR_A_PWM, 0);
  analogWrite(MOTOR_B_PWM, 0);
  digitalWrite(MOTOR_A_BRAKE, HIGH);
  digitalWrite(MOTOR_B_BRAKE, HIGH);
}

void setMotor(int pwmPin, int dirPin, int brakePin, int speed, bool logicalForward) {
  // Release brake, set direction, and apply PWM.
  digitalWrite(brakePin, LOW);
  digitalWrite(dirPin, logicalForward ? HIGH : LOW);
  analogWrite(pwmPin, speed);
}

// For front wheels, effective output = !logical for Motor A (inverted), and = logical for Motor B.
void receiveEvent(int howMany) {
  if (howMany < 1) return;
  uint8_t command = Wire.read();
  lastReceivedTime = millis();
  Serial.print("Front Wheels Received command: ");
  Serial.println(command);
  
  // Immediately stop before applying new command
  stopMotors();
  
  switch (command) {
    case 1: // Forward
      // Desired effective: FL positive, FR positive.
      // For FL (inverted): send false; for FR (normal): send true.
      Serial.println("Front Wheels: Forward");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, false);
      break;
    case 2: // Backward
      // Desired effective: FL negative, FR negative.
      // For FL (inverted): send true; for FR (normal): send false.
      Serial.println("Front Wheels: Backward");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);

      break;
    case 3: // Left (strafe left)
      // For left: both wheels receive logical false.
      Serial.println("Front Wheels: Left");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, false);
      break;
    case 4: // Right (strafe right)
      // For right: both wheels receive logical true.
      Serial.println("Front Wheels: Right");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 5: // Diagonal Forward Left
      Serial.println("Front Wheels: Diagonal Forward Left");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed/2, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 6: // Diagonal Forward Right
      Serial.println("Front Wheels: Diagonal Forward Right");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed/2, false);
      break;
    case 7: // Diagonal Backward Left
      Serial.println("Front Wheels: Diagonal Backward Left");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed/2, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 8: // Diagonal Backward Right
      Serial.println("Front Wheels: Diagonal Backward Right");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed/2, false);
      break;
    case 0:
    default:
      Serial.println("Front Wheels: Stop");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Front Wheels Arduino Starting...");
  
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_A_BRAKE, OUTPUT);
  
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);
  pinMode(MOTOR_B_BRAKE, OUTPUT);
  
  stopMotors();
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  
  Serial.println("Front Wheels I2C Slave ready.");
  lastReceivedTime = millis();
}

void loop() {
}

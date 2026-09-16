#include <Wire.h>

#define SLAVE_ADDRESS 9

// Back Wheels Wiring:
// Motor A = Back Left (normal)
// Motor B = Back Right (inverted)
#define MOTOR_A_PWM    3   // Back Left speed
#define MOTOR_A_DIR    12  // Back Left direction
#define MOTOR_A_BRAKE  9   // Back Left brake

#define MOTOR_B_PWM    11  // Back Right speed
#define MOTOR_B_DIR    13  // Back Right direction
#define MOTOR_B_BRAKE  8   // Back Right brake

const uint8_t motorSpeed = 200;  // Nominal speed
unsigned long lastReceivedTime = 0;

void stopMotors() {
  analogWrite(MOTOR_A_PWM, 0);
  analogWrite(MOTOR_B_PWM, 0);
  digitalWrite(MOTOR_A_BRAKE, HIGH);
  digitalWrite(MOTOR_B_BRAKE, HIGH);
}

void setMotor(int pwmPin, int dirPin, int brakePin, int speed, bool logicalForward) {
  digitalWrite(brakePin, LOW);
  digitalWrite(dirPin, logicalForward ? HIGH : LOW);
  analogWrite(pwmPin, speed);
}

// For back wheels, effective output = logical for Motor A (normal) and = !logical for Motor B (inverted).
void receiveEvent(int howMany) {
  if (howMany < 1) return;
  uint8_t command = Wire.read();
  lastReceivedTime = millis();
  Serial.print("Back Wheels Received command: ");
  Serial.println(command);
  
  stopMotors();
  
  switch (command) {
    case 1: // Forward:
      // Desired effective: BL positive, BR positive.
      // For BL (normal): send true; for BR (inverted): send false.
      Serial.println("Back Wheels: Forward");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, false);
      break;
    case 2: // Backward:
      // Desired effective: BL negative, BR negative.
      // For BL: send false; for BR (inverted): send true.
      Serial.println("Back Wheels: Backward");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 3: // Left (strafe left):
      // For left: both wheels receive logical false.
      Serial.println("Back Wheels: Left");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, false);
      break;
    case 4: // Right (strafe right):
      // For right: both wheels receive logical true.
      Serial.println("Back Wheels: Right");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 5: // Diagonal Forward Left
      Serial.println("Back Wheels: Diagonal Forward Left");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed/2, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 6: // Diagonal Forward Right
      Serial.println("Back Wheels: Diagonal Forward Right");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed/2, false);
      break;
    case 7: // Diagonal Backward Left
      Serial.println("Back Wheels: Diagonal Backward Left");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed/2, true);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed, true);
      break;
    case 8: // Diagonal Backward Right
      Serial.println("Back Wheels: Diagonal Backward Right");
      setMotor(MOTOR_A_PWM, MOTOR_A_DIR, MOTOR_A_BRAKE, motorSpeed, false);
      setMotor(MOTOR_B_PWM, MOTOR_B_DIR, MOTOR_B_BRAKE, motorSpeed/2, false);
      break;
    case 0:
    default:
      Serial.println("Back Wheels: Stop");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Back Wheels Arduino Starting...");
  
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_A_BRAKE, OUTPUT);
  
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);
  pinMode(MOTOR_B_BRAKE, OUTPUT);
  
  stopMotors(); 
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  
  Serial.println("Back Wheels I2C Slave ready.");
  lastReceivedTime = millis();
}

void loop() {

}

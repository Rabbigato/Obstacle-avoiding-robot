// ===== Obstacle-avoiding robot with servo-mounted sensor (ESP32) =====
// L298N + 4 motors (2 per side), HC-SR04 on an SG90 servo

#include <ESP32Servo.h>

// ---- Pins ----
const int ENA = 14, IN1 = 27, IN2 = 26;   // left motors
const int IN3 = 25, IN4 = 33, ENB = 32;   // right motors
const int TRIG = 5, ECHO = 18;            // ECHO through voltage divider
const int SERVO_PIN = 13;

// ---- Tuning ----
const int SPEED      = 150;   // keep <= ~170 on 3 cells so TT motors don't overheat
const int TURN_SPEED = 160;
const int SAFE_CM    = 25;    // obstacle closer than this -> avoid
const int BACK_MS    = 350;   // reverse time
const int TURN_MS    = 400;   // roughly a 90 degree turn - adjust on your floor

const int LOOK_RIGHT  = 20;   // servo angles - swap if it looks the wrong way
const int LOOK_CENTER = 90;
const int LOOK_LEFT   = 160;

Servo eye;
ESP32PWM pwmA, pwmB;

// ---------- Motors ----------
void setMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, leftSpeed > 0);
  digitalWrite(IN2, leftSpeed < 0);
  pwmA.write(abs(leftSpeed));

  digitalWrite(IN3, rightSpeed > 0);
  digitalWrite(IN4, rightSpeed < 0);
  pwmB.write(abs(rightSpeed));
}

void forward()    { setMotors( SPEED,       SPEED); }
void backward()   { setMotors(-SPEED,      -SPEED); }
void turnLeft()   { setMotors(-TURN_SPEED,  TURN_SPEED); }
void turnRight()  { setMotors( TURN_SPEED, -TURN_SPEED); }
void stopMotors() { setMotors(0, 0); }

// ---------- Ultrasonic ----------
long readDistanceCm() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000);
  if (duration == 0) return 999;            // no echo = nothing in range
  return duration * 0.0343 / 2;
}

// Take 3 readings and keep the closest one (safest choice)
long getDistance() {
  long best = 999;
  for (int i = 0; i < 3; i++) {
    long d = readDistanceCm();
    if (d < best) best = d;
    delay(15);
  }
  return best;
}

// Point the sensor somewhere and measure
long lookAt(int angle) {
  eye.write(angle);
  delay(400);                 // give the servo time to get there
  long d = getDistance();
  Serial.print("  angle "); Serial.print(angle);
  Serial.print(" -> "); Serial.print(d); Serial.println(" cm");
  return d;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);

  // Let the servo library and the motor PWM share the ESP32's timers safely
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  pwmA.attachPin(ENA, 1000, 8);   // 1 kHz, 0-255 speed
  pwmB.attachPin(ENB, 1000, 8);

  eye.setPeriodHertz(50);
  eye.attach(SERVO_PIN, 500, 2400);
  eye.write(LOOK_CENTER);

  stopMotors();
  Serial.println("Robot ready - starting in 2 seconds");
  delay(2000);
}

// ---------- Main loop ----------
void loop() {
  long d = getDistance();
  Serial.print("Ahead: "); Serial.print(d); Serial.println(" cm");

  if (d > SAFE_CM) {          // path clear
    forward();
    delay(50);
    return;
  }

  // ---- Obstacle ahead ----
  Serial.println("Obstacle! Backing up and scanning...");
  stopMotors();  delay(150);
  backward();    delay(BACK_MS);
  stopMotors();  delay(150);

  long right = lookAt(LOOK_RIGHT);
  long left  = lookAt(LOOK_LEFT);
  eye.write(LOOK_CENTER);
  delay(300);

  if (right <= SAFE_CM && left <= SAFE_CM) {
    Serial.println("Both sides blocked -> turning around");
    turnRight(); delay(TURN_MS * 2);
  } else if (right >= left) {
    Serial.println("Turning right");
    turnRight(); delay(TURN_MS);
  } else {
    Serial.println("Turning left");
    turnLeft();  delay(TURN_MS);
  }
  stopMotors();
  delay(150);
}
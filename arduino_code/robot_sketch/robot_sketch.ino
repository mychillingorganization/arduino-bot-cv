// ================================================================
// ARDUINO - ROBOT ĐA NĂNG (XE + CÁNH TAY NHÚN NHẢY)
// ================================================================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVO_BASE_PIN      0
#define SERVO_SHOULDER_PIN  4
#define SERVO_ELBOW_PIN     8
#define SERVO_GRIPPER_PIN   12
#define SERVOMIN  150
#define SERVOMAX  600
#define ANGLE_STEP 5
int baseAngle, shoulderAngle, elbowAngle, gripperAngle;
int prev_baseAngle, prev_shoulderAngle, prev_elbowAngle, prev_gripperAngle;

#define ENA 5
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
#define ENB 6
int speedMotor = 200;


// ================================================================
// SETUP (Giữ nguyên)
// ================================================================
void setup() {
  Serial.begin(9600);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopCar();
  pwm.begin();
  pwm.setPWMFreq(50);
  goHome();
  Serial.println("Robot da nang da san sang. Cho lenh...");
  Serial.println(freeRam());
}

// ================================================================
// LOOP
// ================================================================
// Add this inside your existing Arduino Sketch

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    // If command is 'C', we expect angles: C:base,shoulder,elbow,gripper
    if (cmd == 'C') {
       Serial.read(); // Skip the ':'
       int b = Serial.parseInt();
       int s = Serial.parseInt();
       int e = Serial.parseInt();
       int g = Serial.parseInt();
       
       // Direct assignment for smooth tracking
       baseAngle = b; 
       shoulderAngle = s; 
       elbowAngle = e; 
       gripperAngle = g;
       updateAllServos();
    } 
    // Handle standard single-char commands (Old logic)
    else if (cmd >= '0' && cmd <= '5') {
      executeCarCommand(cmd);
    } else {
      executeArmCommand(cmd); // Your existing function
    }
  }
}

// ================================================================
// CÁC HÀM XỬ LÝ LỆNH
// ================================================================
void executeCarCommand(char cmd) { /* ... Giữ nguyên ... */ }

void executeArmCommand(char cmd) {
  switch (cmd) {    
    // === THAY ĐỔI 1: Đổi 'p' thành 'q' và gọi hàm mới ===
    // case 'q': // Dance
    //   armDanceRoutine();
    //   // Sau khi nhảy xong, trở về vị trí đã lưu trước đó
    //   restorePreviousPosition();
    //   updateAllServos();
    //   break;

    // ... (các case còn lại giữ nguyên) ...
    case 'a': baseAngle += ANGLE_STEP; break;
    case 'd': baseAngle -= ANGLE_STEP; break;
    case 'w': shoulderAngle -= ANGLE_STEP; break;
    case 's': shoulderAngle += ANGLE_STEP; break;
    case 'r': elbowAngle -= ANGLE_STEP; break;
    case 'f': elbowAngle += ANGLE_STEP; break;
    case 'o': gripperAngle = 90; break;
    case 'c': gripperAngle = 0;  break;
    case 'm':
      savePreviousPosition();
      baseAngle = 90; shoulderAngle = 90; elbowAngle = 90; gripperAngle = 90;
      break;
    case 'n':
      restorePreviousPosition();
      break;
  }
  
  // Nếu lệnh không phải là 'q', thì cập nhật servo như bình thường
  if (cmd != 'q') {
    updateAllServos();
  }
}

// ================================================================
// CÁC HÀM ĐIỀU KHIỂN XE (Giữ nguyên)
// ================================================================
// ... (Toàn bộ các hàm forward, backward, etc. giữ nguyên) ...


// ================================================================
// CÁC HÀM ĐIỀU KHIỂN CÁNH TAY
// ================================================================

// === THAY ĐỔI 2: Hàm nhảy mới chỉ dành cho cánh tay ===
void armDanceRoutine() {
  Serial.println("Canh tay bat dau nhung nhay!");
  goHome();
  // Lưu lại vị trí hiện tại để có thể quay về
  savePreviousPosition();

  // Động tác 1: Vẫy tay chào
  setServoAngle(SERVO_ELBOW_PIN, 90);
  setServoAngle(SERVO_SHOULDER_PIN, 90);
  for (int i=0; i<3; i++) {
    setServoAngle(SERVO_BASE_PIN, 60);
    delay(500);
    setServoAngle(SERVO_BASE_PIN, 120);
    delay(500);
  }
  setServoAngle(SERVO_BASE_PIN, 90); // Về giữa
  delay(500);

  // Động tác 2: Gật gù
  for (int i=0; i<3; i++) {
    setServoAngle(SERVO_SHOULDER_PIN, 120);
    setServoAngle(SERVO_ELBOW_PIN, 120);
    delay(500);
    setServoAngle(SERVO_SHOULDER_PIN, 90);
    setServoAngle(SERVO_ELBOW_PIN, 90);
    delay(500);
  }
  delay(500);
  
  // Động tác 3: "The Wave" - Sóng tay
  setServoAngle(SERVO_BASE_PIN, 45);
  setServoAngle(SERVO_ELBOW_PIN, 180);
  for (int angle = 45; angle <= 135; angle += 15) {
    setServoAngle(SERVO_SHOULDER_PIN, angle);
    delay(150);
  }
  for (int angle = 135; angle >= 45; angle -= 15) {
    setServoAngle(SERVO_SHOULDER_PIN, angle);
    delay(150);
  }
  
  Serial.println("Ket thuc nhung nhay!");
}


// ... (Toàn bộ các hàm còn lại của cánh tay và xe giữ nguyên) ...
// --- Dán đầy đủ các hàm còn lại vào đây ---
// Các hàm xe
void forward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, speedMotor); analogWrite(ENB, speedMotor);
}
void backward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedMotor); analogWrite(ENB, speedMotor);
}
void turnLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, speedMotor); analogWrite(ENB, speedMotor);
}
void turnRight() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedMotor); analogWrite(ENB, speedMotor);
}
void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
// Các hàm cánh tay
void updateAllServos() {
  baseAngle = constrain(baseAngle, 0, 180);
  shoulderAngle = constrain(shoulderAngle, 15, 160);
  elbowAngle = constrain(elbowAngle, 0, 180);
  setServoAngle(SERVO_BASE_PIN, baseAngle);
  setServoAngle(SERVO_SHOULDER_PIN, shoulderAngle);
  setServoAngle(SERVO_ELBOW_PIN, elbowAngle);
  setServoAngle(SERVO_GRIPPER_PIN, gripperAngle);
  }
void setServoAngle(int servoPin, int angle) {
  int pulseLen = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(servoPin, 0, pulseLen);
}
void savePreviousPosition() {
  prev_baseAngle = baseAngle; prev_shoulderAngle = shoulderAngle;
  prev_elbowAngle = elbowAngle; prev_gripperAngle = gripperAngle;
}
void restorePreviousPosition() {
  baseAngle = prev_baseAngle; shoulderAngle = prev_shoulderAngle;
  elbowAngle = prev_elbowAngle; gripperAngle = prev_gripperAngle;
}
void goHome() {
  baseAngle = 90; shoulderAngle = 150; elbowAngle = 90; gripperAngle = 90;
  savePreviousPosition();
  updateAllServos();
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
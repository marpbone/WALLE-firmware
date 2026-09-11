#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Servo driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Pulse limits
#define SERVO_MIN 120
#define SERVO_MAX 500

// Channels
#define SERVO_0 0 // face : 0 = Dead right ; 70 = Dead center ; Dead left = 140 (160)
#define SERVO_4 4 // upper neck : 75 = high ; 140 = low
#define SERVO_7 7 // lower neck : 25 = low ; 125 = high
#define SERVO_13 13 // right eye : high 115 ; low 70

// Angle limits
#define S0_MIN 75
#define S0_MAX 160

#define S4_MIN 80
#define S4_MAX 135

#define S7_MIN 30
#define S7_MAX 120

#define S13_MIN 70
#define S13_MAX 115

// Current angles
int angle0 = 75;
int angle4 = 80;
int angle7 = 30;
int angle13 = 90;

// Direction
int dir0 = 1;
int dir4 = 1;
int dir7 = 1;
int dir13 = 1;

// Timing
unsigned long lastUpdate0 = 0;
unsigned long lastUpdate4 = 0;
unsigned long lastUpdate7 = 0;
unsigned long lastUpdate13 = 0;

// Speeds
int interval0 = 12;
int interval4 = 20;
int interval7 = 16;
int interval13 = 8; // eye = faster = more natural

// -------- SERIAL DATA --------
int source = 0;
int mood = 0;
int confidence = 0;
int landmark = 0;

String evaluatedMood;

// -------- MOOD CONTROL --------
bool moodActive = false;
unsigned long moodStartTime = 0;
const int moodDuration = 3000;

// -------- DEBUG --------
unsigned long lastDebugTime = 0;
int debugInterval = 1000;

// Convert angle → pulse
uint16_t angleToPulse(int angle) {
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

// -------- SERIAL PARSER --------
void readSerial()
{
  if (Serial.available() >= 4)
  {
    mood = Serial.read();
    source = Serial.read();
    confidence = Serial.read();
    landmark = Serial.read();

    evaluatedMood = evaluateMood();
  }
}

// -------- MOOD NAME --------
String evaluateMood()
{
  switch (mood)
  {
    case 1: return "ANGRY";
    case 2: return "DISGUST";
    case 3: return "FEAR";
    case 4: return "HAPPY";
    case 5: return "SAD";
    case 6: return "SURPRISED";
    case 7: return "NEUTRAL";
    default: return "NONE";
  }
}

// -------- APPLY MOOD --------
void applyMood()
{
  switch (mood)
  {
    case 1:
      pwm.setPWM(SERVO_0, 0, angleToPulse(S0_MIN));
      pwm.setPWM(SERVO_4, 0, angleToPulse(S4_MAX));
      pwm.setPWM(SERVO_7, 0, angleToPulse(S7_MAX));
      pwm.setPWM(SERVO_13, 0, angleToPulse(S13_MAX)); // eye up
      break;

    case 2:
      pwm.setPWM(SERVO_0, 0, angleToPulse(S0_MAX));
      pwm.setPWM(SERVO_4, 0, angleToPulse(S4_MIN));
      pwm.setPWM(SERVO_7, 0, angleToPulse(S7_MIN));
      pwm.setPWM(SERVO_13, 0, angleToPulse(S13_MIN)); // eye down
      break;

    case 3:
      pwm.setPWM(SERVO_0, 0, angleToPulse(S0_MAX));
      pwm.setPWM(SERVO_4, 0, angleToPulse(S4_MIN));
      pwm.setPWM(SERVO_7, 0, angleToPulse(S7_MAX));
      pwm.setPWM(SERVO_13, 0, angleToPulse(S13_MAX));
      break;

    case 4:
      pwm.setPWM(SERVO_0, 0, angleToPulse((S0_MIN + S0_MAX)/2));
      pwm.setPWM(SERVO_4, 0, angleToPulse(S4_MIN));
      pwm.setPWM(SERVO_7, 0, angleToPulse(S7_MAX));
      pwm.setPWM(SERVO_13, 0, angleToPulse((S13_MIN + S13_MAX)/2));
      break;

    case 5:
      pwm.setPWM(SERVO_0, 0, angleToPulse((S0_MIN + S0_MAX)/2));
      pwm.setPWM(SERVO_4, 0, angleToPulse(S4_MAX));
      pwm.setPWM(SERVO_7, 0, angleToPulse(S7_MIN));
      pwm.setPWM(SERVO_13, 0, angleToPulse(S13_MIN));
      break;

    case 6:
      pwm.setPWM(SERVO_0, 0, angleToPulse((S0_MIN + S0_MAX)/2));
      pwm.setPWM(SERVO_4, 0, angleToPulse(S4_MIN));
      pwm.setPWM(SERVO_7, 0, angleToPulse(S7_MAX));
      pwm.setPWM(SERVO_13, 0, angleToPulse(S13_MAX));
      break;

    default:
      pwm.setPWM(SERVO_0, 0, angleToPulse((S0_MIN + S0_MAX)/2));
      pwm.setPWM(SERVO_4, 0, angleToPulse((S4_MIN + S4_MAX)/2));
      pwm.setPWM(SERVO_7, 0, angleToPulse((S7_MIN + S7_MAX)/2));
      pwm.setPWM(SERVO_13, 0, angleToPulse((S13_MIN + S13_MAX)/2));
      break;
  }
}

// -------- IDLE MOTION --------
void updateServos()
{
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate0 >= interval0)
  {
    lastUpdate0 = currentTime;
    angle0 += dir0;
    if (angle0 >= S0_MAX || angle0 <= S0_MIN) dir0 *= -1;
    pwm.setPWM(SERVO_0, 0, angleToPulse(angle0));
  }

  if (currentTime - lastUpdate4 >= interval4)
  {
    lastUpdate4 = currentTime;
    angle4 += dir4;
    if (angle4 >= S4_MAX || angle4 <= S4_MIN) dir4 *= -1;
    pwm.setPWM(SERVO_4, 0, angleToPulse(angle4));
  }

  if (currentTime - lastUpdate7 >= interval7)
  {
    lastUpdate7 = currentTime;
    angle7 += dir7;
    if (angle7 >= S7_MAX || angle7 <= S7_MIN) dir7 *= -1;
    pwm.setPWM(SERVO_7, 0, angleToPulse(angle7));
  }

  // 👁️ EYE ANIMATION (NEW)
  if (currentTime - lastUpdate13 >= interval13)
  {
    lastUpdate13 = currentTime;
    angle13 += dir13;

    if (angle13 >= S13_MAX || angle13 <= S13_MIN)
      dir13 *= -1;

    pwm.setPWM(SERVO_13, 0, angleToPulse(angle13));
  }
}

// -------- DEBUG --------
void debugPrint()
{
  unsigned long currentTime = millis();

  if (currentTime - lastDebugTime >= debugInterval)
  {
    lastDebugTime = currentTime;

    Serial.print("[DEBUG] Mood: ");
    Serial.print(mood);
    Serial.print(" (");
    Serial.print(evaluateMood());
    Serial.print(") | Source: ");
    Serial.print(source);
    Serial.print(" | Conf: ");
    Serial.print(confidence);
    Serial.print(" | Landmark: ");
    Serial.println(landmark);

    debugInterval = random(500, 2500);
  }
}

// -------- SETUP --------
void setup()
{
  Serial.begin(115200);
  pwm.begin();
  pwm.setPWMFreq(50);

  randomSeed(analogRead(0));
}

unsigned long lastReadyTime = 0;
unsigned long now = 0;

// -------- LOOP --------
void loop()
{

  if(now - lastReadyTime > 200 && Serial.availableForWrite() > 10) {
  Serial.println("READY");
  lastReadyTime = now;
  }

  if (Serial.available() >= 4)
  {
    readSerial();

    if (confidence > 40)
    {
      moodActive = true;
      moodStartTime = millis();
    }
  }

  if (moodActive)
  {
    applyMood();

    if (millis() - moodStartTime >= moodDuration)
    {
      moodActive = false;
    }
  }
  else
  {
    updateServos();
  }

  // debugPrint();
}
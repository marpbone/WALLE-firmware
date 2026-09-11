#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Servo shield
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servo limits
#define SERVO_MIN 120
#define SERVO_MAX 500

// Servo channels
#define SERVO_A 11
#define SERVO_B 7
#define SERVO_C 3

byte source_byte;
byte mood_byte;

int source;
int mood;

String evaluatedMood;

// Convert degrees → PCA9685 pulse
uint16_t angleToPulse(int angle)
{
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void readSerial()
{
  source_byte = Serial.read();
  mood_byte   = Serial.read();

  source = source_byte;
  mood   = mood_byte;
}

String evaluateMood()
{
  switch (mood)
  {
    case 1: evaluatedMood = "ANGRY"; break;
    case 2: evaluatedMood = "DISGUST"; break;
    case 3: evaluatedMood = "FEAR"; break;
    case 4: evaluatedMood = "HAPPY"; break;
    case 5: evaluatedMood = "SAD"; break;
    case 6: evaluatedMood = "SURPRISED"; break;
    case 7: evaluatedMood = "NEUTRAL"; break;
    default: evaluatedMood = "NONE"; break;
  }

  return evaluatedMood;
}

void setup()
{
  Serial.begin(115200);

  pwm.begin();
  pwm.setPWMFreq(60);
}

void loop()
{
  Serial.println("READY");
  if (Serial.available())
  {
    readSerial();

    switch (mood)
    {

      case 1: // ANGRY
        pwm.setPWM(SERVO_A, 0, angleToPulse(40));
        pwm.setPWM(SERVO_B, 0, angleToPulse(125));
        pwm.setPWM(SERVO_C, 0, angleToPulse(120));
        delay(3000);
        break;

      case 2: // DISGUST
        pwm.setPWM(SERVO_A, 0, angleToPulse(110));
        pwm.setPWM(SERVO_B, 0, angleToPulse(90));
        pwm.setPWM(SERVO_C, 0, angleToPulse(95));
        delay(3000);
        break;

      case 3: // FEAR
        pwm.setPWM(SERVO_A, 0, angleToPulse(110));
        pwm.setPWM(SERVO_B, 0, angleToPulse(125));
        pwm.setPWM(SERVO_C, 0, angleToPulse(90));
        delay(3000);
        break;

      case 4: // HAPPY
        pwm.setPWM(SERVO_A, 0, angleToPulse(75));
        pwm.setPWM(SERVO_B, 0, angleToPulse(105));
        pwm.setPWM(SERVO_C, 0, angleToPulse(110));
        delay(3000);
        break;

      case 5: // SAD
        pwm.setPWM(SERVO_A, 0, angleToPulse(40));
        pwm.setPWM(SERVO_B, 0, angleToPulse(90));
        pwm.setPWM(SERVO_C, 0, angleToPulse(95));
        delay(3000);
        break;

      case 6: // SURPRISED
        pwm.setPWM(SERVO_A, 0, angleToPulse(110));
        pwm.setPWM(SERVO_B, 0, angleToPulse(120));
        pwm.setPWM(SERVO_C, 0, angleToPulse(120));
        delay(3000);
        break;

      case 7: // NEUTRAL
        pwm.setPWM(SERVO_A, 0, angleToPulse(75));
        pwm.setPWM(SERVO_B, 0, angleToPulse(105));
        pwm.setPWM(SERVO_C, 0, angleToPulse(105));
        delay(3000);
        break;

      default: // NONE
        pwm.setPWM(SERVO_A, 0, angleToPulse(75));
        pwm.setPWM(SERVO_B, 0, angleToPulse(105));
        pwm.setPWM(SERVO_C, 0, angleToPulse(105));
        delay(3000);
        break;
    }

  }
}
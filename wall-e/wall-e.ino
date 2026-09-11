#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Queue.hpp"
#include "MotorController.hpp"
#include <AFMotor_R4.h>
#include <math.h>

// ================= MOTOR SETUP =================
AF_DCMotor m3(3);
AF_DCMotor m4(4);

int current_speed = 0;
int i = 0;
int fragments = 50;


// ================= LED + AUDIO SYSTEM =================
int sourceLeds[2] = {23, 37};
int moodLeds[4] = {27, 29, 31, 33};

const int AMP_SW = 22;
const unsigned long AMP_WAKE_MS = 80;

/// Define pin-mapping
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
#define DIRECTION_L_PIN 12           // Motor direction pins
#define DIRECTION_R_PIN 13
#define PWM_SPEED_L_PIN  3           // Motor PWM pins
#define PWM_SPEED_R_PIN 11
#define BRAKE_L_PIN  9               // Motor brake pins
#define BRAKE_R_PIN  8
#define SERVO_ENABLE_PIN 10          // Servo shield output enable pin
#define CONF_THRESHOLD 1


/**
 * Battery level detection
 *
 *   .------R1-----.-------R2------.     
 *   |             |               |     
 * V_Raw     Analogue pin A2      GND    

 *       DIVIDER_SCALING_FACTOR = R2 / (R1 + R2)
 *       47000 / (100000 + 47000) = 0.3197
 */
#define BAT_L
#ifdef BAT_L
	#define BATTERY_LEVEL_PIN A2
	#define BATTERY_MAX_VOLTAGE 12.6
	#define BATTERY_MIN_VOLTAGE 10.2
	#define DIVIDER_SCALING_FACTOR 0.3197

	#define OLED
	#ifdef OLED
	  
	  #include <U8g2lib.h>
	  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

	#endif /* OLED */
#endif /* BAT_L */


/// Define other constants
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
#define NUMBER_OF_SERVOS 7        // Number of servo motors
#define SERVO_UPDATE_TIME 10      // Time in milliseconds of how often to update servo and motor positions
#define SERVO_OFF_TIME 6000       // Turn servo motors off after 6 seconds
#define STATUS_CHECK_TIME 10000   // Time in milliseconds of how often to check robot status (eg. battery level)
#define CONTROLLER_THRESHOLD 0.1    // The minimum error which the dynamics controller tries to achieve
#define MAX_SERIAL_LENGTH 5       // Maximum number of characters that can be received

#define MOOD_QUEUE_SIZE 10
int moodQueueBuffer[MOOD_QUEUE_SIZE];
Queue<int> moodQueue(MOOD_QUEUE_SIZE, moodQueueBuffer);

/// Instantiate Objects
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
// Servo shield controller class - assumes default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Set up motor controller classes
MotorController motorL(DIRECTION_L_PIN, PWM_SPEED_L_PIN, BRAKE_L_PIN, false);
MotorController motorR(DIRECTION_R_PIN, PWM_SPEED_R_PIN, BRAKE_R_PIN, false);

// Queue for animations - buffer is defined outside of the queue class
// so that the compiler knows how much dynamic memory will be used
struct animation_t {
	uint16_t timer;
	int8_t servos[NUMBER_OF_SERVOS]; 
};

#define QUEUE_LENGTH 40
animation_t buffer[QUEUE_LENGTH];
Queue <animation_t> queue(QUEUE_LENGTH, buffer);


/// Motor Control Variables
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
int pwmspeed = 255;
int moveValue = 0;
int turnValue = 0;
int turnOffset = 0;
int motorDeadzone = 0;

int targetMove = 0;
int targetTurn = 0;


/// Runtime Variables
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
unsigned long lastTime = 0;
unsigned long animeTimer = 0;
unsigned long motorTimer = 0;
unsigned long statusTimer = 0;
unsigned long updateTimer = 0;
bool autoMode = true;
unsigned long lastSerialTime = 0;


// Serial Parsing
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
char firstChar;
char serialBuffer[MAX_SERIAL_LENGTH];
uint8_t serialLength = 0;


// ****** SERVO MOTOR CALIBRATION *********************
// Servo Positions:  Low,High
int preset[][2] = {
  {180, 500},  // head (70° → 140°) (180 - 500 right to left)

  {300, 450},  // upper neck (50° → 145°)
  {183, 458},  // lower neck (30° → 160°)

  {390, 490},  // right eye (MG90D) (40° → 120°)
  {390, 220},  // left eye (MG90D) (125° → 60° reversed)

  {500, 350},  // 5 arm left 
  {300, 550}   // 6 arm right  
};
// *****************************************************


// Servo Control - Position, Velocity, Acceleration
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
// Servo Pins:	     0,   1,   2,   3,   4,   5,   6,   -,   -
// Joint Name:	  head,necT,necB,eyeR,eyeL,armL,armR,motL,motR
float curpos[] = { 340, 326, 321, 252, 316, 300, 300, 180, 180}; //Current Position ()
float setpos[] = { 340, 326, 321, 252, 316, 300, 300,   0,   0};	// Required Position units() 
float curvel[] = {   0,   0,   0,   0,   0,   0,   0,   0,   0};  // Current velocity (units/sec)
float maxvel[] = {1000, 800,1000,3000,3000,1000,1000, 255, 255};  // Max Servo velocity (units/sec)
float accell[] = { 700, 600, 800,2500,2500, 800, 800 ,800, 800};  // Servo acceleration (units/sec^2)

int pickAnimationFromList(const int* options, int size) {
    int idx = random(size);
    return options[idx];
}

void moveRobot(int target_speed, int direction, int acc_window = 1, int fragments_val = 50) {
  drive(target_speed, current_speed, acc_window, fragments_val, direction);
  current_speed = target_speed;
}

void spinRobot(int speed, int duration_ms) {
  m3.setSpeed(speed);
  m4.setSpeed(speed);

  // opposite directions → spin
  m3.run(FORWARD);
  m4.run(BACKWARD);

  delay(duration_ms);

  // stop after spin
  m3.setSpeed(0);
  m4.setSpeed(0);
  m3.run(FORWARD);
  m4.run(FORWARD);

  current_speed = 0; // important reset
}

void pulseMove(int speed, int direction, int duration_ms, int acc_window = 1, int fragments_val = 50) {

  // start moving (accelerate)
  drive(speed, current_speed, acc_window, fragments_val, direction);
  current_speed = speed;

  // move for short time
  delay(duration_ms);

  // stop immediately
  m3.setSpeed(0);
  m4.setSpeed(0);
  m3.run(RELEASE);
  m4.run(RELEASE);

  current_speed = 0;
}

// -------------------------------------------------------------------
/// Initial setup
// -------------------------------------------------------------------

void setup() {

    randomSeed(analogRead(0));
	
    // Output Enable (EO) pin for the servo motors
	pinMode(SERVO_ENABLE_PIN, OUTPUT);
	digitalWrite(SERVO_ENABLE_PIN, HIGH);

	// Communicate with servo shield (Analog servos run at ~60Hz)
	pwm.begin();
	pwm.setPWMFreq(50);

	// // Turn off servo outputs
	// for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
	// 	pwm.setPin(i, 0);
	// }

	// Initialize serial communication for debugging
	Serial.begin(115200);
	// Serial.println(F("--- Wall-E Control Sketch ---"));

	randomSeed(analogRead(0));

	// Check if servo animation queue is working, and move servos to known starting positions
	// if (queue.errors()) Serial.println(F("Error: Unable to allocate memory for servo animation queue"));
	
	// Soft start the servo motors
	// Serial.println(F("Starting up the servo motors"));
	digitalWrite(SERVO_ENABLE_PIN, LOW);
	playAnimation(0);
	// softStart(queue.pop(), 3500);

	// If an oLED is present, start it up
	#ifdef OLED
	//	Serial.println(F("Starting up the display"));
		u8g2.begin();
		displayLevel(100);
	#endif

	// Serial.println(F("Startup complete; entering main loop"));

// ===== LED INIT =====
for(int i = 0; i < 2; i++){
  pinMode(sourceLeds[i], OUTPUT);  
}

for(int i = 0; i < 4; i++){
  pinMode(moodLeds[i], OUTPUT);  
}

// optional startup animation
for(int i = 2; i < 8; i++){
  digitalWrite(i, HIGH); 
  delay(100);
  digitalWrite(i, LOW); 
}

// ===== AMP INIT =====
pinMode(AMP_SW, OUTPUT);
digitalWrite(AMP_SW, LOW);

}

int source = 0;
int mood = 0;
int confidence = 0;
int landmark = 0;

byte source_b = 0;
byte mood_b = 0;
byte confidence_b = 0;
byte landmark_b = 0;

String evaluatedMood;
int idleCounter = 0;
int lastMood = -1;



String evaluateMood() {
    switch (mood) {
        case 1: return "ANGRY";
        case 2: return "DISGUST";
        case 3: return "FEAR";
        case 4: return "HAPPY";
        case 5: return "SAD";
        case 6: return "SURPRISED";
        case 7: return "NEUTRAL";
        default: return "UNKNOWN";
    }
}

void readSerial() {
  
	if (Serial.available() >= 4) {
    mood_b = Serial.read();
    source_b = Serial.read();
    confidence_b = Serial.read();
    landmark_b = Serial.read();

		mood = mood_b;
		source = source_b;
		confidence = confidence_b;
		landmark = landmark_b;

    evaluatedMood = evaluateMood();

		Serial.print("[INPUT] Mood: ");
        Serial.print(mood);
        Serial.print(" (");
        Serial.print(evaluatedMood);
        Serial.print(") | Source: ");
        Serial.print(source);
        Serial.print(" | Confidence: ");
        Serial.print(confidence);
        Serial.print(" | Landmark: ");
        Serial.println(landmark);

        processData(mood, source);
  }
}



void evaluateSerial()
{
    if (source == 0) return;
    if (confidence < CONF_THRESHOLD) return;
    if (mood == lastMood) return;

    // prevent spam
    if (!moodQueue.empty() && moodQueue.back() == mood)
        return;

    if (moodQueue.size() < MOOD_QUEUE_SIZE) {
        moodQueue.push(mood);

        Serial.print("[QUEUE] Mood added: ");
        Serial.println(mood);
    }
}

bool animationRunning = false;

void processMoodQueue()
{
    // if animation is currently running → do nothing
    if (animationRunning) return;

    // if no animation + moods waiting → start next
    if (!moodQueue.empty()) {

        int nextMood = moodQueue.pop();

        Serial.print("[EXECUTE] Mood: ");
        Serial.println(nextMood);

        int angryAnims[] = {3, 6};
        int disgustAnims[] = {2, 7};
        int fearAnims[] = {4, 8};
        int happyAnims[] = {1, 5};
        int sadAnims[] = {7, 9};
        int surprisedAnims[] = {5, 10};
        int defaultAnims[] = {1, 2};

       int ab = 9;

        // load animation into queue
        switch (nextMood)
        {
            case 1:
                pulseMove(150, FORWARD, 300);
                playAnimation(9);
                // playAnimation(6); // anger // 9
                // playAnimation(pickAnimationFromList(angryAnims, 2));
                playAnimation(0);  // reset
                moveRobot(0, FORWARD);
                break;

            case 2:
                playAnimation(6);
                // playAnimation(3);  // DISGUST // 6
                playAnimation(0);  // reset
                break;

            case 3:
                playAnimation(4);
                //playAnimation(4);  // FEAR // 4
                playAnimation(0);  // reset
                break;

            case 4:
                playAnimation(2);
               // playAnimation(2);  // HAPPY // 2
                playAnimation(0);  // reset
                break;

            case 5:
                playAnimation(3);
                //playAnimation(3);  // SAD // 3
                playAnimation(0);  // reset
                break;

            case 6:
                playAnimation(8);
                //playAnimation(8);  // SURPRISED // 8 
                playAnimation(0);  // reset
                break;

            case 7:
                playAnimation(0);
                //playAnimation(0);  // NEUTRAL // 0
                break;

            default:
                playAnimation(5);  // idk // 5
                playAnimation(0);  // reset
                break;
        }

        animationRunning = true;
    }
}





void drive(int target_speed, int cur_speed, int acc_window, int fragments, int direction){
  int ms = acc_window*1000;
  int segment = ms/fragments;
  float s = 0.0;

  if(cur_speed <= 0){
    s = 1.0;
  } else if(cur_speed >= 255){
    s = 255.0;
  } else{
    s = (float)cur_speed;
  }

  if(target_speed <= 0){
    target_speed = 1;
  } else if(target_speed > 255){
    target_speed = 255;
  }

  float start_speed = s;
  float end_speed = (float)target_speed;
  float delta = end_speed-start_speed;

  for(i=0;i<fragments;i++){
    float x = (float)(i+1)/50.0;
    float factor = 0.0;

    if(delta > 0){
      factor = (exp(3.0*x)-1.0)/(exp(3.0)-1.0);
    } else if(delta < 0){
      factor = (1.0-exp(-3.0*x))/(1.0-exp(-3.0));
    } else {
      factor = 1.0;
    }

    s = start_speed+(delta*factor);

    if(s<1.0){
      s = 1.0;
    } else if(s > 255.0){
      s = 255.0;
    }

    int pwm = (int)round(s);
    
    m3.setSpeed(pwm);
    m4.setSpeed(pwm);
    m3.run(direction);
    m4.run(direction);
    delay(segment);
  }
}


#define IDLE_LIMIT 5



static bool idleTriggered = false;

void idleTimerFunction()
{
    bool shouldIncrement = false;

    if (source == 0)
        shouldIncrement = true;
    else if (confidence < CONF_THRESHOLD)
        shouldIncrement = true;
    else if (mood == lastMood)
        shouldIncrement = true;

    if (shouldIncrement)
    {
        idleCounter++;

        if (idleCounter >= IDLE_LIMIT && !idleTriggered)
        {
            moodQueue.clear();
            moodQueue.push(0);

            animationRunning = false;

            idleCounter = 0;
            idleTriggered = true;
        }
    }
    else
    {
        idleCounter = 0;
        idleTriggered = false;
    }
}


void processData(int moodVal, int sourceVal){
  
  // ---- SOURCE LEDS ----
  for(int i = 0; i < 2; i++){
    int bit = (sourceVal >> i) & 1;
    digitalWrite(sourceLeds[i], bit);
  }

  // ---- MOOD LEDS ----
  for(int i = 0; i < 4; i++){
    int bit = (moodVal >> i) & 1;
    digitalWrite(moodLeds[i], bit);
  }

  // ---- AUDIO ----
  play_audio(moodVal);

  delay(3000);

  // ---- RESET ----
  for(int i = 0; i < 2; i++) digitalWrite(sourceLeds[i], LOW);
  for(int i = 0; i < 4; i++) digitalWrite(moodLeds[i], LOW);

  amp_off();
}

void amp_on(){
  digitalWrite(AMP_SW, HIGH);
  delay(AMP_WAKE_MS);
}

void amp_off(){
  digitalWrite(AMP_SW, LOW);
}

void play_audio(int moodVal){
  amp_on();
  Serial.println("SOUND");
  Serial.println(moodVal);
}

void stop_audio(){
  Serial.println("STOP");
}

// -------------------------------------------------------------------
/// Sequence and generate animations
// -------------------------------------------------------------------




void manageAnimations() {

    static bool stepActive = false;

    // Load next step ONLY when needed
    if (!stepActive && queue.size() > 0 && animeTimer <= millis()) {

        animation_t newValues = queue.pop();
        animeTimer = millis() + newValues.timer;
        stepActive = true;

        for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
            setpos[i] = int(newValues.servos[i] * 0.01 * (preset[i][1] - preset[i][0]) + preset[i][0]);
        }

        Serial.println("[STEP]");
    }

    // step finished → allow next
    if (stepActive && millis() >= animeTimer) {
        stepActive = false;
    }

    // if queue empty → animation done
    if (queue.empty() && !stepActive) {
        animationRunning = false;
    }
}


void manageAnimations() {

	// If we are running an animation
	// -- -- -- -- -- -- -- -- -- -- -- -- -- --
	if ((queue.size() > 0) && (animeTimer <= millis())) {
		// Set the next waypoint time
		animation_t newValues = queue.pop();
		animeTimer = millis() + newValues.timer;

		// Set all the joint positions
		for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
			// Scale the positions using the servo calibration values
			setpos[i] = int(newValues.servos[i] * 0.01 * (preset[i][1] - preset[i][0]) + preset[i][0]);
		}


	// // If we are in autonomous mode and no movements are queued, generate random movements
	// // -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// } else if (autoMode && queue.empty() && (animeTimer <= millis())) {

	// 	// For each of the servos
	// 	for (int i = 0; i < NUMBER_OF_SERVOS; i++) {

	// 		// Randomly determine whether or not to update the servo
	// 		if (random(2) == 1) {

	// 			// For most of the servo motors
	// 			if (i == 0 || i == 1 || i == 5 || i == 6) {

	// 				// Randomly determine the new position
	// 				unsigned int min = preset[i][0];
	// 				unsigned int max = preset[i][1];
	// 				if (min > max) {
	// 					min = max;
	// 					max = preset[i][0];
	// 				}
					
	// 				setpos[i] = random(min, max+1);

	// 			// Since the eyes should work together, only look at one of them
	// 			} else if (i == 3) {

	// 				int midPos1 = int((preset[i][1] - preset[i][0])*0.4 + preset[i][0]);
	// 				int midPos2 = int((preset[i+1][1] - preset[i+1][0])*0.4 + preset[i+1][0]);

	// 				// Determine which type of eye movement to do
	// 				// Both eye move downwards
	// 				if (random(2) == 1) {
	// 					setpos[i] = random(midPos1, preset[i][0]);
	// 					float multiplier = (setpos[i] - midPos1) / float(preset[i][0] - midPos1);
	// 					setpos[i+1] = ((1 - multiplier) * (midPos2 - preset[i+1][0])) + preset[i+1][0];

	// 				// Both eyes move in opposite directions
	// 				} else {
	// 					setpos[i] = random(midPos1, preset[i][0]);
	// 					float multiplier = (setpos[i] - preset[i][1]) / float(preset[i][0] - preset[i][1]);
	// 					setpos[i+1] = (multiplier * (preset[i+1][1] - preset[i+1][0])) + preset[i+1][0];
	// 				}
	// 			}

	// 		}
	// 	}

	// 	// Finally, figure out the amount of time until the next movement should be done
	// 	animeTimer = millis() + random(500, 3000);

 }
}



-------------------------------------------------------------------
/ Manage the movement of the servo motors
/
/ formulas:
/   (s = position, v = velocity, a = acceleration, t = time)
/   s = v^2 / (2*a)  <- to figure out whether to start slowing down
/   v = v + a*t      <- to calculate new servo velocity
/   s = s + v*t      <- to calculate new servo position
-------------------------------------------------------------------

void manageServos(float dt) {

	bool moving = false;

	// For each of the servo motors
	for (int i = 0; i < NUMBER_OF_SERVOS; i++) {

		float posError = setpos[i] - curpos[i];

		// If position error is above the threshold
		if (abs(posError) > CONTROLLER_THRESHOLD && (setpos[i] != -1)) {

			digitalWrite(SERVO_ENABLE_PIN, LOW);
			moving = true;

			// Determine motion direction
			bool dir = true;
			if (posError < 0) dir = false;

			// Determine whether to accelerate or decelerate
			float acceleration = accell[i];
			if ((curvel[i] * curvel[i] / (2 * accell[i])) > abs(posError)) acceleration = -accell[i];

			// Update the current velocity
			if (dir) curvel[i] += acceleration * dt / 1000.0;
			else curvel[i] -= acceleration * dt / 1000.0;

			// Limit Velocity
			if (curvel[i] > maxvel[i]) curvel[i] = maxvel[i];
			if (curvel[i] < -maxvel[i]) curvel[i] = -maxvel[i];
			
			float dP = curvel[i] * dt / 1000.0;

			if (abs(dP) < abs(posError)) curpos[i] += dP;
			else curpos[i] = setpos[i];

			pwm.setPWM(i, 0, curpos[i]);

		} else {
			curvel[i] = 0;
		}
	}

	// Disable servos if robot is not moving
	// This helps prevents the motors from overheating
	if (moving) motorTimer = millis();
	else if (millis() - motorTimer >= SERVO_OFF_TIME) {
		//digitalWrite(SERVO_ENABLE_PIN, HIGH);
		for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
			pwm.setPin(i, 0);
		}
	}
}


void manageServos(float dt) {

    bool moving = false;

    for (int i = 0; i < NUMBER_OF_SERVOS; i++) {

        if (setpos[i] != -1) {
            if (abs(setpos[i] - curpos[i]) > CONTROLLER_THRESHOLD) {
                moving = true;
            }

            curpos[i] = setpos[i];
            pwm.setPWM(i, 0, (int)curpos[i]);
            curvel[i] = 0;
        }
    }

    motorTimer = millis();
}

void softStart(animation_t targetPos, int timeMs) {

	for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
		if (targetPos.servos[i] >= 0) {
			curpos[i] = int(targetPos.servos[i] * 0.01 * (preset[i][1] - preset[i][0]) + preset[i][0]);

			unsigned long endTime = millis() + timeMs / NUMBER_OF_SERVOS;

			while (millis() < endTime) {
				pwm.setPWM(i, 0, curpos[i]);
				delay(10);
				pwm.setPin(i, 0);
				delay(50);
			}
			pwm.setPWM(i, 0, curpos[i]);
			setpos[i] = curpos[i];
		}
	}
}



-------------------------------------------------------------------
/ Manage the movement of the main motors
-------------------------------------------------------------------

void manageMotors(float dt) {

	// Update Main Motor Values
	setpos[NUMBER_OF_SERVOS] = moveValue - turnValue;
	setpos[NUMBER_OF_SERVOS + 1] = moveValue + turnValue;

    // --- Smooth forward/back ---
    float diff = targetMove - moveValue;

    if (abs(diff) > 1) {
        float accelRate = 200.0;

        if (diff > 0)
            moveValue += accelRate * dt;
        else
            moveValue -= accelRate * dt;
    } else {
        moveValue = targetMove;
    }

    // --- Smooth turning ---
    float turnDiff = targetTurn - turnValue;

    if (abs(turnDiff) > 1) {
        float turnRate = 300.0;

        if (turnDiff > 0)
            turnValue += turnRate * dt;
        else
            turnValue -= turnRate * dt;
    } else {
        turnValue = targetTurn;
    }

	// Apply turn offset (motor trim) only when motors are active
	if (setpos[NUMBER_OF_SERVOS] != 0) setpos[NUMBER_OF_SERVOS] -= turnOffset;
	if (setpos[NUMBER_OF_SERVOS + 1] != 0) setpos[NUMBER_OF_SERVOS + 1] += turnOffset;

	for (int i = NUMBER_OF_SERVOS; i < NUMBER_OF_SERVOS + 2; i++) {

		float velError = setpos[i] - curvel[i];

		// If velocity error is above the threshold
		if (abs(velError) > CONTROLLER_THRESHOLD && (setpos[i] != -1)) {

			// Determine whether to accelerate or decelerate
			float acceleration = accell[i];
			if (setpos[i] < curvel[i] && curvel[i] >= 0) acceleration = -accell[i];
			else if (setpos[i] < curvel[i] && curvel[i] < 0) acceleration = -accell[i]; 
			else if (setpos[i] > curvel[i] && curvel[i] < 0) acceleration = accell[i];

			// Update the current velocity
			float dV = acceleration * dt / 1000.0;
			if (abs(dV) < abs(velError)) curvel[i] += dV;
			else curvel[i] = setpos[i];
		} else {
			curvel[i] = setpos[i];
		}

		// Apply deadzone offset
		if (curvel[i] > 0) curvel[i] += motorDeadzone;
		else if (curvel[i] < 0) curvel[i] -= motorDeadzone; 

		// Limit Velocity
		if (curvel[i] > maxvel[i]) curvel[i] = maxvel[i];
		if (curvel[i] < -maxvel[i]) curvel[i] = -maxvel[i];
	}

	// Update motor speeds
	motorL.setSpeed(curvel[NUMBER_OF_SERVOS]);
	motorR.setSpeed(curvel[NUMBER_OF_SERVOS+1]);
}



-------------------------------------------------------------------
/ Battery level detection
-------------------------------------------------------------------

#ifdef BAT_L
void checkBatteryLevel() {

	// Read the analogue pin and calculate battery voltage
	float voltage = analogRead(BATTERY_LEVEL_PIN) * 5 / 1024.0;
	voltage = voltage / DIVIDER_SCALING_FACTOR;
	int percentage = int(100 * (voltage - BATTERY_MIN_VOLTAGE) / float(BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE));

  // Update the oLed Display if installed
  #ifdef OLED
    displayLevel(percentage);
  #endif

	// Send the percentage via serial
	//Serial.print(F("Battery_")); Serial.println(percentage);
}
// #endif

void checkBatteryLevel() {
    displayLevel(100);
}


// -------------------------------------------------------------------
/// Main program loop
// -------------------------------------------------------------------

unsigned long lastReadyTime = 0;
const unsigned long READY_INTERVAL = 2500;

void loop() {

  unsigned long now = millis();

	  if (now - lastReadyTime >= READY_INTERVAL) {
    Serial.println("READY");
    lastReadyTime = now;
  	}

    bool hasSerial = (Serial.available() >= 4);

    if (hasSerial) {
        readSerial();
        autoMode = false;
        lastSerialTime = millis();
    }

    // timeout ? back to autonomous
    if (millis() - lastSerialTime > 10000) {
        autoMode = true;
    }

    bool validInput = (source != 0 && confidence >= CONF_THRESHOLD);

	if (!autoMode && validInput) {
			evaluateSerial();
			idleCounter = 0;
			idleTriggered = false;
	}
	else if (!autoMode) {
			idleTimerFunction();
	}


    lastMood = mood;

		processMoodQueue(); 
		manageAnimations();         

    if (millis() - updateTimer >= SERVO_UPDATE_TIME) {
        updateTimer = millis();

        unsigned long newTime = micros();
        float dt = (newTime - lastTime) / 1000000.0;  
        lastTime = newTime;

        manageServos(dt);
    }

    if (millis() - statusTimer >= STATUS_CHECK_TIME) {
        statusTimer = millis();

        #ifdef BAT_L
            checkBatteryLevel();
        #endif
    }
}

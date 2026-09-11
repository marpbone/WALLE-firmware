#ifndef ANIMATIONS_INO
#define ANIMATIONS_INO

void playAnimation(int animationNo) {

	switch (animationNo) {

		case 0:
			// --- Reset Servo positions ---
			//          time,head,necT,necB,eyeR,eyeL,armL,armR
			Serial.println("anim");
			queue.push({3500,  50,  10,  0,   0,   0,  0,  0});
			break;

		case 1:
			//          time,head,necT,necB,eyeR,eyeL,armL,armR
			Serial.println("SOUND");
			Serial.println("X animation");
			queue.push({ 500,  50,  10,  0,    0,   0,  60,  60});
			queue.push({ 500,  50,  10,  0,  100, 100,  60,  60});
			queue.push({ 500,  50,  10,  0,  100, 100,  40,  40});
			queue.push({ 500,  50,  10,  0,    0,   0,  40,  40});
			queue.push({ 500,  50,  10,  0,    0, 100,  60,  60});
			queue.push({ 500,  50,  10,  0,  100, 100,  60,  60});
			queue.push({ 500,  50,  10,  0,  100, 100,  40,  40});
			queue.push({   0,  50,  10,   0,   0,   0,  40,  40});
			break;

	case 2:
		Serial.println("SOUND");
		Serial.println("happy animation");
		queue.push({500, 50, 10, 20,  0,  0, 80,  0});
		queue.push({500, 50, 50,  0,  0,  0,  0, 80});
		queue.push({500, 50, 10, 20,  0,  0, 80,  0});
		queue.push({500, 50, 50,  0,  0,  0,  0, 80});
		queue.push({500, 50, 10, 20,  0,  0, 80,  0});
		queue.push({500, 50, 50,  0,  0,  0,  0, 80});
		queue.push({  0, 50, 10, 20,  0,  0, 80,  0});
		queue.push({  0,  50,  10,   0,   0,   0,  40,  40});
		break;

		case 3:
		Serial.println("SOUND");
		Serial.println("sad animation");
		queue.push({50, 50,   0, 10, 30, 30, 0, 0});
		queue.push({50, 50,   3, 10, 30, 30, 0, 0});
		queue.push({50, 50,   5, 10, 30, 30, 0, 0});
		queue.push({50, 50,   8, 10, 30, 30, 0, 0});
		queue.push({50, 50,  10, 10, 30, 30, 0, 0});
		queue.push({50, 50,  13, 10, 30, 30, 0, 0});
		queue.push({50, 50,  15, 10, 30, 30, 0, 0});
		queue.push({50, 50,  18, 10, 30, 30, 0, 0});
		queue.push({50, 50,  20, 10, 30, 30, 0, 0});
		queue.push({50, 50,  23, 10, 30, 30, 0, 0});
		queue.push({50, 50,  25, 10, 30, 30, 0, 0});
		queue.push({50, 50,  28, 10, 30, 30, 0, 0});
		queue.push({50, 50,  30, 10, 30, 30, 0, 0});
		queue.push({50, 50,  33, 10, 30, 30, 0, 0});
		queue.push({50, 50,  35, 10, 30, 30, 0, 0});
		queue.push({50, 50,  38, 10, 30, 30, 0, 0});
		queue.push({50, 50,  40, 10, 30, 30, 0, 0});
		queue.push({50, 50,  43, 10, 30, 30, 0, 0});
		queue.push({50, 50,  45, 10, 30, 30, 0, 0});
		queue.push({50, 50,  48, 10, 30, 30, 0, 0});
		queue.push({50, 50,  50, 10, 30, 30, 0, 0});
		queue.push({50, 50,  53, 10, 30, 30, 0, 0});
		queue.push({50, 50,  55, 10, 30, 30, 0, 0});
		queue.push({50, 50,  58, 10, 30, 30, 0, 0});
		queue.push({50, 50,  60, 10, 30, 30, 0, 0});
		queue.push({50, 50,  63, 10, 30, 30, 0, 0});
		queue.push({50, 50,  65, 10, 30, 30, 0, 0});
		queue.push({50, 50,  68, 10, 30, 30, 0, 0});
		queue.push({50, 50,  70, 10, 30, 30, 0, 0});
		queue.push({50, 50,  73, 10, 30, 30, 0, 0});
		queue.push({50, 50,  75, 10, 30, 30, 0, 0});
		queue.push({50, 50,  78, 10, 30, 30, 0, 0});
		queue.push({50, 50,  80, 10, 30, 30, 0, 0});
		queue.push({50, 50,  83, 10, 30, 30, 0, 0});
		queue.push({50, 50,  85, 10, 30, 30, 0, 0});
		queue.push({50, 50,  88, 10, 30, 30, 0, 0});
		queue.push({50, 50,  90, 10, 30, 30, 0, 0});
		queue.push({50, 50,  93, 10, 30, 30, 0, 0});
		queue.push({50, 50,  95, 10, 30, 30, 0, 0});
		queue.push({50, 50,  98, 10, 30, 30, 0, 0});
		queue.push({50, 50, 100, 10, 30, 30, 0, 0});
		queue.push({   0,  50,  10,   0,   0,   0,  40,  40});
		break;

		case 4:
		Serial.println("SOUND");
		Serial.println("fear animation");
		queue.push({ 50,   45,  10,   0,  30,   0,  45,  45});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});
		queue.push({ 50,   55,  10,   0,  30,   0,  35,  35});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});

		queue.push({ 50,   45,  10,   0,  30,   0,  45,  45});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});
		queue.push({ 50,   55,  10,   0,  30,   0,  35,  35});

		queue.push({ 50,   45,  10,   0,   0,  30,  45,  45});
		queue.push({ 50,   50,  10,  10,  30,   0,  40,  40});
		queue.push({ 50,   55,  10,   0,   0,  30,  35,  35});
		queue.push({ 50,   50,  10,  10,  30,   0,  40,  40});

		queue.push({ 50,   45,  10,   0,   0,  30,  45,  45});
		queue.push({ 50,   50,  10,  10,  30,   0,  40,  40});
		queue.push({ 50,   55,  10,   0,   0,  30,  35,  35});

		queue.push({ 50,   45,  10,   0,  30,   0,  45,  45});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});
		queue.push({ 50,   55,  10,   0,  30,   0,  35,  35});
		queue.push({ 50,   45,  10,   0,  30,   0,  45,  45});

		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});
		queue.push({ 50,   55,  10,   0,  30,   0,  35,  35});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});

		queue.push({ 50,   45,  10,   0,  30,   0,  45,  45});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});
		queue.push({ 50,   55,  10,   0,  30,   0,  35,  35});

		queue.push({ 50,   45,  10,   0,   0,  30,  45,  45});
		queue.push({ 50,   50,  10,  10,  30,   0,  40,  40});
		queue.push({ 50,   55,  10,   0,   0,  30,  35,  35});
		queue.push({ 50,   50,  10,  10,  30,   0,  40,  40});

		queue.push({ 50,   45,  10,   0,   0,  30,  45,  45});
		queue.push({ 50,   50,  10,  10,  30,   0,  40,  40});
		queue.push({ 50,   55,  10,   0,   0,  30,  35,  35});

		queue.push({ 50,   45,  10,   0,  30,   0,  45,  45});
		queue.push({ 50,   50,  10,  10,   0,  30,  40,  40});
		queue.push({ 50,   55,  10,   0,  30,   0,  35,  35});

		queue.push({   0,  50,  10,  10,   0,   0,  40,  40});
		break;

		case 5:
			//          time,head,necT,necB,eyeR,eyeL,armL,armR
			Serial.println("SOUND");
			Serial.println("idle animation");
			queue.push({ 500,  80,  10,  0,    0,   0,  40,  40});
			queue.push({ 500,  50,  10, 50,    0,   0,  40,  40});
			queue.push({ 500,  20,  10,  0,    0,   0,  40,  40});
			queue.push({ 500,  50,  10, 50,    0,   0,  40,  40});
			queue.push({ 500,  80,  10,  0,    0,   0,  40,  40});
			queue.push({ 500,  50,  10, 50,    0,   0,  40,  40});
			queue.push({ 500,  20,  10,  0,    0,   0,  40,  40});
			queue.push({   0,  50,  10,   0,   0,   0,  40,  40});
			break;

		case 6:
		Serial.println("SOUND");
		Serial.println("disgust animation");
		queue.push({3500, 100,  10,   0,   0,   0,  40,  40});
		queue.push({   0,  50,  10,   0,   0,   0,  40,  40});
		break;

		case 7:
		Serial.println("SOUND");
		Serial.println("sad animation 2");
		queue.push({  50,  50,  10,   0,   0,   0,  40,  40});
		queue.push({  50,  50,  10,   0,   0,   5,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  10,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  15,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  20,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  25,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  30,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  35,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  40,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  45,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  50,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  55,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  60,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  65,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  70,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  75,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  80,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  85,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  90,  40,  40});
		queue.push({  50,  50,  10,   0,   0,  95,  40,  40});
		queue.push({2500,  50,  10,   0,   0, 100,  40,  40});
		queue.push({   0,  50,  10,   0,   0,   0,  40,  40});
		break;

		case 8:
			//          time,head,necT,necB,eyeR,eyeL,armL,armR
			Serial.println("SOUND");
			Serial.println("surprised animation");
			queue.push({3500, 50,  35,  70,  100, 100,  100,  100});
			queue.push({   0,  50,  10,   0,   0,   0,  40,  40});
			break;

		case 9:
			//          time,head,necT,necB,eyeR,eyeL,armL,armR
			Serial.println("SOUND");
			Serial.println("disgusted animation");

			// ---- 50 → 20 ----
			queue.push({50, 50,10,10,10,10, 0,0});
			queue.push({50, 46,10,10,10,10, 0,0});
			queue.push({50, 42,10,10,10,10, 0,0});
			queue.push({50, 38,10,10,10,10, 0,0});
			queue.push({50, 34,10,10,10,10, 0,0});
			queue.push({50, 30,10,10,10,10, 0,0});
			queue.push({50, 26,10,10,10,10, 0,0});
			queue.push({50, 22,10,10,10,10, 0,0});
			queue.push({50, 20,10,10,10,10, 0,0});

			// ---- CYCLE 1 (20 → 80 → 20) ----
			queue.push({50, 24,10,10,10,10, 0,0});
			queue.push({50, 28,10,10,10,10, 0,0});
			queue.push({50, 32,10,10,10,10, 0,0});
			queue.push({50, 36,10,10,10,10, 0,0});
			queue.push({50, 40,10,10,10,10, 0,0});
			queue.push({50, 44,10,10,10,10, 0,0});
			queue.push({50, 48,10,10,10,10, 0,0});
			queue.push({50, 52,10,10,10,10, 0,0});
			queue.push({50, 56,10,10,10,10, 0,0});
			queue.push({50, 60,10,10,10,10, 0,0});
			queue.push({50, 64,10,10,10,10, 0,0});
			queue.push({50, 68,10,10,10,10, 0,0});
			queue.push({50, 72,10,10,10,10, 0,0});
			queue.push({50, 76,10,10,10,10, 0,0});
			queue.push({50, 80,10,10,10,10, 0,0});

			queue.push({50, 76,10,10,10,10, 0,0});
			queue.push({50, 72,10,10,10,10, 0,0});
			queue.push({50, 68,10,10,10,10, 0,0});
			queue.push({50, 64,10,10,10,10, 0,0});
			queue.push({50, 60,10,10,10,10, 0,0});
			queue.push({50, 56,10,10,10,10, 0,0});
			queue.push({50, 52,10,10,10,10, 0,0});
			queue.push({50, 48,10,10,10,10, 0,0});
			queue.push({50, 44,10,10,10,10, 0,0});
			queue.push({50, 40,10,10,10,10, 0,0});
			queue.push({50, 36,10,10,10,10, 0,0});
			queue.push({50, 32,10,10,10,10, 0,0});
			queue.push({50, 28,10,10,10,10, 0,0});
			queue.push({50, 24,10,10,10,10, 0,0});
			queue.push({50, 20,10,10,10,10, 0,0});

			// ---- CYCLE 2 ----
			queue.push({50, 24,10,10,10,10, 0,0});
			queue.push({50, 28,10,10,10,10, 0,0});
			queue.push({50, 32,10,10,10,10, 0,0});
			queue.push({50, 36,10,10,10,10, 0,0});
			queue.push({50, 40,10,10,10,10, 0,0});
			queue.push({50, 44,10,10,10,10, 0,0});
			queue.push({50, 48,10,10,10,10, 0,0});
			queue.push({50, 52,10,10,10,10, 0,0});
			queue.push({50, 56,10,10,10,10, 0,0});
			queue.push({50, 60,10,10,10,10, 0,0});
			queue.push({50, 64,10,10,10,10, 0,0});
			queue.push({50, 68,10,10,10,10, 0,0});
			queue.push({50, 72,10,10,10,10, 0,0});
			queue.push({50, 76,10,10,10,10, 0,0});
			queue.push({50, 80,10,10,10,10, 0,0});

			queue.push({50, 76,10,10,10,10, 0,0});
			queue.push({50, 72,10,10,10,10, 0,0});
			queue.push({50, 68,10,10,10,10, 0,0});
			queue.push({50, 64,10,10,10,10, 0,0});
			queue.push({50, 60,10,10,10,10, 0,0});
			queue.push({50, 56,10,10,10,10, 0,0});
			queue.push({50, 52,10,10,10,10, 0,0});
			queue.push({50, 48,10,10,10,10, 0,0});
			queue.push({50, 44,10,10,10,10, 0,0});
			queue.push({50, 40,10,10,10,10, 0,0});
			queue.push({50, 36,10,10,10,10, 0,0});
			queue.push({50, 32,10,10,10,10, 0,0});
			queue.push({50, 28,10,10,10,10, 0,0});
			queue.push({50, 24,10,10,10,10, 0,0});
			queue.push({50, 20,10,10,10,10, 0,0});

			// ---- CYCLE 3 ----
			queue.push({50, 24,10,10,10,10, 0,0});
			queue.push({50, 28,10,10,10,10, 0,0});
			queue.push({50, 32,10,10,10,10, 0,0});
			queue.push({50, 36,10,10,10,10, 0,0});
			queue.push({50, 40,10,10,10,10, 0,0});
			queue.push({50, 44,10,10,10,10, 0,0});
			queue.push({50, 48,10,10,10,10, 0,0});
			queue.push({50, 52,10,10,10,10, 0,0});
			queue.push({50, 56,10,10,10,10, 0,0});
			queue.push({50, 60,10,10,10,10, 0,0});
			queue.push({50, 64,10,10,10,10, 0,0});
			queue.push({50, 68,10,10,10,10, 0,0});
			queue.push({50, 72,10,10,10,10, 0,0});
			queue.push({50, 76,10,10,10,10, 0,0});
			queue.push({50, 80,10,10,10,10, 0,0});

			queue.push({50, 76,10,10,10,10, 0,0});
			queue.push({50, 72,10,10,10,10, 0,0});
			queue.push({50, 68,10,10,10,10, 0,0});
			queue.push({50, 64,10,10,10,10, 0,0});
			queue.push({50, 60,10,10,10,10, 0,0});
			queue.push({50, 56,10,10,10,10, 0,0});
			queue.push({50, 50,10,10,10,10, 0,0});


			// instant arm drop + head center
			queue.push({50,  50,10,10,10,10,  0,  0});

			break;



		default:
			// Serial.println(F("Invalid animation requested"));
			break;
	}


}

#endif /* ANIMATIONS_INO */

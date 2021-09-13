#include "include/CameraSlider.hpp"

// Until now only fake pins in order to dont cause an error at compiling
#define MOTOR_1_PIN_1 1
#define MOTOR_1_PIN_2 2
#define MOTOR_1_PIN_3 3
#define MOTOR_1_PIN_4 4

#define MOTOR_2_PIN_1 1
#define MOTOR_2_PIN_2 2
#define MOTOR_2_PIN_3 3
#define MOTOR_2_PIN_4 4

#define MOTOR_3_PIN_1 1
#define MOTOR_3_PIN_2 2
#define MOTOR_3_PIN_3 3
#define MOTOR_3_PIN_4 4

CameraSlider slider(MOTOR_1_PIN_1, MOTOR_1_PIN_2, MOTOR_1_PIN_3, MOTOR_1_PIN_4, 
					MOTOR_2_PIN_1, MOTOR_2_PIN_2, MOTOR_2_PIN_3, MOTOR_2_PIN_4, 
					MOTOR_3_PIN_1, MOTOR_3_PIN_2, MOTOR_3_PIN_3, MOTOR_3_PIN_4);

void setup() {
	Serial.begin(9600);
	slider.setMaxSpeed(500);
}

void loop() {

	slider.run();
}

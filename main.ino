#include <AccelStepper.h>

// Until now only fake pins in order to dont cause an error at compiling
#define MOTOR_PIN_1 1
#define MOTOR_PIN_2 2
#define MOTOR_PIN_3 3
#define MOTOR_PIN_4 4

AccelStepper stepper(64, MOTOR_PIN_1, MOTOR_PIN_2, MOTOR_PIN_3, MOTOR_PIN_4);

void setup() {
	stepper.setMaxSpeed(500);
	//stepper.moveTo
}

void loop() {

}

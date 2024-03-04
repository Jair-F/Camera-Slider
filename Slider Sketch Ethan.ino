//Stepper driver pin definitions
#define PIN_STEP1_ENABLE 0
#define PIN_STEP1_DIRECTION 1
#define PIN_STEP1_STEP 3

#define PIN_STEP2_ENABLE 2
#define PIN_STEP2_DIRECTION 6
#define PIN_STEP2_STEP 5

//Button pin definitions
#define PIN_BUTTON_1 46
#define PIN_BUTTON_2 47

#define JOYSTICK_BTN 4
#define START_SEQUENCE_BTN 11
#define PRINT_SERIAL_BTN 8

#define MAX_SPEED_NEMA 5000
#define MAX_SPEED_MOTOR_1 5000
#define MAX_SPEED_MOTOR_2 5000

//NEMA 23
int driverPUL = 9;    // PUL- pin
int driverDIR = 10;    // DIR- pin
int spd = A1;     // Potentiometer

int pd = 500;       // Pulse Delay period
boolean setdir = LOW; // Set Direction

int maxStepsPerSecond_NEMA23 = 400;
int maxStepsPerSecond_Motor_1 = 400;
int maxStepsPerSecond_Motor_2 = 400;


//Stepper driver variables
int stepsPerSecond_Motor_1 = 0;
int stepsPerSecond_Motor_2 = 0;
int currentPosition_Motor_1 = 0;
int Position_1_Motor_1 = 0;
int Position_2_Motor_1 = 0;
int currentPosition_Motor_2 = 0;
int Position_1_Motor_2 = 0;
int Position_2_Motor_2 = 0;

int stepsPerSecond_NEMA23 = 0;
int currentPosition_NEMA23 = 0;
int Position_1_NEMA23 = 0;
int Position_2_NEMA23 = 0;

long startSequenceTime;
long lastTimeButtonWasHigh;
long lastTimeStartSequenceButtonWasHigh;
long waitBeforeDirectionChange_Time;
int joyStickBtnValue = LOW; 
int startSequenceBtnValue = LOW; 
boolean joyStickBtn_Pressed_5_Sec = false;
boolean startSequenceBtn_Pressed_5_Sec = false;

boolean NEMA23_reachedTarget = true;
boolean Motor_1_reachedTarget = true;
boolean Motor_2_reachedTarget = false;

boolean NEMA23_isMaxStepPerSeconds_Set = false;
boolean Motor_1_isMaxStepPerSeconds_Set = false;
boolean Motor_2_isMaxStepPerSeconds_Set = false;

int axisCalibrationState = 0;

//For speed adjustment
int NEMA23_SpeedAdjustment = 1;
int Motor_1_SpeedAdjustment = 1;
int Motor_2_SpeedAdjustment = 1;


void setup()
{
   // initialize serial communication at 9600 bits per second:
    //Serial.begin(115200);
    //Set both buttons as INPUT_PULLUP so we can detect when they are pressed
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);

    pinMode(JOYSTICK_BTN, INPUT_PULLUP);
    pinMode(START_SEQUENCE_BTN, INPUT_PULLUP);
    pinMode(PRINT_SERIAL_BTN, INPUT_PULLUP);

    //Condigure stepper driver pins as OUTPUTs
    pinMode(PIN_STEP1_DIRECTION, OUTPUT);
    pinMode(PIN_STEP1_STEP, OUTPUT);
    pinMode(PIN_STEP1_ENABLE, OUTPUT);

    pinMode(PIN_STEP2_DIRECTION, OUTPUT);
    pinMode(PIN_STEP2_STEP, OUTPUT);
    pinMode(PIN_STEP2_ENABLE, OUTPUT);

    //ENABLE pin has to be pulled LOW for TCM2209 used in this example to enable the driver
    digitalWrite(PIN_STEP1_ENABLE, LOW);
    digitalWrite(PIN_STEP2_ENABLE, LOW);

    pinMode (driverPUL, OUTPUT);
  pinMode (driverDIR, OUTPUT);

   
}

void control_StepperMotor_1()
{
    static unsigned long nextAnalogRead = 0;

    if ((micros() > nextAnalogRead) && (!startSequenceBtn_Pressed_5_Sec)){
      
      int sensorValue = analogRead(A1);
      stepsPerSecond_Motor_1 = map(sensorValue, 0, 1023, -4000, 4000);
      nextAnalogRead = micros() + 3000;
      //Serial.println("\nRead poti ");
      //Serial.println(sensorValue);
      //Serial.println("\nSteps per second ");
      //Serial.println(stepsPerSecond_Motor_1);
    }
    if (((stepsPerSecond_Motor_1 < 1000) && (stepsPerSecond_Motor_1 > -1000))  && (!startSequenceBtn_Pressed_5_Sec)){
      //Serial.println("\n In Steps per second **********");
      stepsPerSecond_Motor_1 = 0;
    }

     //Serial.println("\n After In Steps per second **********");

   // stepsPerSecond_Motor_1 = 0;

   if((startSequenceBtn_Pressed_5_Sec == true) && (Position_2_Motor_1 < Position_1_Motor_1)){
      if (currentPosition_Motor_1 <= Position_2_Motor_1)
      {
        if (!Motor_1_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_1 = (maxStepsPerSecond_Motor_1);
          Motor_1_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_1 = 0;
        }
        
      }
      else if (currentPosition_Motor_1 >= Position_1_Motor_1)
      {
        if (!Motor_1_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_1 = -(maxStepsPerSecond_Motor_1);
          Motor_1_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_1 = 0;
        }
        
      }
    }
    else if((startSequenceBtn_Pressed_5_Sec == true) && (Position_2_Motor_1 > Position_1_Motor_1)){
      if (currentPosition_Motor_1 >= Position_2_Motor_1)
      {
        if (!Motor_1_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_1 = -(maxStepsPerSecond_Motor_1);
          Motor_1_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_1 = 0;
        }
        
      }
      else if (currentPosition_Motor_1 <= Position_1_Motor_1)
      {
        if (!Motor_1_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_1 = +(maxStepsPerSecond_Motor_1);
          Motor_1_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_1 = 0;
        }
        
      }
    }

    static unsigned long nextChange = 0;
    static uint8_t currentState = LOW;
    if (stepsPerSecond_Motor_1 == 0)
    {
       //Serial.println("\n In Steps per second  = 0");
        //if speed is 0, set the step pin to LOW to keep current position
        currentState = LOW;
        digitalWrite(PIN_STEP1_STEP, LOW);
    }
    else
    {
       
        //if stepsPerSecond_Motor_1 is not 0, then we need to calculate the next time to change the state of the driver
        if (micros() > nextChange)
        {

            //Generate steps
            if (currentState == LOW)
            {
                currentState = HIGH;
                nextChange = micros() + 30;

                if (stepsPerSecond_Motor_1 > 0)
                {
                    currentPosition_Motor_1++;
                }
                else if (stepsPerSecond_Motor_1 < 0)
                {
                    currentPosition_Motor_1--;
                }
            }
            else
            {
                currentState = LOW;
                nextChange = micros() + (1000 * abs(1000.0f / stepsPerSecond_Motor_1)) - 30;
            }

            //Set direction based on the sign of stepsPerSecond_Motor_1
            if (stepsPerSecond_Motor_1 > 0)
            {
                digitalWrite(PIN_STEP1_DIRECTION, LOW);
            }
            else
            {
                digitalWrite(PIN_STEP1_DIRECTION, HIGH);
            }

            //Write out the step pin
            digitalWrite(PIN_STEP1_STEP, currentState);
        }
    }
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void control_StepperMotor_2()
{
    static unsigned long nextAnalogRead = 0;

    if ((micros() > nextAnalogRead) && (!startSequenceBtn_Pressed_5_Sec)){
      
      int sensorValue = analogRead(A2);

      stepsPerSecond_Motor_2 = map(sensorValue, 0, 1023, -4000, 4000);
      
      nextAnalogRead = micros() + 3000;
      //Serial.println("\nRead poti ");
      //Serial.println(sensorValue);
      //Serial.println("\nSteps per second ");
      //Serial.println(stepsPerSecond_Motor_2);
    }
    if (((stepsPerSecond_Motor_2 < 1000) && (stepsPerSecond_Motor_2 > -1000)) && (!startSequenceBtn_Pressed_5_Sec)){
      //Serial.println("\n In Steps per second **********");
      stepsPerSecond_Motor_2 = 0;
    }

    //stepsPerSecond_Motor_2 = 8000;

    if((startSequenceBtn_Pressed_5_Sec == true) && (Position_2_Motor_2 < Position_1_Motor_2)){
      if (currentPosition_Motor_2 <= Position_2_Motor_2)
      {
        
        if (!Motor_2_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_2 = (maxStepsPerSecond_Motor_2);
          Motor_2_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_2 = 0;
        }
        
      }
      else if (currentPosition_Motor_2 >= Position_1_Motor_2)
      {
        if (!Motor_2_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_2 = -(maxStepsPerSecond_Motor_2);
          Motor_2_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_2 = 0;
        }
        
      }
    }
    else if((startSequenceBtn_Pressed_5_Sec == true) && (Position_2_Motor_2 > Position_1_Motor_2)){
      if (currentPosition_Motor_2 >= Position_2_Motor_2)
      {
        if (!Motor_2_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_2 = -(maxStepsPerSecond_Motor_2);
          Motor_2_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_2 = 0;
        }
        
      }
      else if (currentPosition_Motor_2 <= Position_1_Motor_2)
      {
        if (!Motor_2_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_Motor_2 = +(maxStepsPerSecond_Motor_2);
          Motor_2_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_Motor_2 = 0;
        }
        
      }
    }

    static unsigned long nextChange = 0;
    static uint8_t currentState = LOW;
    if (stepsPerSecond_Motor_2 == 0)
    {
        //if speed is 0, set the step pin to LOW to keep current position
        currentState = LOW;
        digitalWrite(PIN_STEP2_STEP, LOW);
    }
    else
    {
        //if stepsPerSecond_Motor_2 is not 0, then we need to calculate the next time to change the state of the driver
        if (micros() > nextChange)
        {

            //Generate steps
            if (currentState == LOW)
            {
                currentState = HIGH;
                nextChange = micros() + 30;

                if (stepsPerSecond_Motor_2 > 0)
                {
                    currentPosition_Motor_2++;
                }
                else if (stepsPerSecond_Motor_2 < 0)
                {
                    currentPosition_Motor_2--;
                }
            }
            else
            {
                currentState = LOW;
                nextChange = micros() + (1000 * abs(1000.0f / stepsPerSecond_Motor_2)) - 30;
            }

            //Set direction based on the sign of stepsPerSecond_Motor_2
            if (stepsPerSecond_Motor_2 > 0)
            {
                digitalWrite(PIN_STEP2_DIRECTION, LOW);
            }
            else
            {
                digitalWrite(PIN_STEP2_DIRECTION, HIGH);
            }

            //Write out the step pin
            digitalWrite(PIN_STEP2_STEP, currentState);
        }
    }
  
}

void control_NEMA23()
{
  //NEMA 23

    static unsigned long nextAnalogRead_NEMA23 = 0;

    if ((micros() > nextAnalogRead_NEMA23) && (!startSequenceBtn_Pressed_5_Sec)){
      //Serial.println("\nRead poti ");
      int sensorValue_NEMA23 = analogRead(A0);
      stepsPerSecond_NEMA23 = map(sensorValue_NEMA23, 0, 1023, -800, 800);
      nextAnalogRead_NEMA23 = micros() + 3000;
      //Serial.println("\nRead poti ");
      //Serial.println(sensorValue_NEMA23);
      //Serial.println("\nSteps per second ");
      //Serial.println(stepsPerSecond_NEMA23);
    }
    if (((stepsPerSecond_NEMA23 < 500) && (stepsPerSecond_NEMA23 > -500)) && (!startSequenceBtn_Pressed_5_Sec)) {
      stepsPerSecond_NEMA23 = 0;
    }
    //stepsPerSecond_NEMA23 = 1000;

    if(((startSequenceBtn_Pressed_5_Sec == true) || (NEMA23_isMaxStepPerSeconds_Set == true)) && (Position_2_NEMA23 < Position_1_NEMA23)){
      if (currentPosition_NEMA23 <= Position_2_NEMA23)
      {
        if (!NEMA23_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_NEMA23 = (maxStepsPerSecond_NEMA23);
          NEMA23_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_NEMA23 = 0;
        }
        
      }
      else if (currentPosition_NEMA23 >= Position_1_NEMA23)
      {
        if (!NEMA23_isMaxStepPerSeconds_Set)
        {
           stepsPerSecond_NEMA23 = -(maxStepsPerSecond_NEMA23);
          NEMA23_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_NEMA23 = 0;
        }
       
      }else
      {
        waitBeforeDirectionChange_Time = millis();
      }
    }
    else if(((startSequenceBtn_Pressed_5_Sec == true)|| (NEMA23_isMaxStepPerSeconds_Set == true)) && (Position_2_NEMA23 > Position_1_NEMA23)){
      if (currentPosition_NEMA23 >= Position_2_NEMA23)
      {
        if (!NEMA23_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_NEMA23 = -(maxStepsPerSecond_NEMA23);
          NEMA23_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_NEMA23 = 0;
        }
        
      }
      else if (currentPosition_NEMA23 <= Position_1_NEMA23)
      {
         if (!NEMA23_isMaxStepPerSeconds_Set)
        {
          stepsPerSecond_NEMA23 = +(maxStepsPerSecond_NEMA23);
          NEMA23_isMaxStepPerSeconds_Set = true;
        }
        else
        {
          stepsPerSecond_NEMA23 = 0;
        }
        
      }
    }
    
    static unsigned long nextChange_NEMA23 = 0;
    static uint8_t currentState_NEMA23 = LOW;
    if (stepsPerSecond_NEMA23 == 0)
    {
        //if speed is 0, set the step pin to LOW to keep current position
        currentState_NEMA23 = LOW;
        digitalWrite(driverPUL, LOW);
    }
    else
    {
        //if stepsPerSecond is not 0, then we need to calculate the next time to change the state of the driver
        if (micros() > nextChange_NEMA23)
        {

            //Generate steps
            if (currentState_NEMA23 == LOW)
            {
                currentState_NEMA23 = HIGH;
                nextChange_NEMA23 = micros() + 30;

                if (stepsPerSecond_NEMA23 > 0)
                {
                    currentPosition_NEMA23++;
                }
                else if (stepsPerSecond_NEMA23 < 0)
                {
                    currentPosition_NEMA23--;
                }
            }
            else
            {
                currentState_NEMA23 = LOW;
                nextChange_NEMA23 = micros() + (1000 * abs(1000.0f / stepsPerSecond_NEMA23)) - 30;
            }

            //Set direction based on the sign of stepsPerSecond
            if (stepsPerSecond_NEMA23 > 0)
            {
                digitalWrite(driverDIR, LOW);
                //digitalWrite(PIN_STEP1_DIRECTION_MOTOR_2, LOW);
               // stepCounter--;
            }
            else
            {
                digitalWrite(driverDIR, HIGH);
               // digitalWrite(PIN_STEP1_DIRECTION_MOTOR_2, HIGH);
               // stepCounter++;
            }

            //Write out the step pin
            digitalWrite(driverPUL, currentState_NEMA23);
           // digitalWrite(PIN_STEP1_STEP_MOTOR_2, currentState);
           
           
        }
    }
  
}

void joyStickBtn_Pressed_5_Sec_function()
{

  joyStickBtnValue = digitalRead(JOYSTICK_BTN);
   if (joyStickBtnValue == HIGH) {   // assumes LOW when pressed and HIGH when not-pressed
       lastTimeButtonWasHigh = millis();
       joyStickBtn_Pressed_5_Sec = false;
   }

   if (millis() - lastTimeButtonWasHigh >= 5000 /*requiredTime*/) {
       // input has been LOW throughout the required time
       // do whatever needs to be done
      joyStickBtn_Pressed_5_Sec = true;
       
   }
 
}

void startSequenceBtn_Pressed_5_Sec_function()
{

  startSequenceBtnValue = digitalRead(START_SEQUENCE_BTN);
   if (startSequenceBtnValue == LOW) {   // assumes LOW when pressed and HIGH when not-pressed
       lastTimeStartSequenceButtonWasHigh = millis();
       startSequenceBtn_Pressed_5_Sec = false;
       NEMA23_isMaxStepPerSeconds_Set = false;
       Motor_1_isMaxStepPerSeconds_Set = false;
       Motor_2_isMaxStepPerSeconds_Set = false;
   }

   if ((startSequenceBtnValue == LOW) && (stepsPerSecond_NEMA23 == 0) && (stepsPerSecond_Motor_1 == 0) && (stepsPerSecond_Motor_2 == 0))
   {
    //startSequenceBtn_Pressed_5_Sec = false;
   }

   if (millis() - lastTimeStartSequenceButtonWasHigh >= 3000 /*requiredTime*/) {
       // input has been LOW throughout the required time
       // do whatever needs to be done
      startSequenceBtn_Pressed_5_Sec = true;
      
       
   }

 
}

void takeAxisPosition()
{
  /*
   Serial.println("\nAxis calibrate state: ");
   Serial.println(axisCalibrationState);
   Serial.println("\ncurrentPosition_Motor_1: ");
   Serial.println(currentPosition_Motor_1);
   Serial.println("\ncurrentPosition_Motor_2: ");
   Serial.println(currentPosition_Motor_2);
   Serial.println("\ncurrentPosition_NEMA23: ");
   Serial.println(currentPosition_NEMA23);
   Serial.println("\nPosition_1_Motor_1: ");
   Serial.println(Position_1_Motor_1);
   Serial.println("\nPosition_1_Motor_2: ");
   Serial.println(Position_1_Motor_2);
   Serial.println("\nPosition_1_NEMA23: ");
   Serial.println(Position_1_NEMA23);
   Serial.println("\nPosition_2_Motor_1: ");
   Serial.println(Position_2_Motor_1);
   Serial.println("\nPosition_2_Motor_2: ");
   Serial.println(Position_2_Motor_2);
   Serial.println("\nPosition_2_NEMA23: ");
   Serial.println(Position_2_NEMA23);
   */
   

switch (axisCalibrationState) {
  //To start the axis calibration, we wait till the button on the joystick was pressed 5 seconds
  //This is the first position for the axis
  //Set all current axis postions to zero
  case 0:
    // statements
    if (joyStickBtn_Pressed_5_Sec == true) {
      joyStickBtn_Pressed_5_Sec = false;
      lastTimeButtonWasHigh = millis();
      
      axisCalibrationState = 1;
      currentPosition_Motor_1 = 0;
      currentPosition_Motor_2 = 0;
      currentPosition_NEMA23 = 0; 
      Position_1_Motor_1 = 0;
      Position_1_Motor_2 = 0;
      Position_1_NEMA23 = 0;
    }
    break;

  //Drive to the second position and press the joystick button again for 5 seconds
  case 1:
    // statements
    if (joyStickBtn_Pressed_5_Sec == true) {
      joyStickBtn_Pressed_5_Sec = false;
      lastTimeButtonWasHigh = millis();
      
      axisCalibrationState = 2;
      Position_2_Motor_1 = currentPosition_Motor_1;
      Position_2_Motor_2 = currentPosition_Motor_2;
      Position_2_NEMA23 = currentPosition_NEMA23;
    }
    
    break;
  
  case 2:

    break;
  default:
    // statements
    break;
}
  
}

void calculateRequestedSpeed()
{
   int analogRead_Speed = analogRead(A3);
   int requested_Speed = map(analogRead_Speed, 0, 1023, 1, 25);

   //NEMA 23
   if (Position_1_NEMA23 > Position_2_NEMA23) 
   {
     maxStepsPerSecond_NEMA23 = (((Position_1_NEMA23 - Position_2_NEMA23)/requested_Speed) * NEMA23_SpeedAdjustment);// map(requested_Speed, 0, 1023, 0, 400);
   }else
   {
    maxStepsPerSecond_NEMA23 = (((Position_2_NEMA23 - Position_1_NEMA23)/requested_Speed) * NEMA23_SpeedAdjustment);
   }

   if (maxStepsPerSecond_NEMA23 > MAX_SPEED_NEMA)
   {
    maxStepsPerSecond_NEMA23 = MAX_SPEED_NEMA;
   }

   //Motor 1
   if (Position_1_Motor_1 > Position_2_Motor_1) 
   {
     maxStepsPerSecond_Motor_1 = (((Position_1_Motor_1 - Position_2_Motor_1)/requested_Speed) * Motor_1_SpeedAdjustment);// map(requested_Speed, 0, 1023, 0, 400);
   }else
   {
    maxStepsPerSecond_Motor_1 = (((Position_2_Motor_1 - Position_1_Motor_1)/requested_Speed) * Motor_1_SpeedAdjustment);
   }

   if (maxStepsPerSecond_Motor_1 > MAX_SPEED_MOTOR_1)
   {
    maxStepsPerSecond_Motor_1 = MAX_SPEED_MOTOR_1;
   }

    //Motor 2
   if (Position_1_Motor_2 > Position_2_Motor_2) 
   {
     maxStepsPerSecond_Motor_2 = (((Position_1_Motor_2 - Position_2_Motor_2)/requested_Speed) * Motor_2_SpeedAdjustment);// map(requested_Speed, 0, 1023, 0, 400);
   }else
   {
    maxStepsPerSecond_Motor_2 = (((Position_2_Motor_2 - Position_2_Motor_1)/requested_Speed) * Motor_2_SpeedAdjustment);
   }

   if (maxStepsPerSecond_Motor_2 > MAX_SPEED_MOTOR_2)
   {
    maxStepsPerSecond_Motor_2 = MAX_SPEED_MOTOR_2;
   }
  
   //maxStepsPerSecond_Motor_1 = map(requested_Speed, 0, 1023, 0, 400);
   //maxStepsPerSecond_Motor_2 = map(requested_Speed, 0, 1023, 0, 400);

}

void printSerial_Positions()
{
  if (startSequenceBtn_Pressed_5_Sec)
  {
    
   int tmp1 = abs(Position_1_Motor_1-Position_2_Motor_1);
   //Serial.println("\nPosition_NEMA23: ");
   Serial.println(tmp1);
   
   int tmp2 = abs(Position_1_Motor_1-Position_2_Motor_1);
   Serial.println("\nPosition_Motor_1: ");
   Serial.println(tmp2);

   int tmp3 = abs(Position_1_Motor_2-Position_2_Motor_2);
   Serial.println("\nPosition_Motor_2: ");
   Serial.println(tmp3);
  }
}



///////////end of control_StepperMotor_2()

void loop()
{
  calculateRequestedSpeed();
  control_NEMA23();
  control_StepperMotor_1();
  control_StepperMotor_2();
  joyStickBtn_Pressed_5_Sec_function();
  startSequenceBtn_Pressed_5_Sec_function();
  takeAxisPosition();

  /*
  if (startSequenceBtnValue)
  {
    Serial.println("\nAxis calibrate state: ");
   Serial.println(axisCalibrationState);
   Serial.println("\ncurrentPosition_Motor_1: ");
   Serial.println(currentPosition_Motor_1);
   Serial.println("\ncurrentPosition_Motor_2: ");
   Serial.println(currentPosition_Motor_2);
   Serial.println("\ncurrentPosition_NEMA23: ");
   Serial.println(currentPosition_NEMA23);
    //printSerial_Positions(); 
  }
  */
 
  
    
}

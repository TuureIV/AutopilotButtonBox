/*
This 
*/
#include <Arduino.h>
#include <Joystick.h>
#include "CustomRotaryEncoder.h"
#include <Button.h>

#define button_1 18 //Pin
#define button_2 15 //Pin
#define button_3 14 //Pin
#define button_4 16 //Pin
#define potentiometer_1 A10 //Pin
#define buttonCount 4

void chekButtonPress();
void setEncoderDirections();
int getAverageOutput(int pinToRead);
void checkPotentiometer();
enum MODE
{
    NOT_DEFINED = HIGH,
    SIM = LOW, // Switch ON (+)
    SYSTEM = HIGH // Switch OFF (-)
};

//Values for potentiometer

int ryAxis_ = 0;
const bool initAutoSendState = true;
int currentOutputLevel = 0;
const int numReadings = 20;
int readings[numReadings];      // the readings from the analog input
int index = 0;              // the index of the current reading
int total = 0;                  // the running total

CustomRotaryEncoder encoders[3] = {
    CustomRotaryEncoder(8, 9, 21, 2), //  Every encoder produces 3 joystick buttons
    CustomRotaryEncoder(5, 6, 7, 5),
    CustomRotaryEncoder(2, 3, 4, 8) 
   
};

Button buttons[buttonCount] = {
    Button (button_1,1),
    Button (button_2,2),
    Button (button_3,3),
    Button (button_4,4)
};

Joystick_ Joystick( JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD, 
                    14, //num off buttons on joystick list
                    0, // hat switch count
                    false,		//l includeXAxis = true,
                    false,
                    false,
                    false,
                    true,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false
                    );

int getEncoderCount()
{
    return sizeof(encoders)/sizeof(encoders[0]);
}

MODE mode = NOT_DEFINED;
unsigned long previousStatusTime = 0;
//int statusInterval = 30 * 1000; // Every 10 seconds


// the setup function runs once when you press reset or power the board
void setup()   
{      

    //rotarynapit
    pinMode(4, INPUT_PULLUP); 
    pinMode(7, INPUT_PULLUP); 
    pinMode(10, INPUT_PULLUP); 
    
    Serial.begin(57600);
    Joystick.begin(true);
    for (int thisReading = 0; thisReading < numReadings; thisReading++)
    {
    readings[thisReading] = 0;
    }
}

// the loop function runs over and over again forever
void loop()
{
 
    setEncoderDirections();
    chekButtonPress();
    checkPotentiometer();
    delay(10); // Just some delay so we dont run too fast :)
}

void setEncoderDirections()
{
    for (int i = 0; i < getEncoderCount(); i++)
    {
        encoders[i].tick();
        encoders[i].myTick();

        RotaryEncoder::Direction direction = encoders[i].getDirection();
        if (direction != RotaryEncoder::Direction::NOROTATION)
        {
            ENCODER_ACTION action = direction == RotaryEncoder::Direction::CLOCKWISE ? ENCODER_ACTION::INCREASE : ENCODER_ACTION::DECREASE;
            uint8_t button = encoders[i].getButtonIndex(mode == SIM, action);
            Joystick.pressButton(button);
            delay(100); // Debug
            Joystick.releaseButton(button);

            Serial.print("Encoder action: ");
            Serial.println(action);
        }

        if (  encoders[i].getStateChanged() )
        {
            uint8_t button = encoders[i].getButtonIndex(mode == SIM, ENCODER_ACTION::CLICK);
            if (  encoders[i].getIsPressed() )
            {
                
                Joystick.pressButton(button);
            }
            else
                Joystick.releaseButton(button);
        }
            
    }
}

void chekButtonPress()
{
    
    for (int i = 0; i <buttonCount; i++)
    {   
        buttons[i].ButtonPress();

        if ( buttons[i].GetBtnStateChanged() )
        {
            uint8_t btn = buttons[i].GetBtnOrder();
            if (buttons[i].GetBtnPressed())
            {
                Joystick.pressButton(btn);
            }
            else
                Joystick.releaseButton(btn);
        
	    }
    }

}     

void checkPotentiometer()
{
    currentOutputLevel = getAverageOutput(potentiometer_1);
    Joystick.setRyAxis(currentOutputLevel); 
}


int getAverageOutput(int pinToRead){
  index = 0;
  total = 0; 
 
  while (index < numReadings){
    readings[index] = analogRead(pinToRead);
    total = total + readings[index];
    index = index + 1;
    //delay (1);
  }
  return total / numReadings;
}
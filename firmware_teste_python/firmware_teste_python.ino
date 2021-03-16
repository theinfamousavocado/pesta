#include <CmdMessenger.h>
#define irleft A0
#define irright A1
#define irfront A2
#define irback A3 
// Encoder output to Arduino Interrupt pin
#define ENC_IN1L 2
#define ENC_IN2L 30
#define ENC_IN1R 3
#define ENC_IN2R 32

const int IN1=22;
const int IN2=24;
const int ENA=6;
const int IN3=26;
const int IN4=28;
const int ENB=9;
int right_motor_direction_flag = 0;
int left_motor_direction_flag = 0;

// Pulse count from encoder
volatile float encoderValueL = 0;
volatile float encoderValueR = 0; 
// One-second interval for measurements
int interval = 500;
float l_m_s, r_m_s ;// variáveis que vão receber o valor de velocidade dos motores
// Counters for milliseconds during interval
float previousMillis = 0;
float currentMillis = 0;
 
CmdMessenger cmdMessenger(Serial);
  
enum
{
  // Commands
  kAcknowledge         , // Command to acknowledge that cmd was received
  kError               , // Command to report errors
  kGetVelocities       , // Command to request add two floats
  kSendDataSensors , // Command to report addition result
};

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kGetVelocities, OnGetVelocities);
  cmdMessenger.attach(kSendDataSensors, OnSendDataSensors);
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kError,"Command without attached callback");
}

// Callback function that responds that Arduino is ready (has booted up)
void OnArduinoReady()
{
  cmdMessenger.sendCmd(kAcknowledge,"Arduino ready");
}
 
void OnGetVelocities()
{
  // Retreive first parameter as float
  l_m_s = cmdMessenger.readFloatArg();
  
  // Retreive second parameter as float
  r_m_s = cmdMessenger.readFloatArg();
}
void OnSendDataSensors(){
     cmdMessenger.sendCmdStart(kSendDataSensors);  
     //cmdMessenger.sendCmdBinArg((float)seconds);                              
     cmdMessenger.sendCmdBinArg((float)encoderValueL);                
     cmdMessenger.sendCmdBinArg((float)encoderValueR);                   
     cmdMessenger.sendCmdBinArg((float)getRange(irleft));
     cmdMessenger.sendCmdBinArg((float)getRange(irright));
     cmdMessenger.sendCmdBinArg((float)getRange(irfront));
     cmdMessenger.sendCmdBinArg((float)getRange(irback));                        
     cmdMessenger.sendCmdEnd();  
} 

void setup()
{
  Serial.begin(115200);
   pinMode(ENC_IN1L, INPUT_PULLUP);
   pinMode(ENC_IN2L, INPUT_PULLUP);
   pinMode(ENC_IN1R, INPUT_PULLUP);
   pinMode(ENC_IN2R, INPUT_PULLUP); 
  // Attach interrupt 
  attachInterrupt(digitalPinToInterrupt(ENC_IN1L), updateEncoderL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_IN1R), updateEncoderR, RISING);
  // Setup initial values for timer
  previousMillis = millis();
  pinMode(IN1, OUTPUT);    
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

   // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Send the status to the PC that says the Arduino has booted
  cmdMessenger.sendCmd(kAcknowledge,"Arduino has started!");
}

void updateEncoderL()
{
 if (digitalRead(ENC_IN1L) == HIGH) {
    if (digitalRead(ENC_IN2L) == LOW) {
      encoderValueL++;
    } else {
      encoderValueL--;
    }
  } else {
    if (digitalRead(ENC_IN2L) == LOW) {
      encoderValueL--;
    } else {
      encoderValueL++;
    }
  }
}

void updateEncoderR()
{
 if (digitalRead(ENC_IN1R) == HIGH) {
    if (digitalRead(ENC_IN2R) == LOW) {
      encoderValueR++;
    } else {
      encoderValueR--;
    }
  } else {
    if (digitalRead(ENC_IN2R) == LOW) {
      encoderValueR--;
    } else {
      encoderValueR++;
    }
  }
}

float getRange(int num){
    float sample;
    float sample2;
    float sample3;
    // Get data
    sample=analogRead(num);    
    sample2= sample*0.0049;
    // Magic numbers to get cm
    sample3= 29.988*pow(sample2, -1.173);
    return sample3; //convert to meters
}


void Move_Robot(int motor_speed_left,int motor_speed_right)
{


     // For forward motion
     if(motor_speed_left > 0 && motor_speed_left <= 255)
     {

        //Setting encoder direction flag
        left_motor_direction_flag = 1;
      
        analogWrite(ENA, motor_speed_left);//Sets speed variable via PWM 
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
     }

     if(motor_speed_right > 0 && motor_speed_right <= 255)
     {

        //Setting encoder direction flag
        right_motor_direction_flag = 1;
        
        analogWrite(ENB, motor_speed_right);//Sets speed variable via PWM 
        
        digitalWrite(IN3, HIGH); //The two motors are rotating in direction, so here the signal is inversesd than the first
        digitalWrite(IN4, LOW);
     }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       // For reverse motion


     if(motor_speed_left < 0 && motor_speed_left >= -255 )
  
     {

         //Setting encoder direction flag
        left_motor_direction_flag = -1;
       
        analogWrite(ENA, abs(motor_speed_left));//Taking abs
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
     }

     if(motor_speed_right < 0 && motor_speed_right >= -255)
     {

        //Setting encoder direction flag
        right_motor_direction_flag = -1;
      
        analogWrite(ENB, abs(motor_speed_right));//Sets speed variable via PWM 
        
        digitalWrite(IN3, LOW); //The two motors are rotating in direction, so here the signal is inversesd than the first
        digitalWrite(IN4, HIGH);
     }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       // For Stopping the robot


     if(motor_speed_left == 0 )
     {
        analogWrite(ENA, motor_speed_left);//Sets speed variable via PWM 
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
     }

     if(motor_speed_right == 0)
     {
        analogWrite(ENB, motor_speed_right);//Sets speed variable via PWM 
        
        digitalWrite(IN3, HIGH); //The two motors are rotating in direction, so here the signal is inversesd than the first
        digitalWrite(IN4, HIGH);
     }
}
     


void loop()
{ 
  currentMillis = millis();
  cmdMessenger.feedinSerialData();
  Move_Robot(l_m_s, r_m_s);  
} 



 

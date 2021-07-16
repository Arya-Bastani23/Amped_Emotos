//Maxon EPOS2 controller

//Include files
#include <mcp_can.h> // CAN_BUS_SHIELD lib from SeeedStudio, make sure DEBUG_MODE = 0 as difined in mcp_can_dfs.h line 35, for best performance
#include <SPI.h>     // SPI interface with CAN shield
#include <SD.h>      // SD card library, also uses SPI, with other CS pin
#include <stdio.h>   // usefull C funstions

//Define datatypes
#define INT8U unsigned char
#define INT32U unsigned long

// Define Buttons
#define GREEN  A1
#define RED    A2
#define UP     A3
#define DOWN   A4

// Define leds 
#define LEDgreen 4
#define LEDred   5

// Define States
#define Startup        1
#define Homing         2
#define SetupPosCtrl   3
#define PosCtrl        4
#define Jog            5
#define Idle           6
#define Disabled       7

// Define Variables
INT8U Flag_Recv = 0;
INT8U len = 0;
INT8U buf[8];
INT32U ID=0;
INT32U NodeID=0x601;
INT32U SyncID=0x80;
char str[20];

const int chipSelect = 9;
File dataFile;

#define maxsize 81
char buffer[maxsize];    // make sure this is large enough for the largest string it must hold including NULL terminator
int i=0;

//define CAN datatypes
struct CANOpenData {
  byte datatype;
  word Index;
  byte SubIndex;
  long data;
} CANdata;

union CANdatabuf
    {
    CANOpenData data;
    byte bytes [8];
    };

struct CANOpen {
  word ID;
  CANdatabuf msg;
} CANOpenMsg;

struct CANOpenSetOperational {
  word ID;
  byte bytes[2];
} StartUp = {0x00, 0x01, 0x00};

struct CANOpenSync {
  word ID;
  byte bytes[2];
} Sync = {0x80, 0x00, 0x00};


//CAN messages
unsigned char STARTUP[2] =      {0x01, 0x00}; //CAN open message to change from pre-operational state to operational
unsigned char SYNC[2] =         {0x00, 0x00}; //CAN sync message, after which CANopen devices will respond synchronous,CAN busload can be controlled by Master, in this case its Arduino.

CANOpen ResetFaults = (CANOpen){0x601, (CANOpenData){0x2B, 0x6040, 0x00, 0x80}};
CANOpen HomingMode  = (CANOpen){0x601, (CANOpenData){0x2F, 0x6060, 0x00, 0x06}};
CANOpen PowerDown  =  (CANOpen){0x601, (CANOpenData){0x2B, 0x6040, 0x00, 0x06}};
CANOpen PowerOn  =    (CANOpen){0x601, (CANOpenData){0x2B, 0x6040, 0x00, 0x0F}};
CANOpen StartHoming = (CANOpen){0x601, (CANOpenData){0x2B, 0x6040, 0x00, 0x1F}};
CANOpen PositionMode =(CANOpen){0x601, (CANOpenData){0x2F, 0x6060, 0x00, 0x01}};
CANOpen ProfileSpeed =(CANOpen){0x601, (CANOpenData){0x23, 0x6081, 0x00, 8000}};
CANOpen PositionSet  =(CANOpen){0x601, (CANOpenData){0x23, 0x607A, 0x00, 0x00}};
CANOpen PositionDo  = (CANOpen){0x601, (CANOpenData){0x2B, 0x6040, 0x00, 0x3F}};

//Servodrive states
boolean Fault = false;
boolean PowerstageOn = false;
boolean TargetReached = false;
boolean HomingAttained = false;
boolean HomingError = false;
boolean Halt = false;
byte    ModeOfOperation = 0;

//timing variables
unsigned long t; //timeout within subroutines
unsigned long time; //Time in main loop
unsigned long t_prevCAN; //delta time calculation for CAN update loop
unsigned long t_prevBLINK; //delta time calculation for LED blinking
unsigned long t_timein; //start of wait for CAN receive
unsigned long t_timeout = 100; //time out value time for CAN receive message
int t_deltaCAN = 10; //update freq CAN messages.
int t_deltaBLINK = 250; //update freq LED blinking.
boolean LEDstate = false; //Initial LED state

//Statemachine State variable and initial value
byte State = Startup;


//****************************************************************
//    Setup routine
//    Is called automically on powerup, only once (Arduino keyword)
//****************************************************************

MCP_CAN CAN(3);

void setup()
{
  //set Leds as output
  pinMode(LEDgreen, OUTPUT); 
  pinMode(LEDred, OUTPUT);
  //During startup, RED = on, GReen = off 
  digitalWrite(LEDgreen, HIGH);
  digitalWrite(LEDred, LOW);
 
  //set buttons as inputs
  pinMode(GREEN,INPUT);
  pinMode(RED,INPUT);
  pinMode(UP,INPUT);
  pinMode(DOWN,INPUT);
  
  // Enable internal pull-ups for buttons, button will read 1 released, 0 for pressed
  digitalWrite(GREEN, HIGH);       
  digitalWrite(RED, HIGH);
  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);

  //setup serial communication for USB connection
  Serial.begin(115200);
  
  //initialise SD card
  Serial.println("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  else{
    Serial.println("card initialized.");
    // open the file 
    dataFile = SD.open("profiel.txt");
  }

  // open motion profile
  if (dataFile) {
  // if the file is open, succes!!
    Serial.println("profiel.txt opened");
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening profiel.txt");
  } 

  // init can bus, baudrate: 500k
  if(CAN.begin(CAN_500KBPS) ==CAN_OK) {
    Serial.print("can init ok!!\r\n");
    //initialise mask and filters to only recieve message 0x581 and 0x381
    CAN.init_Mask(0,0,0x7FF);//Mask0 all ID bits will be checked
    CAN.init_Mask(1,0,0x7FF);//Mask1 all ID bits will be checked
    
    CAN.init_Filt(0,0,0x581);//Filter 0, Mask0, will receive only this ID
    CAN.init_Filt(1,0,0x7FF);//Filter 1, Mask0, additional ID that is never used
    
    CAN.init_Filt(2,0,0x381);//Filter 2, Mask1, will receive only this ID
    CAN.init_Filt(3,0,0x7FF);//Filter 3, Mask1, additional ID that is never used
    CAN.init_Filt(4,0,0x7FF);//Filter 4, Mask1, additional ID that is never used
    CAN.init_Filt(5,0,0x7FF);//Filter 5, Mask1, additional ID that is never used
    delay(100);

  }
  else Serial.print("Can init fail!!\r\n");

  //Set initial statemachine value to Startup
  State = Startup;
  
}

//****************************************************************
//    Subroutines
//****************************************************************

//****************************************************************
//    Receive CAN data
//****************************************************************
void doCANreceive(void){
    //Wait for message
    t_timein = millis();
    while(CAN.checkReceive()==4){ // 3=Message available, 4 = No message available, as difined in mcp_can_dfs.h form CAN_BUS_SHIELD lib from SeeedStudio
        //Check for timeout
        if ((millis()-t_timein) > t_timeout) return;
        //Check button states?
        //Update leds?
    }
    
    if(Flag_Recv>=0)                   // check if get data via Interupt line
    {
      Flag_Recv = 0;                // clear flag
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      ID = CAN.getCanId();
      
      switch (ID) {
        case 0x381:
          Fault = bitRead(buf[0], 3);
          PowerstageOn = bitRead(buf[0], 4);
          Halt = bitRead(buf[0], 6);
          TargetReached = bitRead(buf[1], 2);
          HomingAttained = bitRead(buf[1], 4);
          HomingError = bitRead(buf[1], 5);
          if (Halt) State = Disabled;
          break;
        case 0x481:
          ModeOfOperation = buf[0];
          break;
        //default: 
          // if nothing else matches, do the default
          // default is optional
      }
    }

}


//****************************************************************
//    Read SD card data
//    Return Value was [LF]
//***************************************************************
long readFileString (char *strArray) {
  long Value;
    while (dataFile.available()){            
      strArray[i] = dataFile.read();
      if (strArray[i]==10){
        strArray[i]=0;
        i=0;
        Value=atol(strArray);
        return Value;
      }
      if (i!=(maxsize-1)){
        i++;
      }
    }
  return 0;
}



//****************************************************************
//    Check Buttons    
//****************************************************************
void CheckButtons(void){

    switch (State) {
      case Startup:
        break;
      case Homing:
        break;
      case SetupPosCtrl:
        break;
      case PosCtrl:
        if (digitalRead(RED) == 0) {
          State = Idle; //stop and hold current position
          if (dataFile) dataFile.seek(0); // reset to first value
        }
        break;
      case Jog:
        if ((digitalRead(UP) == 1) && (digitalRead(DOWN) == 1)) State = Idle; // All buttons released, return to Idle
        break;
      case Idle:
        if (digitalRead(GREEN) == 0) State = Homing; //allways start profile with homing routine
        if ((digitalRead(UP) == 0) || (digitalRead(DOWN) == 0)) State = Jog;
        break;
      case Disabled:
        if (!Halt) State = Startup; //allways start profile with homing routine
        break;
      }

}

//****************************************************************
//    Update Leds
//****************************************************************
void UpdateLeds(void){

    if ((time - t_prevBLINK) >= t_deltaBLINK){
       t_prevBLINK = time;
       LEDstate = !LEDstate;
    }
  
    switch (State) {
      case Startup:
         digitalWrite(LEDgreen, LOW);
         digitalWrite(LEDred, HIGH);
        break;
      case Homing:
         digitalWrite(LEDgreen, HIGH);
         digitalWrite(LEDred, HIGH);
        break;
      case SetupPosCtrl:
         digitalWrite(LEDgreen, LOW);
         digitalWrite(LEDred, HIGH);
        break;
      case PosCtrl:
         digitalWrite(LEDgreen, HIGH);
         digitalWrite(LEDred, LEDstate);//blinking RED
        break;
      case Jog:
         digitalWrite(LEDgreen, HIGH);
         digitalWrite(LEDred, LOW);//blinking RED
        break;
      case Idle:
         digitalWrite(LEDgreen, LEDstate);
         digitalWrite(LEDred, LOW);//blinking RED
        break;
      case Disabled:
         digitalWrite(LEDgreen, LOW);//blinking RED
         digitalWrite(LEDred, HIGH);//blinking RED
        break;
      }

}


//****************************************************************
//    Startup Maxon servodrive
//****************************************************************
void doStartup(void){
//Setoperational
  CAN.sendMsgBuf(0x00, 0, 2, STARTUP);
  delay(500);
//Check if Red button is pressed, if so State will be Disabled
  CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
  doCANreceive(); // wait for 381, Fault == 0;
  if (Halt) return;
//Reset Faults
  CAN.sendMsgBuf(ResetFaults.ID, 0, 8, ResetFaults.msg.bytes);
//Recieve CAN, wait for Fault = 0, check Timout, UpdateLeds?, CheckButtons?
  doCANreceive(); // wait for 581
//Recieve CAN, wait for Fault = 0, check Timout, UpdateLeds?, CheckButtons?
  Fault = 1;
  t = millis();
  while (Fault){
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 381, Fault == 0;
    if ((millis()-t) > 1000) return; // if not within 1 second reset, start again.
  }
//Change State
  State = Homing;

}

//****************************************************************
//    Homing
//****************************************************************
void doHoming(void){

//Change Mask
  CAN.init_Filt(0,0,0x581); // responce from command
  CAN.init_Filt(2,0,0x481); // only wait for status message 0x481
  delay(10);  
//Select Homing Mode
  CAN.sendMsgBuf(HomingMode.ID, 0, 8, HomingMode.msg.bytes);
  doCANreceive(); // wait for 581
  t = millis();
  while (ModeOfOperation != 6){
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 481, ModeOfOperation == 6;
    if ((millis()-t) > 1000) {
      State = Startup;
      return; // if not within 10 second homed, start again.
    }
  }
//change MASK
  CAN.init_Filt(0,0,0x581); // responce from command
  CAN.init_Filt(2,0,0x381); // only wait for status message 0x381
  delay(10);  
//Powerdown
  CAN.sendMsgBuf(PowerDown.ID, 0, 8, PowerDown.msg.bytes);
  doCANreceive(); // wait for 581
  t = millis();
  while (PowerstageOn){
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 381, PowerstageOn == 0;
    if (digitalRead(RED) == 0){
      State = Idle;
      return;
    }
    if ((millis()-t) > 1000) {
      State = Startup;
      return; // if not within 10 second homed, start again.
    }
  }
//PowerOn
  CAN.sendMsgBuf(PowerOn.ID, 0, 8, PowerOn.msg.bytes);
  doCANreceive(); // wait for 581
  t = millis();
  while (!PowerstageOn){
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 381, PowerstageOn == 1;
    if ((millis()-t) > 1000) {
      State = Startup;
      return; // if not within 10 second homed, start again.
    }
  }
//Wait for power to settle
delay(1000);

//StartHoming
  CAN.sendMsgBuf(StartHoming.ID, 0, 8, StartHoming.msg.bytes);
  doCANreceive(); // wait for 581
  t = millis();
  HomingAttained = false;
  while (!HomingAttained){ //&& HomingTime < 10sec
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 381, TargetReached == 1  && HomingAttained == 1;
    if ((millis()-t) > 10000) {
      State = Startup;
      return; // if not within 10 second homed, start again.
    }
  }

//Change State
  State = SetupPosCtrl;

}

//****************************************************************
//    Setup Position Control
//****************************************************************
void setupPositionControl(void){
//Change Mask
  CAN.init_Filt(0,0,0x581); // responce from command
  CAN.init_Filt(2,0,0x481); // only wait for status message 0x481
  delay(10);  
//Select PositionControlMode
  CAN.sendMsgBuf(PositionMode.ID, 0, 8, PositionMode.msg.bytes);
  doCANreceive(); // wait for 581
  t = millis();
  while (ModeOfOperation != 1){
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 481, ModeOfOperation == 1;
    if ((millis()-t) > 1000) {
      State = Startup;
      return; // if not within 10 second homed, start again.
    }
  }
//change MASK
  CAN.init_Filt(0,0,0x581); // responce from command
  CAN.init_Filt(2,0,0x381); // only wait for status message 0x381
  delay(10);  
//SetProfileVelocity
  CAN.sendMsgBuf(ProfileSpeed.ID, 0, 8, ProfileSpeed.msg.bytes);
  doCANreceive(); // wait for 581
  delay(10);
//Change State
  if (dataFile) {
      dataFile.seek(0); // reset to first value
  }  

  //State = Idle; //after homing wait for user input
  State = PosCtrl; //after homing start profile
}

//****************************************************************
//    Play Profile
//****************************************************************
void doProfile(void){

    //Read setpoint form SD
      if (dataFile) {
        if (dataFile.available()) {
          PositionSet.msg.data.data = readFileString (buffer);
        }
        else{
          dataFile.seek(0); // reset to first value
          State = Homing; // Start from homing
        }
      }  
    
    //Set Setpoint
      CAN.sendMsgBuf(PositionSet.ID, 0, 8, PositionSet.msg.bytes);
      doCANreceive(); // wait for 581
      if (Halt) return;
    //Execute Setpoint
      CAN.sendMsgBuf(PositionDo.ID, 0, 8, PositionDo.msg.bytes);
      doCANreceive(); // wait for 581
      if (Halt) return;
    //Send Sync message
      CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
      doCANreceive(); // wait for 481, ModeOfOperation == 1;
      if (Halt) return;

}



//****************************************************************
//    Jog
//****************************************************************
void doJog(void){
  
   //Read Button states
      if (digitalRead(UP) == 0) {
          PositionSet.msg.data.data += 140; //increment about 8 deg/s
          if (PositionSet.msg.data.data > 110000) PositionSet.msg.data.data = 110000; 
        }
        if (digitalRead(DOWN) == 0) {
          PositionSet.msg.data.data -= 140; // decrement about 8 deg/s
          if (PositionSet.msg.data.data < 0) PositionSet.msg.data.data = 0; 
        }

   //Set Setpoint
      CAN.sendMsgBuf(PositionSet.ID, 0, 8, PositionSet.msg.bytes);
      doCANreceive(); // wait for 581
    //Execute Setpoint
      CAN.sendMsgBuf(PositionDo.ID, 0, 8, PositionDo.msg.bytes);
      doCANreceive(); // wait for 581
    //Send Sync message
      CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
      doCANreceive(); // wait for 481, ModeOfOperation == 1;
}

//****************************************************************
//    Jog
//****************************************************************
void doIdle(void){
  //Send Sync message, to check servodrive status
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 381, Status & Faults;
}

//****************************************************************
//    Jog
//****************************************************************
void doDisable(void){
//Disable drive (allready done via Big Red Button, but also software wise as precaution
    CAN.sendMsgBuf(PowerDown.ID, 0, 8, PowerDown.msg.bytes);
    doCANreceive(); // wait for 581
  //Send Sync message, to check servodrive status
    CAN.sendMsgBuf(Sync.ID, 0, 2, Sync.bytes);
    doCANreceive(); // wait for 381, Status & Faults;
}

//****************************************************************
//    Main loop, actual main program that loops indefinitely
//    
//****************************************************************

void loop()
{
  time = millis();

  CheckButtons();

  UpdateLeds();
  
  //if its time, send and receive CAN messages
  if ((time - t_prevCAN) >= t_deltaCAN){
    t_prevCAN = time;
    
    //Statemachine based on switch case
    switch (State) {
      case Startup:
        doStartup();
        break;
      case Homing:
        doHoming();
        break;
      case SetupPosCtrl:
        setupPositionControl();
        break;
      case PosCtrl:
        doProfile();
        break;
      case Jog:
        doJog();
        break;
      case Idle:
        doIdle();
        break;
      case Disabled:
        doIdle();
        break;
      }

  }//end of CAN send & receive

}//end of main loop

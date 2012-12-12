#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


// define new data types
typedef enum {
  MOUSE_MODE_READY,
  MOUSE_MODE_OBSTACLE_AVOIDING,
  MOUSE_MODE_LINE_FOLLOWING,
  MOUSE_MODE_COMBAT
} MouseMode;

typedef enum {
  MOUSE_STATUS_FORWARD,
  MOUSE_STATUS_REVERSE,
  MOUSE_STATUS_STOP,
  MOUSE_STATUS_TURNLEFT,
  MOUSE_STATUS_TURNRIGHT
} MouseStatus;

typedef enum {
  MOUSE_ACTION_FORWARD,
  MOUSE_ACTION_REVERSE,
  MOUSE_ACTION_STOP,
  MOUSE_ACTION_TURNLEFT,
  MOUSE_ACTION_TURNRIGHT,
  MOUSE_ACTION_TURNAROUND
} MouseAction;

typedef enum {
  MOTOR_LEFT,
  MOTOR_RIGHT 
} Motor;

typedef enum {
  MOTOR_STATUS_FORWARD,
  MOTOR_STATUS_REVERSE,
  MOTOR_STATUS_STOP
} MotorStatus;

typedef enum {
  MOTOR_ACTION_FORWARD,
  MOTOR_ACTION_REVERSE,
  MOTOR_ACTION_STOP
} MotorAction;

    
// declare functions
void ControlMotor(Motor motor, MotorAction action);

void Delay(int);

byte BitSet(byte Bit_position, byte Var_old);
byte BitClear(byte Bit_position, byte Var_old);

// constants for touch bar switches
// NB: we assume that PTAD2 and PTAD1 are connected to
//     front touch bars left and right, respectively
#define touchBarFrontLeft   PTAD_PTAD2
#define touchBarFrontRight  PTAD_PTAD1

// constants for IR sensors
// NB: we assume that PTAD7 and PTAD6 are connected to
//     front infrared sensors left and right, respectively
#define infraredFrontLeft   PTAD_PTAD7
#define infraredFrontRight  PTAD_PTAD6

// constants for motor control
#define s1  0x4444
#define s2  0x2222
#define f   0xFFFF


// global variables
MouseMode mouseMode;  // operation mode of mouse
MouseStatus mouseStatus; // status of mouse
MotorStatus leftMotor, rightMotor;  // status of left and right motors


void main(void)
{    
  EnableInterrupts;
  SOPT=0x00; //disable watchdog

  //--------------------------------------------------------
  // for motor control
  //--------------------------------------------------------
  TPM1SC=0b00001000; //setup timer1 for pwm on bus clk
  TPM1MOD=0x8888;     //set pwm period
  
  TPM1C2SC=0x28;     //setup ports f0-f3 for pwm
  TPM1C3SC=0x28;
  TPM1C4SC=0x28;
  TPM1C5SC=0x28;

  //--------------------------------------------------------
  // for KBI handling
  //--------------------------------------------------------
  PTDPE = 0xFF; // enable port D pullups for push button switch
  PTDDD = 0x00;	// set port D as input
    // Switch 3 and 4 are connected to port D3 and D2, respectively

  // KBI configuration
  KBI1SC = BitClear(1, KBI1SC); // KBIE=0 -> disable KBI
  KBI1PE = 0x60;	              // enable KBI function for pins 5 and 6 only
  KBI1SC = BitClear(0, KBI1SC);	// KBIMOD=0 -> select edge-only detection
  KBI1SC = BitSet(2, KBI1SC);   // KBACK=1 -> clear KBI flag 
  KBI1SC = BitSet(1, KBI1SC);   // KBIE=1 0> enable KBI 

  // initialise motor status
  leftMotor = MOTOR_STATUS_STOP;
  rightMotor = MOTOR_STATUS_STOP;

  // now mouse is ready for further operations
  mouseMode = MOUSE_MODE_READY;
  
  for(;;) {
    // do nothing here; justwaiting for interrupts  
  }    // loop forever
}

//--------------------------------------------------------
// Functioins for various mouse modes
//--------------------------------------------------------
// function to avoid obstacles
void AvoidObstacle() {
  mouseMode = MOUSE_MODE_OBSTACLE_AVOIDING;
  
  for (;;) {
    // first move forward
    ControlMouse(MOUSE_ACTION_FORWARD);

    // first, check the status of touch bars    
    if (!touchBarFrontLeft && !touchBarFrontRight) {
      // neither is touched (i.e., both the values are zero)
      
      // then check the status of IF sensors
      if (!infraredFrontLeft && !infraredFrontRight) {
      // neither is touched (i.e., both the values are zero)
      // then, back to the loop
        break;              
      }
      else if (infraredFrontLeft) {
      // left sensor detects; avoid left obstacle
        
      }
      else if (infraredFrontRight) {
      // right sensor detects; avoid right obstacle
        
      }
      else {
      // both sensors detect; avoid front obstacle
      ControlMouse(MOUSE_ACTION_STOP);
      ControlMouse(MOUSE_ACTION_REVERSE);
      ControlMouse(MOUSE_ACTION_TURNAROUND);	// 180 dgree turn  
      }
    }
    else if (touchBarFrontLeft) {
      // left bar is touched; avoid left obstacle
      ControlMouse(MOUSE_ACTION_STOP);
      ControlMouse(MOUSE_ACTION_REVERSE);
      ControlMouse(MOUSE_ACTION_TURNRIGHT);
    }
    else if (touchBarFrontRight) {
      // right bar is touched; avoid right obstacle
      ControlMouse(MOUSE_ACTION_STOP);
      ControlMouse(MOUSE_ACTION_REVERSE);
      ControlMouse(MOUSE_ACTION_TURNLEFT);
    }
    else {
      // both bars are touched; avoid front obstacle
      ControlMouse(MOUSE_ACTION_STOP);
      ControlMouse(MOUSE_ACTION_REVERSE);
      ControlMouse(MOUSE_ACTION_TURNAROUND);	// 180 dgree turn
    } 
  } // end of for() loop  
}

//--------------------------------------------------------
// Functions for mouse control
//--------------------------------------------------------
void ControlMouse(MouseAction action)
{
  switch (action) {
    case MOUSE_ACTION_FORWARD:
      if (leftMotor != MOTOR_STATUS_FORWARD) {
        ControlMotor(leftMotor, MOTOR_ACTION_FORWARD);
      }
      if (rightMotor != MOTOR_STATUS_FORWARD) {
        ControlMotor(rightMotor, MOTOR_ACTION_FORWARD);
      }
      if (mouseStatus != MOUSE_STATUS_FORWARD) {
        mouseStatus = MOUSE_STATUS_FORWARD;
      }
      break;
    case MOUSE_ACTION_REVERSE:
      if (leftMotor != MOTOR_STATUS_REVERSE) {
        ControlMotor(leftMotor, MOTOR_ACTION_REVERSE);
      }
      if (rightMotor != MOTOR_STATUS_REVERSE) {
        ControlMotor(rightMotor, MOTOR_ACTION_REVERSE);
      }
      if (mouseStatus != MOUSE_STATUS_REVERSE) {
        mouseStatus = MOUSE_STATUS_REVERSE;
      }
      break;
    case MOUSE_ACTION_STOP:
      if (leftMotor != MOTOR_STATUS_STOP) {
        ControlMotor(leftMotor, MOTOR_ACTION_STOP);
      }
      if (rightMotor != MOTOR_STATUS_STOP) {
        ControlMotor(rightMotor, MOTOR_ACTION_STOP);
      }
      if (mouseStatus != MOUSE_STATUS_STOP) {
        mouseStatus = MOUSE_STATUS_STOP;
      }
      break;      
  } // end of switch()
  
}

//--------------------------------------------------------
// Functions for motor control
//--------------------------------------------------------
void ControlMotor(Motor motor, MotorAction action)
{
  MotorStatus status;
  dword tpm1, tpm2;
    // holding values to be transferred to TPM registers (TPM1C2V/TPM1C3V or TPM1C4V/TPM1C5V)
  
  switch (action) {
    case MOTOR_ACTION_FORWARD:
      tpm1 = s1;
      tpm2 = f;
      status = MOTOR_STATUS_FORWARD;
      break;
    case MOTOR_ACTION_REVERSE:
      tpm1 = f;
      tpm2 = s1;
      status = MOTOR_STATUS_REVERSE;
      break;  
    case MOTOR_ACTION_STOP:
      tpm1 = f;
      tpm2 = f;
      status = MOTOR_STATUS_STOP;
      break;
  }
   
  if (motor == MOTOR_LEFT) {
    TPM1C2V=tpm1;
    TPM1C3V=tpm2;
    leftMotor = status;
  } else {
    TPM1C4V=tpm1;
    TPM1C5V=tpm2;
    rightMotor= status;
  }
}

//--------------------------------------------------------
// Functioins for KBI handling
//--------------------------------------------------------
// function to set a specific bit of an unsigned 8-bit interger
byte BitSet(byte Bit_position, byte Var_old)
{
        byte Var_new;
        Var_new = Var_old | (byte)(1<<Bit_position);
        return Var_new;
}

// function to clear a specific bit of an unsigned 8-bit interger
byte BitClear(byte Bit_position, byte Var_old)
{
        byte Var_new;
        Var_new = Var_old & (~(byte)(1<<Bit_position));
        return Var_new;        
}

//--------------------------------------------------------
// Interrupt service routines
//--------------------------------------------------------
interrupt VectorNumber_Vkeyboard1 void intSW3_4()
{   // for KBI
// interrupt is a keyword. 
// VectorNumber_Vkeyboard1 (22) is the interrupt source number for KBI module
    byte sw3, sw4;
    
  sw3 = PTDD & 0x08;
  sw4 = PTDD & 0x04;

  // simple FSM for motor status handling
  if (sw3 == 0) {
    // for left motor
    switch (leftMotor) {
      case MOTOR_STATUS_STOP:
        ControlMotor(MOTOR_LEFT, MOTOR_ACTION_FORWARD);
        break;
      case MOTOR_STATUS_FORWARD:
        ControlMotor(MOTOR_LEFT, MOTOR_ACTION_REVERSE);
        break;
      case MOTOR_STATUS_REVERSE:
        ControlMotor(MOTOR_LEFT, MOTOR_ACTION_STOP);
        break;
    }
  } else if (sw4 == 0) {
    // for right motor
    switch (rightMotor) {
      case MOTOR_STATUS_STOP:
        ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_FORWARD);
        break;
      case MOTOR_STATUS_FORWARD:
        ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_REVERSE);
        break;
      case MOTOR_STATUS_REVERSE:
        ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_STOP);
        break;
    }
  }
  
  // 'braking' hasn't been implemented yet
   
  KBI1SC_KBACK = 1; // clear KBI interrupt flag
}

// ISR to provide distance feedback to motor control functions;
// currently not based on tacho meter but based on simple counter
// (i.e., time-based)
interrupt VectorNumber_Vtpm1ovf void intTPM1OVF()
{
  // clear TPM1 overflow
  
  // check motor statuses
  
  // check distance variable and reduce it if not zeroS  
}

//--------------------------------------------------------
// Misc. functions
//--------------------------------------------------------
void Delay(int a){
     int b=0,c=0;
     for (b=0;b<a;b++){
          for(c=0;c<100;c++);
     }
}
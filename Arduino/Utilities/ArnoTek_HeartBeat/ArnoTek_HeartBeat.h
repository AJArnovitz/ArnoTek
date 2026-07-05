//*******************************************************************************************
//                                                                                          *
//                                A r n o T e k _ H e a r t B e a t                         *
//                                                                                          *
//                   Copyright Anthony J. Arnovitz 2023    All rights reserved.             *
//                                                                                          *
//------------------------------------------------------------------------------------------*
//                                                                                          *
//      This class defines a Heartbeat utility for Arduino applications. It may use         *
// internally referenced millis() time values or the time value may be passed in using a    *
// parameter as desired. When the "Toggle" method is called, the heartbeat will be toggled  *
// if the amount of time (in milliseconds) specified at initialization has passed since the *
// last time the heartbeat was toggled.  If the specified amount of time has not passed,    *
// the "Toggle" method exits without doing anything.                                        *
//                                                                                          *
//      NOTE: This code will properly handle rollover of the Arduino time value.            *
//                                                                                          *
//*******************************************************************************************

#ifndef ArnoTekHeartBeat_h
#define ArnoTekHeartBeat_h

// This class controls the hearbeat signal
class ArnoTek_HeartBeat
{
  private:
    long unsigned ExpirationTime;   // value for when the timeout last expired
    int           Pin;              // pin that is used for the heartbeat signal
    int           Delay;            // delay (in milliseconds) between heartbeat signal toggles
    bool          Value;            // current value of the heartbeat signal

  public:  
    //*************************************************************************************
    // Initalize the heartbeat control object using an internal timer value
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             delay - number of miliseconds between toggling of the output signal
    //             pin - the digital pin that is used to output the heartbeat signal
    //*************************************************************************************

    Init(int delay, int pin)
    {
    	Init(millis(), delay, pin);        // Go initialize the heartbeat object
    }
	
 

    //*************************************************************************************
    // Initalize the heartbeat control object using a specified timer value
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             curTime - current value of the Arduino millisecond time
    //             delay - number of miliseconds between toggling of the output signal
    //             pin - the digital pin that is used to output the heartbeat signal
    //*************************************************************************************

    Init(long unsigned curTime, int delay, int pin)
    {
      // Save the initial values
      Delay = delay;
      Pin   = pin;
      Value = false;

      // configure the output pin for the heartbeat and set its initial signal
      pinMode(Pin, OUTPUT);
      digitalWrite(Pin, Value);

      // Save the current time
      ExpirationTime = curTime;
    }



    //*************************************************************************************
    // Toggle the heartbeat when the previously specified amount of time has passed
    // using an internal timer value
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             {NONE}
    //*************************************************************************************

    void Toggle()
    {
	Toggle(millis());        // Go toggle the heartbeat when appropriate
    }




    //*************************************************************************************
    // Toggle the heartbeat when the previously specified amount of time has passed
    // using a specified timer value
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             curTime - current value of the Arduino time
    //*************************************************************************************

    void Toggle(long unsigned curTime)
    {
      // Has the timeout period expired?
      if (curTime - ExpirationTime >= Delay) 
      {
        // Yes - toggle the heartbeat and save the time the last time the heartbeat was toggled
        ExpirationTime = curTime;        // Save the time for when the heartbeat last expired
	Value          = !Value;         // Toggle the heartbeat
        digitalWrite(Pin, Value);        // output the new heartbeat state
      }
   }

};
#endif  // ArnoTekHeartBeat_h
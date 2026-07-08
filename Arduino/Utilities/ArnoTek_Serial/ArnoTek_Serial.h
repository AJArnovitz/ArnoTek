//*******************************************************************************************
//                                                                                          *
//                               A r n o T e k _ S e r i a l                                *
//                                                                                          *
//                   Copyright Anthony J. Arnovitz 2026    All rights reserved.             *
//                                                                                          *
//------------------------------------------------------------------------------------------*
//                                                                                          *
//     This class defines the a serial interface that does not use "delay" statements and   *
// is suitable for applications that are time dependent and cannot tolerate "delay"         *
// statements.                                                                              *
//                                                                                          *
//      NOTE: This code will properly handle rollover of the Arduino time value.            *
//                                                                                          *
//------------------------------------------------------------------------------------------*
//                                                                                          *
// How to use this code:                                                                    *
//     1. Copy this code to your Arduino library                                            *
//     2. Add "#include <ArnoTek_Serial.h>" to your source code                             *
//     3. Define serial interface object before the setup routine                           *
//     4. Call the "Init" method of the serial interface object in the setup routine to set *
//        the interface's baud rate.                                                        *
//     5. Call the "GetMessage" method of the serial interface object obtain incoming text  *
//        The "GetMessage" method returns a true boolen value indicating if a completed     *
//        message is ready for processing.  If no completed message is available, "false"   *
//        is returned.                                                                      *
//                                                                                          *
// Example:                                                                                 *
//    #include <ArnoTek_Serial.h>                                                           *
//        .                                                                                 *
//        .                                                                                 *
//        .                                                                                 *
//    ArnoTek_Serial AT_Serial;       // Define serial interface object                     *
//    char inMessage[64];             // Define a buffer for the incoming message           *
//                                    // "64" is the maximun Arduino serial buffer size)    *
//        .                                                                                 *
//        .                                                                                 *
//        .                                                                                 *
//                                                                                          *
//    void setup()                                                                          *
//    {                                                                                     *
//       AT_Serial.Init(115200);   // Initialize the serial interface object and set the    * 
//                                 // baud rate (Serial.begin(x); )                         *
//                                                                                          *
//                             --- OR ---                                                   *
//                                                                                          *
//       AT_Serial.Init();         // Initialize the serial interface object but do NOT     * 
//                                 // "begin" the Serial interface.                         *
//                                 // (Serial.begin(x); is not executed in this             *
//                                 // initialization method and must be set in the user's   *
//                                 // setup code.)                                          *
//           .                                                                              *
//           .                                                                              *
//           .                                                                              *
//    }                                                                                     *
//                                                                                          *
//                                                                                          *
//    void loop()                                                                           *
//    {                                                                                     *
//       if(AT_Serial.GetMessage(inMessage))  // Check if a completed message was received  *
//       {                                                                                  *
//         // Process completed received message (the completed message is in "inMessage")  *
//       }                                                                                  *
//           .                                                                              *
//           .                                                                              *
//           .                                                                              *
//    }                                                                                     *
//                                                                                          *
//*******************************************************************************************


#ifndef ArnoTekSerial_H
#define ArnoTekSerial_H

class ArnoTek_Serial
{
  private:
    static const int       waitTimeBetweenReads       = 20;            // Number of milliseconds to wait between successive reads of the serial input;
    static const int       maxIncomingMessageLength   = 64;            // 64 is the Arduino's max length of an incoming serial message 
    bool                   isMessageInProgress;                        // TRUE is a message is in progress coming in
    char                   incomingMsg_USB[maxIncomingMessageLength];  // Buffer for the incoming message
    int                    bufferIndex;                                // Current index of the next input buffer position
    volatile long unsigned previousTimeEvent;                          // Last timer value of when the previous byte was procesed



  public:
    //*************************************************************************************
    //                                   I n i t ( )
    //
    // Initialize the serial message processing class but does not "begin" the serial port
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             {NONE}
    //*************************************************************************************
    
    void Init()
    {
      Init(0);
    }


    //*************************************************************************************
    //                                  I n i t (x)
    //
    // Initialize the serial message processing class and sets the interface baud rate
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             baudRate - Baud rate for the serial connection
    //                        (if = 0, the serial port is not started by this method)
    //*************************************************************************************
    
    void Init(long baudRate)
    {
      previousTimeEvent   = millis();
      bufferIndex         = 0;
      isMessageInProgress = false;

      if(baudRate != 0)
      {
        Serial.begin(baudRate);
      }
    }


    //*************************************************************************************
    //                                 G e t M e s s a g e
    //
    // Gets the completed recieved message (if there is one)
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             ReturnBuffer - Buffer to place the completed message
    //    Returns:
    //             False if no message is available for processing
    //             True if a completed message was received and is ready for processing
    //
    // NOTE: The end of an incomming message is determined when end end-of-line character
    //       is received or if the max length of the internal receive buffer is reached.
    //*************************************************************************************

    bool GetMessage(char* ReturnBuffer)
    {
      bool returnValue                      = false;     // Assume nothing is ready until proven otherwise
      volatile long unsigned CurrentTime_MS = millis();  // Get the current time value

      if (CurrentTime_MS - previousTimeEvent < waitTimeBetweenReads) return returnValue;    // If it is not time to do anything, get out immediately 


      previousTimeEvent = CurrentTime_MS;        // Save the time for when last did something

      // get the bytes of the incoming message when they are available
      if (Serial.available())
      {
        isMessageInProgress = true;

        // There is data on the serial port - go get it
        incomingMsg_USB[bufferIndex] = Serial.read();

        // Is this the end of the message? (Also make sure we do not overrun the buffer)
        if ((incomingMsg_USB[bufferIndex] == '\n') || ((bufferIndex + 1) >= maxIncomingMessageLength) )   
        {
            // Yes, the entire message has been received 
            incomingMsg_USB[bufferIndex] = '\0';    // mark the end of the message (Overwrite last used byte in buffer)

            // Copy the entire input buffer to the user's buffer and clear the input buffer
            for(int i = 0; i < maxIncomingMessageLength; i++)
            {
              ReturnBuffer[i]    = incomingMsg_USB[i];
              incomingMsg_USB[i] = ' ';       // Clear the entire buffer for the next time around in case the next message is shorter that the max buffer length
            }

            // reset the control variables for the next message and mark that there is a message available
            bufferIndex         = 0;
            isMessageInProgress = false;
            returnValue         = true;
        }
        else      // Not the end of the message - just bump the index for the next incoming byte
        {
            bufferIndex++;
        }  
      } 
  
      return returnValue;
    }

};     // End of ArnoTekSerial class

#endif
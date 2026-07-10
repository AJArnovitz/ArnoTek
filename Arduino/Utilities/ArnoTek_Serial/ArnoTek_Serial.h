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
//    char inMessage[64];             // Define a buffer for the incoming message. You      *
//                                    // can define a buffer length between 1 and 64 bytes. *
//                                    // ("64" is the maximun Arduino serial buffer size)   *
//        .                                                                                 *
//        .                                                                                 *
//        .                                                                                 *
//                                                                                          *
//    void setup()                                                                          *
//    {                                                                                     *
//       AT_Serial.Init();            // Initialize the serial interface object but do NOT  * 
//                                    // "begin" the Serial interface.  Buffer length is    *
//                                    // set to the Arduino maximum length                  *
//                                    // (Serial.begin(x); is not executed in this          *
//                                    // initialization method and must be set in the       *
//                                    // user's setup code.)                                *
//                                                                                          *
//                             --- OR ---                                                   *
//                                                                                          *
//       AT_Serial.Init(20);          // Initialize the serial interface object but do NOT  * 
//                                    // "begin" the Serial interface. Buffer length is set *
//                                    // to 20. (Serial.begin(x); is not executed in this   *
//                                    // initialization method and must be set in the       *
//                                    // user's setup code.)                                *
//                                                                                          *
//                             --- OR ---                                                   *
//                                                                                          *
//       AT_Serial.Init(35, 115200);  // Initialize the serial interface object, set the    *
//                                    // buffer length to 35, and set the serial interface  *
//                                    // baud rate to 115200. (Serial.begin(x); IS executed *
//                                    // in this initialization method and there is no need *
//                                    // to execute a Serial.begin(x) in the user's setup   *
//                                    // code.)                                             *
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
    static const int       maxArduinoMessageLength    = 64;            // 64 is the Arduino's max length of an incoming serial message 
    bool                   isMessageInProgress;                        // TRUE is a message is in progress coming in
    bool                   isMessageComplete;                          // TRUE if the message is complete and ready to pass tp the user
    bool                   isPriorOverflow;                            // TRUE if an end-of-line character was inserted on the previous message due to a user buffer overflow
    char                   incomingMsg_USB[maxArduinoMessageLength];   // Buffer for the incoming message
    int                    bufferIndex;                                // Current index of the next input buffer position
    int                    userBufferLength;                           // Length of the user's buffer length
    volatile long unsigned previousTimeEvent;                          // Last timer value of when the previous byte was procesed



  public:
    //*************************************************************************************
    //                                   I n i t ( )
    //
    // Initialize the serial message processing class, set the user's buffer length to the
    // Arduino maximum length.
    // *** This method does NOT "begin" the serial interface. ***
    //-------------------------------------------------------------------------------------
    // Parameters:
    //             {NONE}
    //*************************************************************************************
    
    void Init()
    {
      Init(maxArduinoMessageLength, 0);
    }



    //*************************************************************************************
    //                                 I n i t (bufferLength)
    //
    // Initialize the serial message processing class and sets the user's buffer length.
    // *** This method does NOT "begin" the serial interface. ***
    //-------------------------------------------------------------------------------------
    // Parameters:
    //         bufferLength - Length of the user's input buffer
    //*************************************************************************************
    
    void Init(int bufferLength)
    {
      Init(bufferLength, 0);
    }



    //*************************************************************************************
    //                             I n i t (bufferLength, baudRate)
    //
    // Initialize the serial message processing class, set the user's specified buffer 
    // length, and sets the interface baud rate.
    //-------------------------------------------------------------------------------------
    // Parameters:
    //         bufferLength - Length of the user's input buffer - must be between 1 and
    //                        the Arduino's maximum buffer length (64)
    //             baudRate - Baud rate for the serial connection
    //                        (if = 0, the serial interface is not started by this method)
    //*************************************************************************************
    
    void Init(int bufferLength, long baudRate)
    {
      previousTimeEvent     = millis();
      bufferIndex           = 0;
      isMessageInProgress   = false;
      isMessageComplete     = false;
      isPriorOverflow       = false;

      if(baudRate != 0)
      {
        Serial.begin(baudRate);
      }

      // Validate the user specified buffer length and change it if the specified value is outside of allowable values
      if(bufferLength <= 0)
      {
        // The user specified a buffer length of zero or below so make the buffer length 1
        userBufferLength    = 1;
      }
      else if (bufferLength > maxArduinoMessageLength)
      {
        // The user specified a buffer length is greater than the Arduino's maximum length 
        // so make the buffer length equal the the Arduino maximum length      
        userBufferLength    = maxArduinoMessageLength;
      }
      else 
      { 
        // The user specified a valid buffer length so use it       
        userBufferLength    = bufferLength;
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

      //*****************************************************************************************************************************
      //                                   Note about the processing of "isPriorOverflow" boolean                                   *
      //----------------------------------------------------------------------------------------------------------------------------*
      //                                                                                                                            *
      //    The last byte of the message written to the user's buffer must always be a numeric 0 to indicate the end                *
      //    of the message.  This code will automatically change the end-of-line character received from the interface              *
      //    to a numeric 0 to indicate the end of the message.  If the user enters same number or more characters in the interface, *
      //    this code will automatically insert a numeric 0 in the last byte of the buffer to indicate the end of the               *
      //    message.  So if the user defined a 5 byte buffer and entered 6 characters in the interface, a 5 byte messsage           *
      //    will be returned of the first four characaters followeded by a numeric 0 in the last byte of the buffer to              *
      //    indicate the end of the first message.  The following call to "GetMessage" will return the remaining entered            *
      //    two characters and the terminating newline character from the interface will be changed to a numeric 0 to               *
      //    indicate the end of the message.  So in this scenario, entering 6 characters when the buffer was defined as             *
      //    being 5 bytes long results in TWO messages being returned even though the user only entered one end-of-line character.  *
      //    Characters 1 through 4 will be returned in the first message and characters 5 through 6 will be returned in a           *
      //    second message.                                                                                                         *
      //                                                                                                                            *
      //    If the user entered 4 bytes (with a 5 byte buffer size being specified as described earlier), a numeric 0 is            *
      //    inserted into the last byte of the user buffer and a message is returned to the user even though the end-of-line        *
      //    character has not yet been received.  This situation is when the "isBufferOverflow" is set to TRUE so if the next       *
      //    character received is an end-of-line character, it is ignored.  So in this scenario, only one message will be returned  *
      //    consisting of characters 1 through 4 followed by a numeric 0 in the last byte of the buffer to indicate the end of      *
      //    the message.                                                                                                            *
      //                                                                                                                            *
      //*****************************************************************************************************************************


      // get the bytes of the incoming message when they are available
      if (Serial.available())
      {
        isMessageInProgress                 = true;

        // There is data on the serial port - go get it
        incomingMsg_USB[bufferIndex] = Serial.read();

        // Is this the end of the message from the serial port? 
        if (incomingMsg_USB[bufferIndex] == '\n')    
        {
          if(isPriorOverflow)
          {
            isPriorOverflow                 = false;  // If we are in a buffer overflow situation on the next characater, just ignore it
          } 
          else
          {
            // Yes, the entire message has been received and the last message was not a buffer overflow situation
            incomingMsg_USB[bufferIndex]    = '\0';   // Mark the end of the message (Overwrite last used byte in buffer)
            isMessageComplete               = true;   // Mark the message complete and ready to pass to the user
          }
        }
        else 
        {
          // Process the next character that is not and-of-line
          isPriorOverflow                   = false;  // Don't worry about a prior buffer overflow situation since the next byte is not an end-of-line

          // Are we at the end of the user's buffer (leaving room for the string terminator character)
          if (bufferIndex >= (userBufferLength -2) )  // The "2" is because the buffer index is relative 0 plus we need a byte for the string terminator
          {
            isPriorOverflow                 = true;   // The message may be longer than the user's buffer
            incomingMsg_USB[++bufferIndex]  = '\0';   // Mark the end of the message (first byte AFTER the last character receiverd)
            isMessageComplete               = true;   // Mark the message complete and ready to pass to the user
          }        
          else // Not the end of the message - just bump the index to get things ready for the next incoming byte
          {
              bufferIndex++;
          }
        }  

        // Is the received message ready to pass back to the user?
        if(isMessageComplete)
        {          
            // Copy the entire input buffer to the user's buffer and clear the input buffer
            for(int i = 0; i < userBufferLength; i++)
            {
              ReturnBuffer[i]               = incomingMsg_USB[i];
              incomingMsg_USB[i]            = ' ';    // Clear the entire buffer for the next time around in case the next message is shorter that the max buffer length
            }

            // Reset the control variables for the next message and mark that there is a message available
            bufferIndex                     = 0;
            isMessageInProgress             = false;
            isMessageComplete               = false;
            returnValue                     = true;
        }
      } 
  
      return returnValue;
    }

};     // End of ArnoTek_Serial class

#endif
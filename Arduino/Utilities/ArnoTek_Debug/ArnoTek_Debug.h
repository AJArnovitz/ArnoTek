//*******************************************************************************************
//	                             A r n o T e k _ D e b u g . h                          *
//                                                                                          *
//                   Copyright Anthony J. Arnovitz 2026    All rights reserved.             *
//                                                                                          *
//------------------------------------------------------------------------------------------*
//                                                                                          *
//     This include file defines the debugging definitions that duplicate the Serial        *
// methods.  If the ArnoTek_DEBUG compiler variable is set to 1, the duplicate methods are  *
// defined.  If it is not set to 1, the duplicate methods are defined as spaces and do not  *
// have an impact on the compiled code and do not generate any object/machine code.         *
//                                                                                          *
//------------------------------------------------------------------------------------------*
//                                                                                          *
// How to use this code:                                                                    *
//     1. Copy this code to your Arduino library                                            *
//     2. Add "#include <ArnoTek_Debug.h>" to your source code                              *
//     3. Define ArnoTek_DEBUG = 1 to enable the serial print statements at compile time    *
//                         - - -  OR   - - -                                                *
//        Define ArnoTek_DEBUG = 0 to disable the serial print statements at compile time   *
//     4. Use the various ArnoTek_Debug statements                                          *
//                                                                                          *
//                                                                                          *
// NOTE: If non-debugging serial input/output is used, do not use the ArnoTek_DebugInit     *
//       statement but instead use a Serial.begin statement prior to the ArnoTek_Debug      *
//       statements.                                                                        *
//                                                                                          *
//       The ArnoTek_DEBUG compile variable must be defined PRIOR to the                    *
//       #include <ArnoTek_Debug.h> statement in your code                                  *
//                                                                                          *
//------------------------------------------------------------------------------------------*
//                                                                                          *
// Example:                                                                                 *
//    #define ArnoTek_DEBUG 1                                                               *
//    - - -  OR   - - -                                                                     *
//    #define ArnoTek_DEBUG 0                                                               *
//        .                                                                                 *
//    #include <ArnoTek_Debug.h>                                                            *
//        .                                                                                 *
//        .                                                                                 *
//   ArnoTek_DebugInit(115200);  // Do not use if non-debugging serial interface is used    *
//   ArnoTek_DebugLn("");                                                                   *
//   ArnoTek_DebugLn("**********************************");                                 *
//   ArnoTek_DebugLn("*** Here is a heading ***");                                          *
//   ArnoTek_DebugLn("**********************************");                                 *
//   ArnoTek_DebugLn("");                                                                   *
//   ArnoTek_DebugLn2(someVariable, HEX);    // Display value in hexidecimal                *
//        .                                                                                 *
//        .                                                                                 *
//        .                                                                                 *
//                                                                                          *
//*******************************************************************************************

#ifndef ArnoTekDebug_H
    #define ArnoTekDebug_H

    #if ArnoTek_DEBUG == 1
        #define ArnoTek_DebugInit(x) Serial.begin(x)
        #define ArnoTek_Debug(x) Serial.print(x)
        #define ArnoTek_Debug2(x, y) Serial.print(x, y)
        #define ArnoTek_DebugLn(x) Serial.println(x)
        #define ArnoTek_DebugLn2(x, y) Serial.println(x, y)
        #define ArnoTek_DebugFlush() Serial.flush();
        #define ArnoTek_DebugDelay(x) delay(x)
    #else
        #define ArnoTek_DebugInit(x)
        #define ArnoTek_Debug(x) 
        #define ArnoTek_Debug2(x, y)
        #define ArnoTek_DebugLn(x) 
        #define ArnoTek_DebugLn2(x, y)
        #define ArnoTek_DebugFlush() 
        #define ArnoTek_DebugDelay(x)
    #endif
#endif
/*


 Revision History:
 ~~~~~~~~~~~~~~~~~
 26.08.23:  - Started


 ToDo:
 ~~~~~
*/

#ifndef __WORD_CLOCK_MOTION_DETECT_EXTENTION__
#define __WORD_CLOCK_MOTION_DETECT_EXTENTION__
#if !defined(AVR) && !defined(ESP32)
  #error Platform is not supported
#endif


#include <MLLExtension.h>

#ifndef MD_DEBUG_LED_PIN
//#define MD_DEBUG_LED_PIN 18
#endif

//***********************************************
class WordClock_MotionDetect: public MLLExtension
//***********************************************
{
  private:
    uint32_t    HoldTime;
    uint8_t     SensorPin;
    uint8_t     HoldPin;
    bool        OldSensor;
    uint32_t    LastMotion;
    uint32_t    LastUpdate;

    //----------------------------------------------------------------------------------
    public:WordClock_MotionDetect(uint32_t HoldTime, uint8_t SensorPin, uint8_t HoldPin) // Constructor
    //----------------------------------------------------------------------------------
    {
      this->HoldTime  = HoldTime;
      this->SensorPin = SensorPin;
      this->HoldPin   = HoldPin;
	}

    //-----------------------------------------
    public:void setup(MobaLedLib_C& mobaLedLib)
    //-----------------------------------------
    {
      //Serial << F("\nWordClock MotionDetect setup\n"); // Debug
      OldSensor = false;
      #ifdef MD_DEBUG_LED_PIN
        pinMode(MD_DEBUG_LED_PIN, OUTPUT);
        digitalWrite(MD_DEBUG_LED_PIN, 1);
      #endif
      pinMode(SensorPin, INPUT);
      digitalWrite(HoldPin, 1);
      pinMode(HoldPin, OUTPUT);
      LastUpdate = LastMotion = millis();
	}

    //-------------------------------------------------------------
    private: void Debug_Loop(MobaLedLib_C& mobaLedLib, bool Sensor)
    //-------------------------------------------------------------
    {
      #ifdef MD_DEBUG_LED_PIN
        digitalWrite(MD_DEBUG_LED_PIN, Sensor);
      #endif

      #ifdef MD_DEBUG_RGB_LED
        {
        #ifndef MD_DEBUG_RGB_LED_END
          #if MD_DEBUG_RGB_LED == 16
             #define MD_DEBUG_RGB_LED_END MD_DEBUG_RGB_LED - 16
          #else
             #define MD_DEBUG_RGB_LED_END MD_DEBUG_RGB_LED + 16
          #endif
        #endif
        #ifndef MD_DEBUG_RGB_STEPP_PERIODE
          #define MD_DEBUG_RGB_STEPP_PERIODE   (HoldTime/16)
        #endif
        // Wenn eine Bewegung erkannt wird gehen alle Debug LEDs an
        // Die LEDs gehen nacheinander aus wenn keine Bewegung mehr erkannt wird
        int Direction = MD_DEBUG_RGB_LED > MD_DEBUG_RGB_LED_END ? -1:1;
        uint8_t Cnt = abs(MD_DEBUG_RGB_LED_END - MD_DEBUG_RGB_LED);
        uint32_t Time_since_Motion = millis() - LastMotion;
        uint32_t Steps_since_Motion = Time_since_Motion / MD_DEBUG_RGB_STEPP_PERIODE;
        static uint32_t LastPrint = 0;
        if (millis() - LastPrint > 1000)
            {
            LastPrint = millis();
            // Serial << F("Steps_since_Motion:") << Steps_since_Motion << endl; // Debug
            }
        if (Sensor)
             mobaLedLib.leds[MD_DEBUG_RGB_LED] = CRGB::White;
        else mobaLedLib.leds[MD_DEBUG_RGB_LED] = CRGB::Black;
        uint8_t i, LedNr = MD_DEBUG_RGB_LED + Direction;
        for (i = 1; i < Cnt; i++, LedNr += Direction)
            {
            if (Steps_since_Motion > i)
                 mobaLedLib.leds[LedNr] = CRGB::Black;
            else mobaLedLib.leds[LedNr] = CRGB::HotPink;
            }
        }
      #endif
    }

    //----------------------------------------
    public:void loop2(MobaLedLib_C& mobaLedLib)
    //----------------------------------------
    {
      bool Sensor = digitalRead(SensorPin);
      Debug_Loop(mobaLedLib, Sensor);

      if (Sensor == true)
           {
           if (OldSensor == false)
             {
             //Serial << F("Motion detected after ") << (millis() - LastMotion) / 1000 << F(" s\n"); // Debug
             digitalWrite(HoldPin, 1);
             }
           LastMotion = LastUpdate = millis();
           }
      else {
           #ifdef DEBUG_POWEROFF
             if (millis() - LastUpdate > 1000) // Debug
                {
                LastUpdate = millis();
                uint32_t Time_since_Motion = millis() - LastMotion;
                if (digitalRead(HoldPin)) Serial << F("Power off in ") <<  (HoldTime - Time_since_Motion) / 1000 << F(" s\n");
                }
           #endif // DEBUG_POWEROFF
           if (millis() - LastMotion > HoldTime)
              {
              //if (digitalRead(HoldPin)) Serial << F("Power off !!\n"); // Debug
              digitalWrite(HoldPin, 0);
              }
           }
      OldSensor = Sensor;
    }
};


#endif // __WORD_CLOCK_MOTION_DETECT_EXTENTION__
/*
In file included from C:\Users\Hardi\DOCUME~1\Arduino\LIBRAR~1\IRremote\src/IRFeedbackLED.hpp:34:0,
                 from C:\Users\Hardi\DOCUME~1\Arduino\LIBRAR~1\IRremote\src/IRremote.hpp:218,
                 from C:\Users\Hardi\DOCUME~1\Arduino\LIBRAR~1\IRremote\src/IRremote.h:10,
                 from C:\Users\Hardi\DOCUME~1\Arduino\LIBRAR~1\InfraredMLX\src/InfraredMLX.h:29,
                 from C:\Users\Hardi\AppData\Local\Temp\MOBALE~1\ESP32\sketch\LEDs_AutoProg.h:606,
                 from C:\Users\Hardi\AppData\Local\Temp\MOBALE~1\ESP32\sketch\LEDs_AutoProg.ino.cpp:270:
C:\Users\Hardi\DOCUME~1\Arduino\LIBRAR~1\IRremote\src/private/IRFeedbackLEDDefs.h:83:2: warning: #warning No definition for LED_BUILTIN for default feedback LED found. Check private/IRFeedbackLEDDefs.h. [-Wcpp]
 #warning No definition for LED_BUILTIN for default feedback LED found. Check private/IRFeedbackLEDDefs.h.
*/

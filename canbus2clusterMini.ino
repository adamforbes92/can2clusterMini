/* 
Super basic CAN-BUS converter to Digital Output.  Used for MK3 'digital' clusters in ME7.x and aftermarket conversions and will provide an RPM and Speed ONLY.
Supports GPS add-ons
All outputs are configurable 12v Square Wave with definable max limits based on x RPM
V1.00 - basic code, derived from Can2Cluster 

Forbes-Automotive, 2025
*/

// for CAN
#include "canbus2clusterMini_defs.h"
#include <ESP32_CAN.h>
ESP32_CAN<RX_SIZE_256, TX_SIZE_16> chassisCAN;

// for GPS
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
SoftwareSerial ss(pinRX_GPS, pinTX_GPS);
TinyGPSPlus gps;

// for inputs / paddles
#include <ButtonLib.h>  //include the declaration for this class
buttonClass btnPadUp(pinPaddleUp, 0, false);
buttonClass btnPadDown(pinPaddleDown, 0, false);

// define two hardware timers for RPM & Speed outputs
hw_timer_t* timer0 = NULL;
hw_timer_t* timer1 = NULL;

bool rpmTrigger = true;
bool speedTrigger = true;
long frequencyRPM = 20;    // 20 to 20000
long frequencySpeed = 20;  // 20 to 20000

//if (1) {  // This contains all the timers/Hz/Freq. stuff.  Literally in a //(1) to let Arduino IDE code-wrap all this...
// timer for RPM
void IRAM_ATTR onTimer0() {
  rpmTrigger = !rpmTrigger;
  digitalWrite(pinRPM, rpmTrigger);
}

// timer for Speed
void IRAM_ATTR onTimer1() {
  speedTrigger = !speedTrigger;
  digitalWrite(pinSpeed, speedTrigger);
}

// setup timers
void setupTimer() {
  timer0 = timerBegin(0, 40, true);  //div 80
  timerAttachInterrupt(timer0, &onTimer0, true);

  timer1 = timerBegin(1, 40, true);  //div 80 - 40 results in perfect hz transmission
  timerAttachInterrupt(timer1, &onTimer1, true);
}

// adjust output frequency
void setFrequencyRPM(long frequencyHz) {
  if (frequencyHz != 0) {
    timerAlarmDisable(timer0);
    timerAlarmWrite(timer0, 1000000l / frequencyHz, true);
    timerAlarmEnable(timer0);
  } else {
    timerAlarmDisable(timer0);
  }
}

// adjust output frequency
void setFrequencySpeed(long frequencyHz) {
  if (frequencyHz != 0) {
    timerAlarmDisable(timer1);
    timerAlarmWrite(timer1, 1000000l / frequencyHz, true);
    timerAlarmEnable(timer1);
  } else {
    timerAlarmDisable(timer1);
  }
}
//}

void setup() {
  basicInit();   // basic init for setting up IO / CAN / GPS
  setupTimer();  // setup the timers (with a base frequency)

  if (hasNeedleSweep) {
    needleSweep();  // carry out needle sweep if defined
  }
}

void loop() {
  // get the easy stuff out the way first
  // has error - todo: set to flash, etc...
  if (selfTest) {
    //needleSweep();
    diagTest();
  }

  if (hasError) {
    digitalWrite(onboardLED, HIGH);  // light internal LED
  } else {
    digitalWrite(onboardLED, LOW);
  }

  if (vehicleReverse) {
    digitalWrite(pinReverse, HIGH);  // turn relay on...
  } else {
    digitalWrite(pinReverse, LOW);  // turn relay on...
  }

  btnPadUp.tick();    // paddle up
  btnPadDown.tick();  // paddle down

  // send CAN data for paddle up/down etc
  if (boolPadUp) {
    Serial.println(F("Paddle up"));
    sendPaddleUpFrame();
    boolPadUp = false;
  }
  if (boolPadDown) {
    Serial.println(F("Paddle down"));
    sendPaddleDownFrame();
    boolPadDown = false;
  }

  // get speed type (ECU, DSG or GPS)
  switch (speedType) {
    case 0:  // get speed from ecu
      if (calcSpeed > 0) {
        vehicleSpeed = (byte)(calcSpeed >= 255 ? 0 : calcSpeed);
      }
      break;

    case 1:                                       // get speed from dsg
      if ((millis() - lastMillis) > gearPause) {  // check to see if x ms (linPause) has elapsed - slow down the frames!
        lastMillis = millis();
        parseDSG();
      }
      vehicleSpeed = int(dsgSpeed);
      break;

    case 3:
      vehicleSpeed = int(absSpeed);
      break;
  }

  if (speedUnits == 1) {
    vehicleSpeed = vehicleSpeed * mphFactor;
  }

  // calculate final frequency:
  frequencySpeed = map(vehicleSpeed, 0, clusterSpeedLimit, 0, maxSpeed);
  frequencyRPM = map(vehicleRPM, 0, clusterRPMLimit, 0, maxRPM);

  // change the frequency of both RPM & Speed as per CAN information
  if ((millis() - lastMillis2) > rpmPause) {  // check to see if x ms (linPause) has elapsed - slow down the frames!
    lastMillis2 = millis();
#if stateDebug
    Serial.println(frequencyRPM);
    Serial.println(frequencySpeed);
#endif
    setFrequencyRPM(frequencyRPM);      // minimum speed may command 0 and setFreq. will cause crash, so +1 to error 'catch'
    setFrequencySpeed(frequencySpeed);  // minimum speed may command 0 and setFreq. will cause crash, so +1 to error 'catch'  }
  }
}
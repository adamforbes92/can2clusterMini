/* Defines */
// Debug statements
#define ChassisCANDebug 0  // if 1, will print CAN 2 (Chassis) messages ** CAN CHANGE THIS **
#define stateDebug 1       // if 1, will use Serial talkback ** CAN CHANGE THIS **
#define selfTest 1         // increase RPM/speed slowly, flash lights.  For debug only, disable on release! ** CAN CHANGE THIS **

// setup - main functions
#define hasNeedleSweep 0  // do needle sweep on power up? ** CAN CHANGE THIS **
#define speedType 0       // 0 = ECU, 1 = DSG, 2 = GPS, 3 = ABS
#define speedUnits 0      // 0 = kph, 1 = mph
#define mphFactor 0.621371 // to convert from kmh > mph

// setup - tweaky things
#define needleSweepDelay 15  // delay between next freq.  Increase/decrease to change the sweep time ** CAN CHANGE THIS **

// setup - Hz adjustment
#define maxRPM 230    // max RPM in Hz for the cluster (for needle sweep) ** CAN CHANGE THIS **
#define maxSpeed 400  // max Speed in Hz for the cluster (for needle sweep).  MK3 default is 500.  MK1/MK2 (has cable), default is xxx ** CAN CHANGE THIS **

// setup - RPM & speed limits
#define clusterRPMLimit 7000   // rpm ** CAN CHANGE THIS **
#define clusterSpeedLimit 200  // km/h ** CAN CHANGE THIS **
#define shiftLimit 6000        // set the RPM limit for the shift light ** CAN CHANGE THIS **

// setup - step changes (for needle sweep)
#define stepRPM 1
#define stepSpeed 1

// setup - pins (Serial / RX/TX)
#define pinRX_CAN 16  // pin input for SN65HVD230 (CAN_RX)
#define pinTX_CAN 17  // pin input for SN65HVD230 (CAN_TX)
#define pinRX_GPS 13  // pin input for GPS Module (GPS_RX)
#define pinTX_GPS 14  // pin input for GPS Module (GPS_TX)

// setup - pins (output)
#define pinRPM 25      // pin output for RPM22
#define pinSpeed 26    // pin output for Speed
#define pinReverse 18  // pin output for Reverse

// setup - pins (input)
#define pinPaddleUp 36    // pin input for steering wheel paddle up
#define pinPaddleDown 39  // pin input for steering wheel paddle down

// setup - pins (onboard)
#define onboardLED 2  // pin onboard LED

// baudrates
#define baudSerial 9600  // baud rate for debug
#define baudGPS 9600     // baud rate for the GPS device

// DSG variables
#define PI 3.141592653589793
#define LEVER_P 0x8               // park position
#define LEVER_R 0x7               // reverse position
#define LEVER_N 0x6               // neutral position
#define LEVER_D 0x5               // drive position
#define LEVER_S 0xC               // spot position
#define LEVER_TIPTRONIC_ON 0xE    // tiptronic
#define LEVER_TIPTRONIC_UP 0xA    // tiptronic up
#define LEVER_TIPTRONIC_DOWN 0xB  // tiptronic down
#define gearPause 20              // Send packets every x ms ** CAN CHANGE THIS **
#define rpmPause 50

extern uint16_t vehicleRPM = 1;    // current RPM.  If no CAN, this will catch dividing by zero by the map function
extern uint16_t vehicleSpeed = 1;  // current Speed.  If no CAN, this will catch dividing by zero by the map function
extern uint16_t calcSpeed = 0;     // temp var for calculating speed
extern int tempSpeed2[] = { 20, 40, 60, 80, 100, 160, 200 };
extern int tempRPM2[] = { 1000, 2000, 3000, 4000, 5000, 6000, 7000 };

extern double ecuSpeed = 0;  // ECU speed (from analog speed sensor)
extern double dsgSpeed = 0;  // DSG speed (from RPM & Gear), ratios in '_dsg.ino'
extern double gpsSpeed = 0;  // GPS speed (from '_gps.ino')
extern double absSpeed = 0;  // ABS speed (from '_gps.ino')

// DSG variables
extern uint8_t gear = 0;   // current gear from DSG
extern uint8_t lever = 0;  // shifter position
extern uint8_t gear_raw = 0;
extern uint8_t lever_raw = 0;
uint32_t lastMillis = 0;   // Counter for sending frames x ms
uint32_t lastMillis2 = 0;  // Counter for sending frames x ms

// ECU variables
extern bool vehicleEML = false;  // current EML light status
extern bool vehicleEPC = false;  // current EPC light status
extern bool vehicleReverse = false;
extern bool vehiclePark = false;

// external variables / triggers
extern bool boolPadUp = false;    // current EML light status
extern bool boolPadDown = false;  // current EPC light status

// onboard LED for error
extern bool hasError = false;

// define CAN Addresses.  All not req. but here for keepsakes
#define MOTOR1_ID 0x280
#define MOTOR2_ID 0x288
#define MOTOR3_ID 0x380
#define MOTOR5_ID 0x480
#define MOTOR6_ID 0x488
#define MOTOR7_ID 0x588

#define MOTOR_FLEX_ID 0x580
#define GRA_ID 0x38A   // byte 1 & 3 are shaft speeds?
#define gear_ID 0x440  // lower 4 bits of byte 2 are gear?

#define BRAKES1_ID 0x1A0
#define BRAKES2_ID 0x2A0
#define BRAKES3_ID 0x4A0
#define BRAKES5_ID 0x5A0

#define gearLever_ID 0x448
#define mWaehlhebel_1_ID 0x540  // DQ250 DSG ID

#define HALDEX_ID 0x2C0

#define emeraldECU1_ID 0x1000
#define emeraldECU2_ID 0x1001

extern void basicInit(void);
extern void canInit(void);
extern void onBodyRX(void);
extern void needleSweep(void);
extern void setupPins(void);
extern void blinkLED(int duration, int flashes, bool boolEPC, bool boolEML, bool boolRPM, bool boolSpeed);
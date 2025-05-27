void basicInit() {
// basic initialisation - setup pins for IO & setup CAN for receiving...
#if stateDebug
  Serial.begin(baudSerial);
  Serial.println(F("CAN-BUS to Cluster Initialising..."));
  Serial.println(F("Setting up pins..."));
#endif

  if (speedType == 2) {
    ss.begin(baudGPS);
#if stateDebug
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
    Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
    Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
#endif
  }

  setupPins();  // initialise the CAN chip
  setupButtons();

#if stateDebug
  Serial.println(F("Setup pins complete!"));
#endif

#if stateDebug
  Serial.println(F("CAN Chip Initialising..."));
#endif
  canInit();  // initialise the CAN chip
#if stateDebug
  Serial.println(F("CAN Chip Initialised!"));
#endif
}

void setupPins() {
  // define pin modes for outputs
  pinMode(onboardLED, OUTPUT);  // use the built-in LED for displaying errors!

  pinMode(pinRPM, OUTPUT);
  pinMode(pinSpeed, OUTPUT);
  pinMode(pinReverse, OUTPUT);

  pinMode(pinPaddleUp, INPUT);
  pinMode(pinPaddleDown, INPUT);
}

void setupButtons() {
  //setup buttons / inputs
  btnPadUp.attachSingleClick(padUpFunc);      // call intSingle on a single click (single wipe)
  btnPadDown.attachSingleClick(padDownFunc);  // call intSingle on a single click (single wipe)
}

void needleSweep() {
  frequencyRPM = 0;
  frequencySpeed = 0;
  setFrequencyRPM(frequencyRPM);
  setFrequencySpeed(frequencySpeed);

  delay(needleSweepDelay);

#if stateDebug
  Serial.println(F("Starting needle sweep..."));
#endif

  while ((frequencyRPM != maxRPM)) {
    setFrequencyRPM(frequencyRPM);
    setFrequencySpeed(frequencySpeed);

    // scaling?...
    frequencyRPM += stepRPM;
    frequencySpeed += stepSpeed;
    delay(needleSweepDelay);  // increase or decrease the needle sweep speed in _defs
  }

  while ((frequencyRPM != 0)) {
    setFrequencyRPM(frequencyRPM);
    setFrequencySpeed(frequencySpeed);

    // scaling?...
    frequencyRPM -= stepRPM;
    frequencySpeed -= stepSpeed;
    delay(needleSweepDelay);  // increase or decrease the needle sweep speed in _defs
  }

  frequencyRPM = 1;
  frequencySpeed = 5;
  setFrequencyRPM(frequencyRPM);
  setFrequencySpeed(frequencySpeed);

  delay(needleSweepDelay);

#if stateDebug
  Serial.println(F("Finished needle sweep!"));
#endif
}

void blinkLED(int duration, int flashes, bool boolRPM, bool boolSpeed) {
  for (int i = 0; i < flashes; i++) {
    if (boolRPM) {
      delay(duration);
      digitalWrite(pinRPM, HIGH);
      delay(duration);
      digitalWrite(pinRPM, LOW);
    }
    if (boolSpeed) {
      delay(duration);
      digitalWrite(pinSpeed, HIGH);
      delay(duration);
      digitalWrite(pinSpeed, LOW);
    }
  }
}

void diagTest() {
  vehicleRPM += 500;
  vehicleSpeed += 5;

  if (vehicleRPM > clusterRPMLimit) {
    vehicleRPM = 1000;
    frequencyRPM = 1;
  }
  if (vehicleSpeed > clusterSpeedLimit) {
    vehicleSpeed = 1;
    frequencySpeed = 1;
  }

  vehicleReverse = !vehicleReverse;
  digitalWrite(pinReverse, vehicleReverse);

  blinkLED(200, 1, 0, 0);
}
/**
   Authentication for Firbase connection
*/
void setupFirebase() {

  // Start firebase to be ready for incomming commands
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // Callback for data receiver
  Firebase.stream(firebaseUrl, streamCallback);
}

void streamCallback(streamResult event) {
  String eventType = event.eventType();
  eventType.toLowerCase();
  if (eventType == "put") {
    Serial.println("The stream event path: " + event.path() + ", value: " + String(event.getString()));
    Serial.println();

    if (event.getString() == "UNLOCKED") {
      unlockDoor(servo);
    } else {
      lockDoor(servo);
    }
  }
}

/**
   Check schedule
*/
bool isScheduled() {
  return true;
}

/*
   Go to deep sleep mode
*/
void goToSleep() {
  SLEEPSCRN();
std: time_t wait = time(nullptr);
  Serial.println("Waiting to sleep ...");
  while (digitalRead(MOTION_IN) == HIGH || digitalRead(MOTION_OUT) == HIGH) {
    delay(100);
  }
  running = false;
  delay(1000);
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
}

/**
   Setup background thread
*/
void setupThread() {

  xTaskCreatePinnedToCore(
    listenForRemoteControl,                  /* pvTaskCode */
    "Control",            /* pcName */
    10000,                   /* usStackDepth */
    NULL,                   /* pvParameters */
    1,                      /* uxPriority */
    &TaskA,                 /* pxCreatedTask */
    1);                     /* xCoreID */

  Serial.println("Task created...");
}

/**
   Listen for remote control from App
*/
void listenForRemoteControl(void * parameter) {
  setupWiFiServer(server, ssid, password);
  // Connect to firebase
  setupFirebase();
  vTaskDelete( NULL );
}

/**
   Always sleep in the first start up
*/
void firstSleep() {
  if (bootCount == 0) {
    bootCount++;
    SLEEPSCRN();
    delay(15);
    esp_deep_sleep_start();
  }
}

/**
   Set up pin mode
*/
void setupPinMode() {
  pinMode(MOTION_IN, INPUT);
  pinMode(MOTION_OUT, INPUT);
  pinMode(RFID, INPUT);
  pinMode(BATTEST, OUTPUT);
  pinMode(BATTERY_ONE, INPUT);
  pinMode(BATTERY_TWO, INPUT);
  pinMode(SETUP_MODE, INPUT);
  pinMode(RFID_ON, OUTPUT);
  pinMode(EXTRA_BUTTON_1, INPUT);
  pinMode(EXTRA_BUTTON_2, INPUT);
}

void initDB() {
  database.push_back("900_079000177267");
  database.push_back("900_079000177295");
}

void btSetup() {
  SerialBT.begin("CATDOOR"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  BT = 1;
}

void btHandle() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    // Serial.write(SerialBT.read());
    String BTdata = readSerialBT();
    Serial.println(BTdata);
    if (BTdata == "close") {
      SerialBT.print("closing bluetooth");
      BT = 0;
      BTdata = "";
      SerialBT.end();
      goToSleep();
    }
    if (BTdata == "SSID") {
      btFlag = 1;
      SerialBT.print("sOK");
      while (btFlag == 1) {
        if (SerialBT.available()) {
          BTdata = readSerialBT();
          SSIDtemp = BTdata;
          if (SSIDtemp != "") {
            SerialBT.print("SSID set");
            btFlag = 0;
          }
        }
        // wait for ssid data
      }
    }
    if (BTdata == "PASS") {
      btFlag = 1;
      SerialBT.print("pOK");
      while (btFlag == 1) {
        if (SerialBT.available()) {
          BTdata = readSerialBT();
          PASStemp = BTdata;
          // wait for password data
          if (PASStemp != "") {
            SerialBT.print("PASS set");
            btFlag = 0;
          }
        }
      }
    }
    if (BTdata == "USER") {
      btFlag = 1;
      SerialBT.print("uOK");
      while (btFlag == 1) {
        if (SerialBT.available()) {
          BTdata = readSerialBT();
          USERtemp = BTdata;
          // wait for password data
          if (USERtemp != "") {
            SerialBT.print("USER set");
            btFlag = 0;
          }
        }
      }
    }

    if (BTdata == "UPASS") {
      btFlag = 1;
      SerialBT.print("upOK");
      while (btFlag == 1) {
        if (SerialBT.available()) {
          BTdata = readSerialBT();
          UPASStemp = BTdata;
          // wait for password data
          if (UPASStemp != "") {
            SerialBT.print("Auth pass set");
            btFlag = 0;
          }
        }
      }
    }
    // add more funtions here i.e. wifi settings and possibly schedules
  }
  delay(20);
  if ((SSIDtemp != "") && (PASStemp != "") && (UPASStemp != "") && (USERtemp != "") && (btFlag == 0)) {
    btFlag = 2;
    const char* ssid2     = SSIDtemp.c_str();
    const char* password2 = PASStemp.c_str();
    WiFi.begin(ssid2, password2);
    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi Connected.");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    delay(250);
    wificredentials.begin("creds", false);
    wificredentials.putString("SSID", SSIDtemp);
    wificredentials.putString("PASS", PASStemp);
    wificredentials.putString("USER", USERtemp);
    wificredentials.putString("UPASS", UPASStemp);
    wificredentials.end();
    goToSleep();
  }
  Serial.print("SSID:  ");
  Serial.print(SSIDtemp);
  Serial.print("   ");
  Serial.print("Wifi PASS:  ");
  Serial.print(PASStemp);
  Serial.print("   ");
  Serial.print("USER: ");
  Serial.print(USERtemp);
  Serial.print("   ");
  Serial.print("User PASS:  ");
  Serial.println(UPASStemp);
}

//     Read data from bluetooth

String readSerialBT() {
  String readString = "";
  while (SerialBT.available()) {
    char c = SerialBT.read();
    readString += c;
  }
  return readString;
}

void SLEEPSCRN() {
  display.clear();
  display.drawXbm(0, 0, SLPW, SLPH, SLP);
  if (doorMode == "LEARN") {
    display.drawXbm(96, 32, LearnW, LearnH, LearnM);
  }
  if (doorMode == "BOTH") {
    display.drawXbm(96, 32, BTHAW, BTHAH, BTHA);
  }
  if (doorMode == "OUT") {
    display.drawXbm(96, 32, UPAW, UPAH, UPA);
  }
  if (doorMode == "IN") {
    display.drawXbm(96, 32, DWNAW, DWNAH, DWNA);
  }
  if (doorMode == "NONE") {
    display.drawXbm(96, 32, CROSSW, CROSSH, CROSS);
  }
  display.setBrightness(1);
  display.display();
}

void fromFlash() {
  // retrive wifi credentials from flash
  wificredentials.begin("creds", false);
  String ssidt = wificredentials.getString("SSID");
  if (ssidt != "") {
    ssid = ssidt.c_str();
  }
  String passt = wificredentials.getString("PASS");
  if (passt != "") {
    password = passt.c_str();
  }
  String usert = wificredentials.getString("USER");
  if (usert != "") {
    authUsername = usert.c_str();
  }
  String upasst = wificredentials.getString("UPASS");
  if (upasst != "") {
    authPassword = upasst.c_str();
  }
  wificredentials.end();
}

/**
   All door controlling here
 * */
void controlDoor() {
  // Indoor flow
  if (wakeupBit & GPIO_SEL_26) {
    controlDoorInside();
  }

  // Outdoor flow
  if (wakeupBit & GPIO_SEL_27) {
    controlDoorOutside();
  }
  if (wakeupBit & GPIO_SEL_34) {
    sleepState = 1;
  }
}

/**
   Control the door when cat goes out
*/
void controlDoorInside() {
  String readString;
  readString = readSerial2();
  readString.trim();
  if (readString.length() > 0) {
    if (database.contains(readString)) {

      //getSchedules();
      delay(1000);
      Serial.println("Data from Rfid Reader: " + readString);
      // Store last scan time
      lastScan = time(nullptr);
      Serial.println(lastScan);
      // Lock the door and go to sleep mode
      lockDoor(servo);
      // Turn off Rfid reader
      digitalWrite(RFID_ON, LOW);
      // retrive wifi credentials from flash
      //          wificredentials.begin("creds",false);
      //          String ssidt = wificredentials.getString("SSID");
      //          ssid = ssidt.c_str();
      //          String passt = wificredentials.getString("PASS");
      //          password = passt.c_str();
      //          wificredentials.end();
      Serial.print(ssid);
      Serial.print("  ");
      Serial.println(password);
      //setupWiFiServer(server, ssid, password);
      if (WiFi.status() == WL_CONNECTED) { //if wireless connected do online functions
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        printLocalTime();

        // Send notification
        state = OUT;
        sendNotification(readString.c_str(), state);
      }
      goToSleep();
    }
  }
  // Check time expired
  if (time(nullptr) - lastScan > MAX_WAIT_TIME * 5) {
    // Lock the door and go to sleep mode
    lockDoor(servo);
    // Turn off Rfid reader
    digitalWrite(RFID_ON, LOW);
    goToSleep();
  }
}

/**
   Control the door when cat comes in
*/
void controlDoorOutside() {
  String readString;
  readString = readSerial2();
  readString.trim();
  if (readString.length() > 1) {
    Serial.println("Data from Rfid Reader: " + readString);
    if (database.contains(readString)) {
      // Check schedule
      getSchedules();
      if (isScheduled()) {
        // Unlock the door
        unlockDoor(servo);
        std::time_t waiting = time(nullptr);
        while (time(nullptr) - waiting < MAX_WAIT_TIME) {
          // Wait until indoor motion sensor high
          if (digitalRead(MOTION_IN) == HIGH) {
            // retrive wifi credentials from flash
            //                wificredentials.begin("creds",false);
            //                String ssidt = wificredentials.getString("SSID");
            //                ssid = ssidt.c_str();
            //                String passt = wificredentials.getString("PASS");
            //                password = passt.c_str();
            //                wificredentials.end();
            //                Serial.print(ssid);
            //                Serial.print("  ");
            //                Serial.println(password);
            //setupWiFiServer(server, ssid, password);
            if (WiFi.status() == WL_CONNECTED) { // if wireless connected do online functions
              configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
              printLocalTime();
              // Send notification
              state = IN;
              sendNotification(readString.c_str(), state);
            }
            // Lock the door and go to sleep mode
            lockDoor(servo);
            // Turn off Rfid reader
            digitalWrite(RFID_ON, LOW);
            goToSleep();
            break;
          }
        }
        lockDoor(servo);
        // Turn off Rfid reader
        digitalWrite(RFID_ON, LOW);
        goToSleep();
      } else {
        goToSleep();
      }
    } else {
      goToSleep();
    }
  }
}

/**
   Set up GUI
*/
void setupGUI() {
  Wire.begin();
  // Initialze SSD1306 OLED display
  display.init();
  display.flipScreenVertically();
}


void controlGUI() {
  if ((digitalRead(SETUP_MODE) == 0) && (sleepState == 0)) {
    SLEEPSCRN();
    sleepState = 2;  // this is just to make sure sleep screen is only writen once
  }
  if ((digitalRead(SETUP_MODE) == 1) && (active == 0) && (sleepState == 2)) { //-----------go into gui from sleep or while awake reading doors
    sleepState = 1;
    delay(500);
  }

  if ((digitalRead(SETUP_MODE) == 1) && (active == 0) && (sleepState == 1)) {
    Serial.println("EXITING GUI");
    active = 0;
    lastDrawn = 100;
    sleepState = 0;
    delay(150);
    goToSleep();
  }

  if ((digitalRead(SETUP_MODE) == 1) && (active == 1)) {
    Serial.println("ENTERING LEARN MODE");
    active = 0;
    sleepState = 0;
  }
  if ((digitalRead(SETUP_MODE) == 1) && (active == 2)) {
    Serial.println("SETUP WIFI MODE");
    //wifisetupmode(); --- this is being replaced
  }
  if ((digitalRead(SETUP_MODE) == 1) && (active == 3)) {
    Serial.println("SETUP BT MODE");
    btSetup();
    while (BT == 1) {
      btHandle();
    }
    active = 0;
    sleepState = 0;
  }

  if ((digitalRead(SETUP_MODE) == 1) && (active == 4)) {
    Serial.println("LOCKING DOOR");
    // Lock the door and go to sleep mode
    lockDoor(servo);
    active = 0;
    sleepState = 0;
  }
  if ((digitalRead(SETUP_MODE) == 1) && (active == 5)) {
    Serial.println("UNLOCKING DOOR FOR 5 MINUTES");
    // Unlock the door and goto sleep // add timer on sleep to wake in 5mins and lock door then sleep.
    unlockDoor(servo);
    active = 0;
    sleepState = 0;
  }
  if ((digitalRead(SETUP_MODE) == 1) && (active == 6)) {
    Serial.println("KEEPING DOOR UNLOCKED");
    // Unlock the door and goto sleep // update wake to keep door in unlocked state
    unlockDoor(servo);
    active = 0;
    sleepState = 0;
  }
  if ((digitalRead(SETUP_MODE) == 1) && (active == 7)) {
    Serial.println("entering Door Id menu");
    //go into door id menu
    DoorID();
  }

  if (sleepState == 1) {
    if (digitalRead(EXTRA_BUTTON_1) == 1) { ///-------scroll screen right
      active = (active + 1) % 7;
      Serial.print("Screen#  ");
      Serial.println(active);
      delay(screenDelay); ///----------half second delay for debounce
    }

    if (digitalRead(EXTRA_BUTTON_2) == 1) { ///----------scroll scroll left
      active = (active - 1 + 7) % 7;
      Serial.print("Screen#  ");
      Serial.println(active);
      delay(screenDelay); ///----------half second delay for debounce
    }

    if ((active == 0) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      exitMenu();
      display.display();
      lastDrawn = active;
    }
    if ((active == 1) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      learnMode();
      display.display();
      lastDrawn = active;
    }
    if ((active == 2) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      wifiSetup();
      display.display();
      lastDrawn = active;
    }
    if ((active == 3) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      BTSetup();
      display.display();
      lastDrawn = active;
    }
    if ((active == 4) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      lockDoor();
      display.display();
      lastDrawn = active;
    }
    if ((active == 5) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      unlockDoor();
      display.display();
      lastDrawn = active;
    }
    if ((active == 6) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      unlockDoor();
      display.display();
      lastDrawn = active;
    }
    if ((active == 7) && (active != lastDrawn)) {
      display.clear();
      drawTop();
      drawBottom();
      doorIDmenu();
      display.display();
      lastDrawn = active;
    }
  }
}

void drawTop(){
    display.drawXbm(0, 0, LeftArrowW, LeftArrowH, LeftArrow);
    display.drawXbm(120, 0, RightArrowW, RightArrowH, RightArrow);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 0, "SELECT");
    display.drawHorizontalLine(0, 11, 128);
  }

void drawBottom(){
    display.drawHorizontalLine(0, 54, 128);
    display.drawXbm(1, 56, ClockW, ClockH, Clock);
    display.drawXbm(12, 56, WifiW, WifiH, WifiL);
    display.drawXbm(110, 56, BattW, BattH, Batt);
  }

void exitMenu(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"EXIT MENU");
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 22, "EXIT");
  }

void learnMode(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"LEARN MODE");
  display.drawXbm(48, 16, LearnW, LearnH, LearnM);
  }  

void wifiSetup(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"WIFI SETUP");
  display.drawXbm(34, 16, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  }

void BTSetup(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"BT SETUP");
  display.drawXbm(53, 16, BTW, BTH, BTS);
  }

void unlockDoor(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"UNLOCK DOOR");
  display.drawXbm(48, 16, unLockW, unLockH, unLock);
  }

void lockDoor(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"LOCK DOOR");
  display.drawXbm(48, 16, LockW, LockH, Lock);
  }

void doorIDmenu(){
  display.setFont(ArialMT_Plain_10);
  display.drawString(64,54,"SHOW ID CODE");
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 22, "DOOR ID"); 
  }

void doorIDtext(){
  display.setFont(ArialMT_Plain_16);
  display.drawString(64,28,DoorIDnum); 
  }

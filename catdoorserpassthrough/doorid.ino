void DooridManage(){
      int hasdrawn = 0; 
while(flag != 0){
  //--------------------handle screen displays
  if (flag == 1){   
        if (hasdrawn == 0){
        display.init();
  display.display();
      qrcode.init();
  qrcode.create(DoorIDnum);
hasdrawn = 1;
  Serial.println(flag);}
  }

  if (flag == 2){
    hasdrawn = 0;
 display.clear();
drawTop();
drawBottom();
doorIDtext();
display.display();
    }
  if (flag == 3){
    hasdrawn = 0;
display.clear();
drawTop();
drawBottom();
exitMenu();
display.display();
    }
  //----------------------handle button presses
    if (digitalRead(EXTRA_BUTTON_1) == 1){ ///-------scroll screen right
    flag ++;
    if (flag >= 4){
      flag = 1;}
      Serial.println(flag);
      delay(screenDelay);
    }
    
    if (digitalRead(EXTRA_BUTTON_2) == 1){ ///-------scroll screen right
    flag --;
    if (flag <= 1){
      flag = 3;}
      Serial.println(flag);
      delay(screenDelay);
      }
    if ((digitalRead(SETUP_MODE) == 1) && (flag == 3)){
    Serial.println("Exiting DoorID menu");
flag = 0;
    }
}}
  
void DoorID(){
  flag = 1;
  //while (flag != 0){
  DooridManage();
  active = 0;
  lastDrawn = 10;
  setupGUI();
  }//}

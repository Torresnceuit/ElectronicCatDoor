#include <Vector.h>
#include <Servo.h>
#include <WiFi.h>
#include "Helper.h"


/**
   Enable wake up by external triggers
*/
void enableDeepSleepWakeUp() {
  /*First we configure the wake up source
    We set our ESP32 to wake up for an external trigger.
    There are two types for ESP32, ext0 and ext1 .
    ext0 uses RTC_IO to wakeup thus requires RTC peripherals
    to be on while ext1 uses RTC Controller so doesnt need
    peripherals to be powered on.
    Note that using internal pullups/pulldowns also requires
    RTC peripherals to be turned on.
  */
  //  esp_sleep_enable_ext0_wakeup(GPIO_NUM_18, 1); //1 = High, 0 = Low
  //  esp_sleep_enable_ext0_wakeup(GPIO_NUM_19, 1); //1 = High, 0 = Low
  //  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, 1); //1 = High, 0 = Low
  //  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 1); //1 = High, 0 = Low
  //  esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 1); //1 = High, 0 = Low

  //If you were to use ext1, you would use it like
  esp_sleep_enable_ext1_wakeup(
    GPIO_SEL_34 |
    GPIO_SEL_27 |
    GPIO_SEL_26 ,
    ESP_EXT1_WAKEUP_ANY_HIGH);
}

/**
   Setup WiFi and connect to host
*/
void setupWiFiServer(WiFiServer& server, const char* ssid, const char* password) {
  // Connect to Wi-Fi network with SSID and password
  if ((ssid != "") && (password != "")){
  int trytimes = 0;
  if (trytimes < 15){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println(password);
    WiFi.begin(ssid, password);
    Serial.println("Waiting for WiFi");
    while ((WiFi.status() != WL_CONNECTED)||(trytimes > 15)) {
    delay(500);
    trytimes++;
    Serial.print(".");
  }}}

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

/**
    Print local time
*/
void printLocalTime()
{
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");
}

/**
   Blink led every defined time
 * */
void blinkLed(int pinNumber, int seconds) {
  digitalWrite(pinNumber, !digitalRead(pinNumber));
  delay(seconds);
}

/**
   Read data from rfid reader
*/
String readSerial2() {
  String readString = "";
  while (Serial2.available()) {
    char c = Serial2.read();
    readString += c;
  }
  return readString;
}

/**
   Connect to Wifi server
 * */
void connectToServer(Servo& servo, WiFiServer& server) {
  // Listen for incoming clients
  WiFiClient client = server.available();

  // Variable to store the HTTP request
  String header;

  // Decode HTTP GET value
  String valueString = String(5);
  int pos1 = 0;
  int pos2 = 0;

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;

        if (c == '\n') {

          // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            printWebPage(client, valueString);

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1 + 1, pos2);

              //Rotate the servo
              servo.write(valueString.toInt());
              Serial.println(valueString);
            }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {
            // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {
          // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

/**
   Print Web Page
 * */
void printWebPage(WiFiClient client, String valueString) {

  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();

  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
  client.println(".slider { width: 300px; }</style>");
  client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

  // Web Page
  client.println("</head><body><h1>ESP32 with Servo</h1>");
  client.println("<p>Position: <span id=\"servoPos\"></span></p>");
  client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + valueString + "\"/>");
  client.println("<p>Register Cat Id</p>");
  client.println("<input type=\"text\" />");
  client.println("<button onclick=\"register()\"> Register Cat Id </button>");

  client.println("<script>var slider = document.getElementById(\"servoSlider\");");
  client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
  client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
  client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
  client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");

  client.println("</body></html>");
}

/**
   Unlock the door
*/
void unlockDoor(Servo& servo) {
    servo.write(10);
    delay(1000);
}

/**
   Lock the door
*/
void lockDoor(Servo& servo) {
    servo.write(180);
    delay(1000);
}

/*
   Set up for wake up source
*/
void wakeupSetup(uint64_t wakeupBit, int rfidPin, Servo& servo) {
  // Wake up by Indoor Motion Sensor
  if (wakeupBit & GPIO_SEL_26) {
    indoorSetup(rfidPin, servo);
  }

  // Wake up by Outdoor Motion Sensor
  if (wakeupBit & GPIO_SEL_27) {
    outdoorSetup(rfidPin, servo);
  }
}

/*
   Set up for indoor flow
*/
void indoorSetup(int rfidPin, Servo& servo) {
  // Unlock the door
  unlockDoor(servo);
  // Turn on RFID Reader
  digitalWrite(rfidPin, HIGH);
  Serial.println("indoor sensor activated");
}

/*
   Set up for outdoor flow
*/
void outdoorSetup(int rfidPin, Servo& servo) {
  // Turn on RFID Reader
  digitalWrite(rfidPin, HIGH);
  Serial.println("outdoor sensor activated");
}

/**
 * Get Chip Id and return a string
 */
String getChipId(){

  uint64_t chipid;
  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.

  delay(3000);
  return String(chipid);
}

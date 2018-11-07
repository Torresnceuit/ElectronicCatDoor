#include <Servo.h>
#include <WiFi.h>
#include <ctime>


#define extrabutton1 18
#define extrabutton2 19
#define rfidon 12
#define motout 27
#define motin 26
#define rfid 25
#define battest 33
#define batteryone 39
#define batterytwo 36
#define setupmode 34

Servo servo;
int id[] = {};

// GPIO the servo is attached to
static const int servoPin = 13;

// Network credentials
const char* ssid     = /*"Your New Wi-Fi";*/"DigitalStock.co.nz";
const char* password = /*"171007501785";*/"Dreday2001";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

// Variables to store time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 11;
const int   daylightOffset_sec = 3600;

// Save last scan
std::time_t lastScan;

// Time wait until deep sleep in second
const int MAX_WAIT_TIME = 60 ;

// Variable to express that the cat is registered in system or not
bool isRegistered = true;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  lastScan = time(nullptr);

  //pinMode(servo, OUTPUT);
  pinMode(motin, INPUT);
  pinMode(motout, INPUT);
  pinMode(rfid, INPUT);
  pinMode(battest, OUTPUT);
  pinMode(batteryone, INPUT);
  pinMode(batterytwo, INPUT);
  pinMode(setupmode, INPUT);
  pinMode(rfidon, OUTPUT);
  pinMode(extrabutton1, INPUT);
  pinMode(extrabutton2, INPUT);

  // Attach servo to pin 13
  servo.attach(servoPin);

  setupWiFi();

  //Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  /*First we configure the wake up source
    We set our ESP32 to wake up for an external trigger.
    There are two types for ESP32, ext0 and ext1 .
    ext0 uses RTC_IO to wakeup thus requires RTC peripherals
    to be on while ext1 uses RTC Controller so doesnt need
    peripherals to be powered on.
    Note that using internal pullups/pulldowns also requires
    RTC peripherals to be turned on.
  */
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_18, 1); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_19, 1); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, 1); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 1); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 1); //1 = High, 0 = Low

  //If you were to use ext1, you would use it like
  //esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);
}

void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial2.write(Serial.read());   // Read it and send it out Serial2 (rx2 tx2)
  }

  if (digitalRead(motin) == HIGH) {
    Serial.println("indoor pir");
  }
  if (digitalRead(motout) == HIGH) {
    Serial.println("outdoor pir");
  }
  if (digitalRead(setupmode) == HIGH) {
    Serial.println("setupbutton");
  }
  if (digitalRead(extrabutton1) == HIGH) {
    Serial.println("extrabutton one");
  }
  if (digitalRead(extrabutton2) == HIGH) {
    Serial.println("extrabutton two");
  }

  // Blink RFID LED
  blinkRfid();

  // Control the door
  controlDoor();

  // After the door open, the inside/outside motion sensor is activated if the cat pass through
  // We connect to wifi host
  checkForWiFiConnect();

}

void checkForWiFiConnect() {
  if (digitalRead(motin) == HIGH && digitalRead(motout) == HIGH) {
    // Listen for imcoming clients
    connect();
    Serial.print("Cat went through");
  }
}

/**
   Setup WiFi and connect to host
*/
void setupWiFi() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

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
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

/**
   Blink Rfid led every 1 second
 * */
void blinkRfid() {
  digitalWrite(rfidon, HIGH);
}

/**
   All door controlling here
 * */
void controlDoor() {
  String readString;
  readString = rfidReader();
  readString.trim();
  if (readString.length() > 0) {
    if (readString == "900_079000177267") {
      servo.read() > 1 ? servo.write(1) : servo.write(180);
    }
    Serial.println("Data from Rfid Reader: " + readString);
    lastScan = time(nullptr);
    Serial.println(lastScan);
    isRegistered = false;
  }
  //Serial.println(time(nullptr) - lastScan);
  if (time(nullptr) - lastScan > MAX_WAIT_TIME) {
    //Go to sleep now
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
    Serial.println("This will never be printed");
  }

}

/**
   Read data from rfid reader
*/
String rfidReader() {
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
void connect() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            printWebPage(client);
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
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
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
void printWebPage(WiFiClient client) {

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

  client.println("<script>var slider = document.getElementById(\"servoSlider\");");
  client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
  client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
  client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
  client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");

  client.println("</body></html>");

}

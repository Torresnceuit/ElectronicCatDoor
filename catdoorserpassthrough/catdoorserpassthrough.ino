
#include <FirebaseESP32.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include<Vector.h>
#include <Servo.h>
#include <WiFi.h>
#include <ctime>
#include <Wire.h>
#include "Helper.h"
#include "BluetoothSerial.h"
#include <Preferences.h>
#include <qrcode.h>
#include "SSD1306.h"
#include "SSD1306Wire.h"
#include "images.h"
#include "base64.h"

SSD1306Wire  display(0x3c, 21, 22);
//SSD1306 display(0x3c, 21, 22);
QRcode qrcode (&display);

#define EXTRA_BUTTON_1 18
#define EXTRA_BUTTON_2 19
#define RFID_ON 12
#define MOTION_OUT 27
#define MOTION_IN 26
#define RFID 25
#define BATTEST 33
#define BATTERY_ONE 39
#define BATTERY_TWO 36
#define SETUP_MODE 34

// Define Firebase Authentication
#define FIREBASE_HOST "https://catdoornotification.firebaseio.com"
#define FIREBASE_AUTH "eB6MxH5rKlE7RquU0UyeFmGzhtgC8MwJT7DcEQAu"

enum CatState {
  IN,
  OUT,
  NOT_SCHEDULED
};

enum DoorState {
  LOCKED,
  UNLOCKED
};

Preferences wificredentials;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
// Global servo instance
Servo servo;

// GPIO the servo is attached to
static const int servoPin = 13;

// Network credentials
const char* ssid     = /*"Your New Wi-Fi";*/"DigitalStock.co.nz";
const char* password = /*"171007501785";*/"Dreday2001";

// Network static IP detail
IPAddress ip(192, 168, 178, 169);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

String apiServer = "http://catdoorapi.azurewebsites.net";
int port = 80;

// Set web server port number to 80
WiFiServer server(80);

// Variables to store time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 12;
const int   daylightOffset_sec = 3600;

// Save last scan
std::time_t lastScan;

// Count the boot
RTC_DATA_ATTR int bootCount = 0;

// Time wait until deep sleep in second
const int MAX_WAIT_TIME = 5 ;

// Variable to express that the cat is registered in system or not
bool isRegistered = true;

// Maximum number of cat id
const int MAX_CAT_ID = 5;

// DB for cat Id
Vector<String> database;

// Show the bit cause wake up
uint64_t wakeupBit;

// Cat state
CatState state;

// Authorization
char access_token[1024];
const char* authUsername = "torres@digitalstock.co.nz";
const char* authPassword = "Passc0de";

// Request
WiFiClient client;
HTTPClient http;

// JSON parser
StaticJsonBuffer<1024> jsonBuffer;
// GUI state
String doorMode = "BOTH"; // IN, OUT, BOTH, LEARN, NONE
int active = 0;
int lastDrawn = 7;
int sleepState = 0; // -- 0 = asleep 1 = awake 2 = ignore
int BT = 0;
int btFlag = 0;
String SSIDtemp = "";
String PASStemp = "";
String USERtemp = "";
String UPASStemp = "";
int screenDelay = 250;
int flag = 0;
String DoorIDnum = getChipId();

String firebaseUrl = String("/Commands/") + DoorIDnum + String("/State");

// Listening thread
TaskHandle_t TaskA;
bool running = true;
/**
   This method run firstly every time chip wake up
*/
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  // Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Get bit from wake up source
  wakeupBit = esp_sleep_get_ext1_wakeup_status();

  // Assign lastScan to the current time
  lastScan = time(nullptr);

  // Set up the pin mode for input and output
  setupPinMode();

  // Set up GUI
  setupGUI();

  // Attach servo to pin 13
  servo.attach(servoPin);

  // Setup WiFi server
  //WiFi.config(ip, gateway, subnet, primaryDNS, secondaryDNS);
  //setupWiFiServer(server, ssid, password);

  //Init and get the time
  //  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //  printLocalTime();

  // Allow wake up by external trigger
  enableDeepSleepWakeUp();

  // Set up devices for wake up
  wakeupSetup(wakeupBit, RFID_ON, servo);

  // Init database
  initDB();

  // Deep sleep for the first boot
  firstSleep();

  // Put background task
  setupThread();
}

/**
   This method run repeatly all time
*/
void loop() {

  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial2.write(Serial.read());   // Read it and send it out Serial2 (rx2 tx2)
  }

  // Main function to control the door
  controlDoor();

  // Control GUI
  controlGUI();
}

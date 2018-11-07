#include <Vector.h>
#ifndef __HELPER_H__
#define __HELPER_H__
  // Function prototypes
  void setupPinMode();
  void enableDeepSleepWakeUp();
  void setupWiFiServer(WiFiServer& server, const char* ssid, const char* password);
  void print_wakeup_reason();
  void printLocalTime();
  void blinkLed(int pinNumber);
  String readSerial2();
  void connectToServer(Servo& servo, WiFiServer& server);
  void printWebPage(WiFiClient client, String valueString);
  void lockDoor(Servo& servo);
  void unlockDoor(Servo& servo);
  void wakeupSetup(uint64_t wakeupBit, int rfidPin, Servo& servo);
  void indoorSetup(int rfidPin, Servo& servo);
  void outdoorSetup(int rfidPin, Servo& servo);
  String getChipId();
#endif

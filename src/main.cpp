#include <Arduino.h>
#include<ArduinoSimpleLogging.h>
#include <esp_system.h>

#include "PluckyWebServer.hpp"
#include "PluckyInterfaceSerial.hpp"
#include "PluckyInterfaceGroup.hpp"
#include "PluckyInterfaceTcpPort.hpp"

#include "config.hpp"
char userSettingStr_bleFlowControl[USER_SETTING_INT_STR_LEN];
char userSettingStr_tcpPort[USER_SETTING_INT_STR_LEN];



// Web Server using SPIFFS and IotWebConfig
PluckyWebServer webServer;

// Interface for the DE1
PluckyInterfaceSerial de1Serial(SERIAL_DE_UART_NUM);

// Interface Group including all controllers talking to the DE1
// Controllers in the main group:
// 0 = Serial USB
// 1 = Serial BLE
// 2 = TCP Port (a nested group that includes any/all open sockets)
#define NUM_CONTROLLERS 3
PluckyInterfaceGroup controllers(NUM_CONTROLLERS);

void setup() {
  sprintf(userSettingStr_bleFlowControl, DEFAULT_BLE_FLOW_CONTROL);
  sprintf(userSettingStr_tcpPort, DEFAULT_TCP_PORT);

  controllers[0] = new PluckyInterfaceSerial(SERIAL_USB_UART_NUM);
  controllers[1] = new PluckyInterfaceSerial(SERIAL_BLE_UART_NUM);
  controllers[2] = new PluckyInterfaceTcpPort(atoi(userSettingStr_tcpPort));
  controllers.doInit();

  webServer.doInit();

  Logger.info.println("Plucky initialization completed.");
}

void loop() {
  Serial.println("hi");
  Logger.debug.printf("User string = %s\n", userSettingStr_bleFlowControl);
  return;
  webServer.doLoop();
  de1Serial.doLoop();
  controllers.doLoop();
}

#include <Arduino.h>
#include <ArduinoSimpleLogging.h>

#include "PluckyWebServer.hpp"
#include "PluckyInterfaceSerial.hpp"
#include "PluckyInterfaceGroup.hpp"
#include "PluckyInterfaceTcpPort.hpp"

#include "config.hpp"
char *userSettingStr_bleFlowControl;
char *userSettingStr_tcpPort;
char *userSettingStr_promiscuous;

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

bool de1Initialized = false;

void setup() {
  Logger.addHandler(Logger.INFO, Serial);

  userSettingStr_bleFlowControl = new char[USER_SETTING_INT_STR_LEN];
  userSettingStr_tcpPort = new char[USER_SETTING_INT_STR_LEN];
  userSettingStr_promiscuous = new char[USER_SETTING_INT_STR_LEN];
  sprintf(userSettingStr_bleFlowControl, DEFAULT_BLE_FLOW_CONTROL);
  sprintf(userSettingStr_tcpPort, DEFAULT_TCP_PORT);
  sprintf(userSettingStr_promiscuous, DEFAULT_PROMISCUOUS);

  if(!SPIFFS.begin(true)){
      Logger.error.println("An Error has occurred while mounting SPIFFS");
  }
  controllers[0] = new PluckyInterfaceSerial(SERIAL_USB_UART_NUM);
  controllers[1] = new PluckyInterfaceSerial(SERIAL_BLE_UART_NUM);
  controllers[2] = new PluckyInterfaceTcpPort(atoi(userSettingStr_tcpPort));

  de1Serial.doInit();

  controllers.doInit();

  webServer.doInit();

  Logger.info.println("Plucky initialization completed.");
}

void loop() {
  webServer.doLoop();
  de1Serial.doLoop();
  controllers.doLoop();

  if (!de1Initialized) {
#if ENABLE_REMOTE_OOB
      de1Serial.writeAll((uint8_t *)"[E]00000001\n", 12);
      Logger.info.println("Remote Control enabled via factory command.");
#endif //  ENABLE_REMOTE_OOB
  de1Initialized = true;
  Logger.info.println("DE1 (re-)initialized.");
  }
}

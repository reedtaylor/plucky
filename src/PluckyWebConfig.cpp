#include "PluckyWebConfig.hpp"
#include "PluckyWebServer.hpp"
#include "config.hpp"

extern PluckyWebServer webServer;

PluckyWebConfig::PluckyWebConfig(WebServer *_ws) {
  // Initial name of the board. Used e.g. as SSID of the own Access Point.
  sprintf(_machineName, "DE1-%04X", (uint32_t)ESP.getEfuseMac());
 
  _iotWebConf = new IotWebConf(_machineName, &_dnsServer, _ws, WIFI_DEFAULT_PASSWORD, CONFIG_VERSION);
  _iotWebConf->setConfigPin(WIFI_CONFIG_PIN);
  _iotWebConf->setWifiConnectionCallback(wifiConnectedHandler_CB);
  _iotWebConf->setupUpdateServer(&_updateServer);
}

void PluckyWebConfig::doInit() {
  IotWebConfSeparator *separator_BLE = new IotWebConfSeparator("BLE Serial Config");
  IotWebConfParameter *bleFlowControlParam = new IotWebConfParameter(
    "Enable BLE CTS/RTS Flow Control<br/>(Set to 1 if Decent BLE is installed, 0 in most other situations)", 
    "bleFlowControl", userSettingStr_bleFlowControl, USER_SETTING_INT_STR_LEN, "number", "0 or 1", 
    NULL, "", true);

  _iotWebConf->addParameter(separator_BLE);
  _iotWebConf->addParameter(bleFlowControlParam);

  _iotWebConf->init();
}

void PluckyWebConfig::doLoop() {
  _iotWebConf->doLoop();
}

// Handler functions
void PluckyWebConfig::_wifiConnectedHandler() {
    // esp32 dhcp hostname bug https://github.com/espressif/esp-lwip/pull/6
    // workaround https://github.com/espressif/arduino-esp32/issues/2537#issuecomment-508558849
    char *updatedMachineName = _iotWebConf->getThingName(); // pulls in the machine name if overrridden previously via web config
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(updatedMachineName);      
}

void PluckyWebConfig::wifiConnectedHandler_CB() {
  webServer._webConfig->_wifiConnectedHandler();
} 

void PluckyWebConfig::handleConfig_CB() {
  webServer._webConfig->_iotWebConf->handleConfig();
}
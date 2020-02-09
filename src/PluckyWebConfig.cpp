#include "PluckyWebConfig.hpp"
#include "config.hpp"

PluckyWebConfig::PluckyWebConfig(WebServer *ws) {
  // Initial name of the board. Used e.g. as SSID of the own Access Point.
  sprintf(_machineName, "DE1-%04X", (uint32_t)ESP.getEfuseMac());
 
  _webServer = ws;
  _dnsServer = new DNSServer();
  _updateServer = new HTTPUpdateServer();

  _iotWebConf = new IotWebConf(_machineName, dnsServer, webServer, WIFI_DEFAULT_PASSWORD, CONFIG_VERSION);
  _iotWebConf->setConfigPin(WIFI_CONFIG_PIN);
  _iotWebConf->setWifiConnectionCallback(wifiConnectedHandler);
  _iotWebConf->setupUpdateServer(&httpUpdater);
}

void PluckyWebConfig::doInit() {
  IotWebConfSeparator *separator_BLE = new IotWebConfSeparator("BLE Serial Config");
  IotWebConfParameter *bleFlowControlParam = new IotWebConfParameter(
    "Enable BLE CTS/RTS Flow Control<br/>(Set to 1 if Decent BLE is installed, 0 in most other situations)", 
    "bleFlowControl", bleFlowControlStr, BLE_FLOW_CONTROL_STR_LEN, "number", "0 or 1", 
    BLE_FLOW_CONTROL_DEFAULT_STR, "", true);

  _iotWebConf->addParameter(separator_BLE);
  _iotWebConf->addParameter(bleFlowControlParam);

  _iotWebConf->init();
}

// Handler functions
void PluckyWebConfig::wifiConnectedHandler() {
    // esp32 dhcp hostname bug https://github.com/espressif/esp-lwip/pull/6
    // workaround https://github.com/espressif/arduino-esp32/issues/2537#issuecomment-508558849
    char *updatedMachineName = iotWebConf->getThingName(); // pulls in the machine name if overrridden previously via web config
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(updatedMachineName);      
}
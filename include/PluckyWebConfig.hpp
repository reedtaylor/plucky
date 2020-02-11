#ifndef _PLUCKY_WEB_CONFIG_HPP
#define _PLUCKY_WEB_CONFIG_HPP

#define IOTWEBCONF_DEBUG_TO_SERIAL
#define IOTWEBCONF_DEBUG_PWD_TO_SERIAL

#include <IotWebConf.h>
#include <DNSServer.h>

#include "PluckyInterface.hpp"
#include "PluckyWebServer.hpp"
#include "config.hpp"

class PluckyWebConfig {
public:
  PluckyWebConfig(WebServer *webServer);

  void doInit();
  void doLoop();

  void handleConfig();

private:
  char _machineName[33]; // initial name of the machine -- used as default AP SSID etc.

  IotWebConf _iotWebConf;
  DNSServer _dnsServer;
  WebServer *_webServer;
  HTTPUpdateServer _updateServer;

  void PluckyWebConfig::wifiConnectedHandler();
}

#endif

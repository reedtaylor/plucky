#ifndef _PLUCKY_WEB_CONFIG_HPP_
#define _PLUCKY_WEB_CONFIG_HPP_

#define IOTWEBCONF_DEBUG_TO_SERIAL
#define IOTWEBCONF_DEBUG_PWD_TO_SERIAL

#include <IotWebConf.h>
#include <DNSServer.h>

#include "PluckyInterface.hpp"
#include "PluckyWebServer.hpp"
#include "config.hpp"

class PluckyWebConfig {
public:
  PluckyWebConfig(WebServer *_ws);

  void doInit();
  void doLoop();

  // handlers
  static void handleConfig_CB();
  static void wifiConnectedHandler_CB();
  static void handleNotFound_CB();
  
protected:
  char _machineName[33]; // initial name of the machine -- used as default AP SSID etc.

  IotWebConf *_iotWebConf;
  DNSServer _dnsServer;
  HTTPUpdateServer _updateServer;


  void _wifiConnectedHandler();
  static void _handleNotFound();

  friend class PluckyWebServer;

};

#endif // _PLUCKY_WEB_CONFIG_HPP_

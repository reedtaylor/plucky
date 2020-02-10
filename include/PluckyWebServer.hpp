#ifndef _PLUCKY_WEB_SERVER_HPP
#define _PLUCKY_WEB_SERVER_HPP

#include <WebServer.h>
#include <SPIFFS.h>

#include <IotWebConfCompatibility.h> 

// IotWebConf for configuration portal
#include "PluckyWebConfig.hpp"

class PluckyWebServer {
public:
  PluckyWebServer();

  void doInit();
  void doLoop();

private:
  WebServer *_webServer;
  PluckyWebConfig *_webConfig;

  // Helper functions
  String _getContentType(String filename);
  bool _webPathExists(String path);

  // Handler functions
  bool handleFileRead(String path);

};

#endif // _PLUCKY_WEB_SERVER_HPP
#ifndef _PLUCKY_WEB_SERVER_HPP
#define _PLUCKY_WEB_SERVER_HPP

#include <WebServer.h>
#include <SPIFFS.h>
#include <IotWebConfCompatibility.h> 

class PluckyWebServer;

#include "PluckyWebConfig.hpp"

class PluckyWebServer {
public:
  PluckyWebServer(int port=80);
  ~PluckyWebServer();

  void doInit();
  void doLoop();

  // Handler wrappers
  static void handleNotFound_CB();
  static void handleWake_CB();
  static void handleSleep_CB();

protected:
  WebServer *_ws;
  PluckyWebConfig *_webConfig;

  // Helper functions
  String _getContentType(String filename);
  bool _webPathExists(String path);

  // Handlers
  bool _handleFileRead(String path);
  void _handleNotFound();

  friend class PluckyWebConfig;
};

#endif // _PLUCKY_WEB_SERVER_HPP
#include <WebServer.h>
#include <ArduinoSimpleLogging.h>
#include <WebServer.h>

#include "PluckyWebServer.hpp"
#include "PluckyInterfaceSerial.hpp"

extern PluckyWebServer webServer;

PluckyWebServer::PluckyWebServer(int port) {
  _ws = new WebServer(port);
  _webConfig = new PluckyWebConfig(_ws);
}

PluckyWebServer::~PluckyWebServer() {
  delete _ws;
  delete _webConfig;
}


String PluckyWebServer::_getContentType(String filename) {
  if (_ws->hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool PluckyWebServer::_webPathExists(String path){
  bool exists = false;
  File file = SPIFFS.open(path, "r");
  if(!file.isDirectory()){
    exists = true;
  }
  file.close();
  return exists;
}

bool PluckyWebServer::_handleFileRead(String path) {
//  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = _getContentType(path);
  String pathWithGz = path + ".gz";
  if (_webPathExists(pathWithGz) || _webPathExists(path)) {
    if (_webPathExists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    _ws->streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void PluckyWebServer::doInit() {
  _webConfig->doInit();

  // URL handlers for specific patterns
  _ws->on("/config", PluckyWebConfig::handleConfig_CB);
  _ws->on("/de1/wake", PluckyWebServer::handleWake_CB);
  _ws->on("/de1/sleep", PluckyWebServer::handleSleep_CB);


  // URL Handler for everything else
  // - If the request matches a path/file in SPIFFS, that will get served
  // - Else pass it along to the webconfig so that it can check for captive portal
  //   situations and not-404 on those.  But 404 otherwise.  
  _ws->onNotFound(PluckyWebServer::handleNotFound_CB);
}

extern PluckyInterfaceSerial de1Serial;
void PluckyWebServer::handleWake_CB() {
  Logger.info.print("Web initiated wake\n");
  de1Serial.writeAll((uint8_t *)"<B>02\n", 7);
  webServer._ws->sendHeader("Location", "/command.htm");
  webServer._ws->send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer._ws->client().stop(); // Stop is needed because we sent no content length
}

void PluckyWebServer::handleSleep_CB() {
  Logger.info.print("Web initiated sleep\n");
  de1Serial.writeAll((uint8_t *)"<B>00\n", 7);
  webServer._ws->sendHeader("Location", "/command.htm");
  webServer._ws->send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer._ws->client().stop(); // Stop is needed because we sent no content length
}


void PluckyWebServer::doLoop() {
  _webConfig->doLoop();
}

void PluckyWebServer::handleNotFound_CB() {
  webServer._handleNotFound();
}

void PluckyWebServer::_handleNotFound() {
  if (!_handleFileRead(_ws->uri())) {
    _webConfig->_iotWebConf->handleNotFound();
  }
}

/**
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf->handleCaptivePortal())
  {
    // Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>DE1 Plucky Web Configuration</title></head><body><h1>Welcome to the DE1 Plucky Web Portal!</h1>";
  s += "<div>Go to <a href='config'>configure page</a> to change settings.</div>";
  s += "</body></html>\n";

  webServer.send(200, "text/html", s);
}
**/

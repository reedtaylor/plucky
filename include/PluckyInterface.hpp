#ifndef _PLUCKY_INTERFACE_HPP_
#define _PLUCKY_INTERFACE_HPP_

#include <ArduinoSimpleLogging.h>
#include "config.hpp"

class PluckyInterface {
public:
  virtual void doInit();
  virtual void doLoop();

  virtual void begin() = 0;
  virtual void end() = 0;
  virtual bool available() = 0;
  virtual bool readAll() = 0;
  virtual bool availableForWrite(size_t len=0) = 0;
  virtual bool writeAll(const uint8_t *buf, size_t size) = 0;

protected:
  char _interfaceName[32] = "[unknown interface]";


  // helper functions 
  void trimBuffer(uint8_t *buf, uint16_t &len) {
    if (buf[len-1] == '\n') {
      if (buf[len-2] == '\r') {
        // convert CRLF to CR just to make everyone's lives easier
        // but also log a complaint about it
        buf[len-2] = '\n';
        len = len-1;
        Logger.warning.printf("Stripped CRLF from interface %s"), _interfaceName;
      }
    }
    if (len < READ_BUFFER_SIZE) {
      buf[len] = 0; // force null termination for convenience
    }
  }

  void debugHandler(uint8_t *buf, uint16_t &len) {
    if (strncmp((char *)buf, "HEAP", 9)) {
      Logger.debug.printf("Free Heap: %d", esp_get_free_heap_size());
      len=0;
    }
  }


};



#endif // _PLUCKY_INTERFACE_HPP_
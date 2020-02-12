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

};

// helper functions 
void trimBuffer(uint8_t *buf, uint16_t &len, char *interfaceName);
void debugHandler(uint8_t *buf, uint16_t &len);

#endif // _PLUCKY_INTERFACE_HPP_
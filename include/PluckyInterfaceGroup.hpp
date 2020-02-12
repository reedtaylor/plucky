#ifndef _PLUCKY_INTERFACE_GROUP_HPP_
#define _PLUCKY_INTERFACE_GROUP_HPP_

#include "PluckyInterface.hpp"

class PluckyInterfaceGroup : public PluckyInterface {
public:
  PluckyInterfaceGroup(uint8_t numInterfaces);
  ~PluckyInterfaceGroup();
  
  void doInit();
  void doLoop();

  void begin();
  void end();
  bool available();
  bool readAll();
  bool availableForWrite(size_t len=0);
  bool writeAll(const uint8_t *buf, size_t size);

  uint8_t getNumInterfaces();

  operator bool() {
    for (uint16_t i=0; i<_numInterfaces; i++) {
        if (_interfaces[i]) {
            return true;
        }
    }
    return false;
  }

  PluckyInterface * operator [](int i) const {return _interfaces[i];}
  PluckyInterface *& operator [](int i) {return _interfaces[i];}

protected:
  uint8_t _numInterfaces;
  PluckyInterface **_interfaces;
};


#endif // _PLUCKY_INTERFACE_GROUP_HPP_
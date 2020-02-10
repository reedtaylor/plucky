#ifndef _PLUCKY_INTERFACE_GROUP_HPP_
#define _PLUCKY_INTERFACE_GROUP_HPP_

#include "PluckyInterface.hpp"

class PluckyInterfaceGroup : public PluckyInterface {
public:
  PluckyInterfaceGroup(uint8_t num_interfaces);
  void doInit();
  void doLoop();

  void begin();
  void end();
  bool available();
  bool readAll();
  bool availableForWrite(size_t len=0);
  bool writeAll(const uint8_t *buf, size_t size);

  operator bool;

  PluckyInterface operator [](int i) const {return _interfaces[i];}
  PluckyInterface & operator [](int i) {return _interfaces[i];}

private:
  uint8_t _num_interfaces;
  PluckyInterface **_interfaces;
}


#endif // _PLUCKY_INTERFACE_GROUP_HPP_
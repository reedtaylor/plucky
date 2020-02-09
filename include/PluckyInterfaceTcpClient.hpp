#ifndef _PLUCKY_INTERFACE_TCP_CLIENT_HPP_
#define _PLUCKY_INTERFACE_TCP_CLIENT_HPP_

#include <WifiServer.h>

#include "PluckyInterface.hpp"

class PluckyInterfaceTcpClient : public PluckyInterface {
public:
  PluckyInterfaceTcpClient();
  void doInit();
  void doLoop();

  void begin();
  void end();
  bool available();
  bool readAll();
  bool availableForWrite(size_t len=0);
  bool writeAll(const uint8_t *buf, size_t size);

  operator bool;

private:
  uint8_t _num_interfaces;
  PluckyInterface **_interfaces;
}


#endif // _PLUCKY_INTERFACE_TCP_CLIENT_HPP_
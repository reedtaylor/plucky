#ifndef _PLUCKY_INTERFACE_TCP_PORT_HPP_
#define _PLUCKY_INTERFACE_TCP_PORT_HPP_

#include <WiFiServer.h>
#include <WiFiClient.h>

WiFiServer blah();

#include "PluckyInterfaceGroup.hpp"
#include "PluckyInterfaceTcpClient.hpp"

#define TCP_MAX_CLIENTS 6

class PluckyInterfaceTcpPort : public PluckyInterfaceGroup {
public:
  PluckyInterfaceTcpPort(uint16_t port);
  ~PluckyInterfaceTcpPort();

  void doLoop();

  void begin();
  void end();
  bool available();
  bool readAll();
  bool availableForWrite(size_t len=0);
  bool writeAll(const uint8_t *buf, size_t size);

  operator bool() {
    for (int i=0; i<TCP_MAX_CLIENTS; i++) {
      if (_interfaces[i]) {
          return true;
      }
    }
    return false;
  }

protected:
  uint16_t _tcpPort;
  WiFiServer _tcpServer;
};


#endif // _PLUCKY_INTERFACE_TCP_PORT_HPP_
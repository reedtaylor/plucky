#ifndef _PLUCKY_INTERFACE_TCP_PORT_HPP_
#define _PLUCKY_INTERFACE_TCP_PORT_HPP_

#include <WiFiServer.h>

#include "PluckyInterfaceGroup.hpp"
#include "PluckyInterfaceTcpClient.hpp"

#define TCP_MAX_CLIENTS 6

class PluckyInterfaceTcpPort : public PluckyInterfaceGroup {
public:
  PluckyInterfaceTcpPort(int uart_nr);
  void doInit();
  void doLoop();

  void begin();
  void end();
  int available();
  int readAll();
  bool availableForWrite(size_t len=0);
  size_t write(const uint8_t *buf, size_t size);

  operator bool;

private:
  WiFiServer *TCPServer; // (tcpPort, maxTcpClients)
  WiFiClient TCPClient[maxTcpClients];
}


#endif // _PLUCKY_INTERFACE_TCP_PORT_HPP_
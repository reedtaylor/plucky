#ifndef _PLUCKY_INTERFACE_TCP_CLIENT_HPP_
#define _PLUCKY_INTERFACE_TCP_CLIENT_HPP_

#include <WifiServer.h>
#include <WifiClient.h>

#include "PluckyInterface.hpp"
#include "config.hpp"

class PluckyInterfaceTcpClient : public PluckyInterface {
public:
  PluckyInterfaceTcpClient() {  
    _readBufIndex = 0;
    sprintf(_interfaceName, "TCP [no client]");
  };
  ~PluckyInterfaceTcpClient() { };

  void doInit();
  void doLoop();

  void begin();
  void end();
  bool available();
  bool readAll();
  bool availableForWrite(size_t len=0);
  bool writeAll(const uint8_t *buf, size_t size);

  void setTcpClient(WiFiClient newClient);

  bool connected() { return _tcpClient.connected(); }

  operator bool() {
    return _tcpClient.connected();
  }
 
protected:
  WiFiClient _tcpClient;
  uint8_t _readBuf[READ_BUFFER_SIZE];
  uint16_t _readBufIndex;
  char _interfaceName[32];
};


#endif // _PLUCKY_INTERFACE_TCP_CLIENT_HPP_
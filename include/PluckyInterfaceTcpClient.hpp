#ifndef _PLUCKY_INTERFACE_TCP_CLIENT_HPP_
#define _PLUCKY_INTERFACE_TCP_CLIENT_HPP_

#include <WifiServer.h>
#include <WifiClient.h>

#include "PluckyInterface.hpp"

class PluckyInterfaceTcpClient : public PluckyInterface {
public:
  PluckyInterfaceTcpClient(WiFiClient *client);
  void doInit();
  void doLoop();

  void begin();
  void end();
  bool available();
  bool readAll();
  bool availableForWrite(size_t len=0);
  bool writeAll(const uint8_t *buf, size_t size);

    operator bool()
    {
        return _tcpClient->connected();
    }

private:
  WiFiClient *_tcpClient;
  uint8_t _readBuf[READ_BUFFER_SIZE];
  uint16_t _readBufIndex;
  char _interfaceName[32];
};


#endif // _PLUCKY_INTERFACE_TCP_CLIENT_HPP_
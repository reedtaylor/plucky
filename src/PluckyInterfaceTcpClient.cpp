#include<ArduinoSimpleLogging.h>

#include "PluckyInterfaceTcpClient.hpp"
#include "PluckyInterfaceSerial.hpp"
#include "config.h"

PluckyInterfaceTcpClient::PluckyInterfaceTcpClient(WiFiClient *client) {
  _tcpClient = client;
  _readBufIndex = 0;
  sprintf (_interfaceName, "TCP[%s:%d]", _tcpClient->remoteIP().toString()), _tcpClient->remotePort();
}

void PluckyInterfaceTcpClient::doInit() {

}

void PluckyInterfaceTcpClient::doLoop() {

}

void PluckyInterfaceTcpClient::begin() {

}

void PluckyInterfaceTcpClient::end() {
  _tcpClient->stop();
  _readBufIndex = 0;
}

bool PluckyInterfaceTcpClient::available() {
  return _tcpClient->available();
}

bool PluckyInterfaceTcpClient::readAll() {
  if (_tcpClient->available()) {
      while (_tcpClient->available() && (_readBufIndex < READ_BUFFER_SIZE)) {
        _readBuf[_readBufIndex] = _tcpClient->read();
        _readBufIndex++;
        if (_readBuf[_readBufIndex - 1] == '\n') { 
          uint16_t sendLen = readBufIndex;
          readBufIndex = 0;

          trimBuffer(_readBuf, sendLen, _interfaceName);

          // Send to DE
          extern PluckyInterfaceSerial de1Serial;
          de1Serial.writeAll(_readBuf, sendLen);
        }
      }
    }
    if (_readBufIndex >= READ_BUFFER_SIZE) {
      Logger.warning.print("WARNING: BLE Read Buffer Overrun.  Buffer Contents: ");
      Logger.warning.write(_readBuf, _bufferSize);
      Logger.warning.println();
      _readBufIndex = 0;
    } 

}

bool PluckyInterfaceTcpClient::availableForWrite(size_t len=0) {
  return _tcpClient->connected();
}

bool PluckyInterfaceTcpClient::writeAll(const uint8_t *buf, size_t size) {
  return _tcpClient->write(buf, size);
}

operator PluckyInterfaceTcpClient::bool;
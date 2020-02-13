#include<ArduinoSimpleLogging.h>

#include "PluckyInterfaceTcpClient.hpp"
#include "PluckyInterfaceSerial.hpp"
#include "config.h"

void PluckyInterfaceTcpClient::doInit() {
  _readBufIndex = 0;
  begin();
}

void PluckyInterfaceTcpClient::doLoop() {
  readAll();
}

void PluckyInterfaceTcpClient::begin() {
  _readBufIndex = 0;
  Logger.info.printf("Started interface %s\n", _interfaceName);
}

void PluckyInterfaceTcpClient::end() {
  Logger.info.printf("Stopping interface %s\n", _interfaceName);
  _tcpClient.stop();
  _readBufIndex = 0;
  sprintf(_interfaceName, "TCP [no client]");
}

bool PluckyInterfaceTcpClient::available() {
  return _tcpClient.available();
}

bool PluckyInterfaceTcpClient::readAll() {
  bool didRead = false;
  if (_tcpClient.available()) {
    while (_tcpClient.available() && (_readBufIndex < READ_BUFFER_SIZE)) {
      didRead = true;
      _readBuf[_readBufIndex] = _tcpClient.read();
      _readBufIndex++;
      if (_readBuf[_readBufIndex - 1] == '\n') { 
        uint16_t sendLen = _readBufIndex;
        _readBufIndex = 0;

        trimBuffer(_readBuf, sendLen, _interfaceName);
        debugHandler(_readBuf, sendLen);

        // Send to DE
        extern PluckyInterfaceSerial de1Serial;
        de1Serial.writeAll(_readBuf, sendLen);
      }
    }
  }
  if (_readBufIndex >= READ_BUFFER_SIZE) {
    didRead = false;
    Logger.warning.printf("WARNING: Read Buffer Overrun on interface %s -- purging.\n", _interfaceName);
    Logger.debug.print("    Buffer contents: ");
    Logger.debug.write(_readBuf, READ_BUFFER_SIZE);
    Logger.debug.println();
    _readBufIndex = 0;
  } 
  return didRead;
}


bool PluckyInterfaceTcpClient::availableForWrite(size_t len) {
  return _tcpClient.connected();
}

bool PluckyInterfaceTcpClient::writeAll(const uint8_t *buf, size_t size) {
  return _tcpClient.write(buf, size);
}

void PluckyInterfaceTcpClient::setTcpClient(WiFiClient newClient) {
  _tcpClient = newClient;
  _readBufIndex = 0;
  sprintf (_interfaceName, "TCP[%s : %d]", _tcpClient.remoteIP().toString().c_str(), (int)_tcpClient.remotePort());  
  begin();
}
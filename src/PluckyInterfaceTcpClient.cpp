#include<ArduinoSimpleLogging.h>

#include "PluckyInterfaceTcpClient.hpp"

PluckyInterfaceTcpClient::PluckyInterfaceTcpClient(int uart_nr) {
  if (uart_nr == Serial) {
    // We are capturing the (open, global, probably USB) terminal Serial so just grab it
    _serial = &Serial;
  } else {
    _serial = new HardwareSerial(uart_nr);
  } 
  _readBufIndex = 0;
}

void PluckyInterfaceSerial::doInit() {


}
void PluckyInterfaceSerial::doLoop();

void PluckyInterfaceSerial::begin();
void PluckyInterfaceSerial::end();
int PluckyInterfaceSerial::available();
int PluckyInterfaceSerial::read();
bool PluckyInterfaceSerial::availableForWrite(size_t len=0);
size_t PluckyInterfaceSerial::write(const uint8_t *buf, size_t size);

operator PluckyInterfaceSerial::bool;
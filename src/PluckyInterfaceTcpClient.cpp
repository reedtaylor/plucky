#include <driver/uart.h>

#include "PluckyInterfaceSerial.hpp"

#if DEFAULT_BLE_FLOW_CONTROL
uart_hw_flowcontrol_t bleFlowControl = UART_HW_FLOWCTRL_CTS_RTS;
#define BLE_FLOW_CONTROL_DEFAULT_STR "1"

#else // !DEFAULT_BLE_FLOW_CONTROL 

uart_hw_flowcontrol_t bleFlowControl = UART_HW_FLOWCTRL_DISABLE;
#define BLE_FLOW_CONTROL_DEFAULT_STR "0"

#endif // DEFAULT_BLE_FLOW_CONTROL 

#define BLE_FLOW_CONTROL_STR_LEN 4
char bleFlowControlStr[BLE_FLOW_CONTROL_STR_LEN] = BLE_FLOW_CONTROL_DEFAULT_STR;
// These pin assignments are not used if BLE flow control is off
#define SERIAL_BLE_CTS_PIN 12
#define SERIAL_BLE_RTS_PIN 33


PluckyInterfaceSerial::PluckyInterfaceSerial(int uart_nr) {
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
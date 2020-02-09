#ifndef _PLUCKY_INTERFACE_SERIAL_HPP_
#define _PLUCKY_INTERFACE_SERIAL_HPP_

#define UART_BAUD 115200
#define SERIAL_PARAM SERIAL_8N1

#define  SERIAL_USB_UART_NUM UART_NUM_0


#define SERIAL_DE_UART_NUM UART_NUM_1
#define SERIAL_DE_RX_PIN 16
#define SERIAL_DE_TX_PIN 17

#define SERIAL_BLE_UART_NUM UART_NUM_2
#define SERIAL_BLE_RX_PIN 13  // bummer, mk3b uses the LED pin (13) for RX.  This is because of Feather M0 compatibility.
#define SERIAL_BLE_TX_PIN 27

#define DEFAULT_BLE_FLOW_CONTROL 1

class PluckyInterface {
public:
  virtual void doInit();
  virtual void doLoop();

  virtual void begin() = 0;
  virtual void end() = 0;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual bool availableForWrite(size_t len=0) = 0;
  virtual size_t write(const uint8_t *buf, size_t size) = 0;

  virtual operator bool = 0;

private:
  HardwareSerial *_serial;
  uint8_t _readBuf[BUFFER_SIZE];
  uint16_t _readBufIndex;
  int _uart_nr;

}


#endif // _PLUCKY_INTERFACE_HPP_
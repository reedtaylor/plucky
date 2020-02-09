#include <driver/uart.h>

#include "PluckyInterfaceSerial.hpp"

PluckyInterfaceSerial::PluckyInterfaceSerial(int uart_nr) {
    _uart_nr = uart_nr;
    if (uart_nr == SERIAL_USB_UART_NUM) {
        // We are capturing the (open, global, probably USB) terminal Serial so just grab it
        _serial = &Serial;
    } else {
        _serial = new HardwareSerial(uart_nr);
    }

    _readBufIndex = 0;
}

/*
void PluckyInterfaceSerial::doInit() {
    doInit(DEFAULT_BLE_FLOW_CONTROL);
}
*/

void PluckyInterfaceSerial::doInit(bool bleFlowControlEnable = DEFAULT_BLE_FLOW_CONTROL) {

    if (_uart_nr == SERIAL_USB_UART_NUM) {
        _serial.begin(UART_BAUD);
    } else if (_uart_nr == SERIAL_DE_UART_NUM) {
        _serial.begin(UART_BAUD, SERIAL_PARAM, SERIAL_DE_RX_PIN, SERIAL_DE_TX_PIN);
        gpio_pullup_en((gpio_num_t)SERIAL_DE_RX_PIN);  // suppress noise if DE not attached
    } else if (_uart_nr) == SERIAL_BLE_UART_NUM) {
        _serial.begin(UART_BAUD, SERIAL_PARAM, SERIAL_BLE_RX_PIN, SERIAL_BLE_TX_PIN);
        gpio_pullup_en((gpio_num_t)SERIAL_BLE_RX_PIN);  // suppress noise if BLE not attached

        if (bleFlowControlEnable) {
            Serial_USB.println("BLE HW flow control disabled");
        } else {
            Serial_USB.println("BLE HW flow control enabled");
            uart_set_hw_flow_ctrl(SERIAL_BLE_UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);
            uart_set_pin(SERIAL_BLE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, SERIAL_BLE_RTS_PIN, SERIAL_BLE_CTS_PIN);
            gpio_pulldown_en((gpio_num_t)SERIAL_BLE_CTS_PIN);  // this helps but is inconsistent. (not strong enough vs FTDI reset pullup) 
        }
    }



}
void PluckyInterfaceSerial::doLoop();

void PluckyInterfaceSerial::begin();
void PluckyInterfaceSerial::end();
bool PluckyInterfaceSerial::available();
bool PluckyInterfaceSerial::readAll();
bool PluckyInterfaceSerial::availableForWrite(size_t len=0);
bool PluckyInterfaceSerial::writeAll(const uint8_t *buf, size_t size);

operator PluckyInterfaceSerial::bool;
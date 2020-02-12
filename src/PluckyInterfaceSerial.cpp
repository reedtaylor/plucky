#include <driver/uart.h>
#include <HardwareSerial.h>
#include <ArduinoSimpleLogging.h>

#include "PluckyInterfaceSerial.hpp"
#include "PluckyInterfaceGroup.hpp"
#include "config.hpp"

extern char *userSettingStr_bleFlowControl;
extern char *userSettingStr_tcpPort;

PluckyInterfaceSerial::PluckyInterfaceSerial(int uart_nr) {
    _uart_nr = uart_nr;
    _readBufIndex = 0;
    if (uart_nr == SERIAL_USB_UART_NUM) {
        // We are capturing the (open, global, probably USB) terminal Serial so just grab it
        _serial = &Serial;
        sprintf(_interfaceName, "Serial_USB");
    } else {
        _serial = new HardwareSerial(uart_nr);
        if (_uart_nr == SERIAL_DE_UART_NUM) {
            sprintf(_interfaceName, "Serial_DE");
        } else {
            sprintf(_interfaceName, "Serial_BLE");
        }
    }
}

PluckyInterfaceSerial::~PluckyInterfaceSerial() {
    if (_uart_nr != SERIAL_USB_UART_NUM) {
        delete _serial;
    }
}

void PluckyInterfaceSerial::doInit() {
    begin();
    _readBufIndex = 0;
}

void PluckyInterfaceSerial::doLoop() {
    readAll();
}

void PluckyInterfaceSerial::begin() {
    _readBufIndex = 0;
    if (_uart_nr == SERIAL_USB_UART_NUM) {
        sprintf(_interfaceName, "Serial_USB");
        _serial->begin(UART_BAUD);
    } else if (_uart_nr == SERIAL_DE_UART_NUM) {
        sprintf(_interfaceName, "Serial_DE1");
        _serial->begin(UART_BAUD, SERIAL_PARAM, SERIAL_DE_RX_PIN, SERIAL_DE_TX_PIN);
        gpio_pullup_en((gpio_num_t)SERIAL_DE_RX_PIN);  // suppress noise if DE not attached
    } else if (_uart_nr == SERIAL_BLE_UART_NUM) {
        sprintf(_interfaceName, "Serial_BLE");
        _serial->begin(UART_BAUD, SERIAL_PARAM, SERIAL_BLE_RX_PIN, SERIAL_BLE_TX_PIN);
        gpio_pullup_en((gpio_num_t)SERIAL_BLE_RX_PIN);  // suppress noise if BLE not attached
        
        Logger.debug.printf("User string = %s", userSettingStr_bleFlowControl);
        if (atoi(userSettingStr_bleFlowControl) == 0) {
            Logger.info.println("BLE HW flow control disabled");
        } else {
            Logger.info.println("BLE HW flow control enabled");
            uart_set_hw_flow_ctrl(SERIAL_BLE_UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);
            uart_set_pin(SERIAL_BLE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, SERIAL_BLE_RTS_PIN, SERIAL_BLE_CTS_PIN);
            gpio_pulldown_en((gpio_num_t)SERIAL_BLE_CTS_PIN);  // this helps but is inconsistent. (not strong enough vs FTDI reset pullup) 
        }
    }
}

void PluckyInterfaceSerial::end() {
    _serial->end();
    _readBufIndex = 0;
}

bool PluckyInterfaceSerial::available() {
    return _serial->available();
}

bool PluckyInterfaceSerial::readAll() {
    bool didRead = false;
    while (_serial->available() && _readBufIndex < READ_BUFFER_SIZE) {
        _readBuf[_readBufIndex] = _serial->read();
        _readBufIndex++;
        didRead = true;
        if (_readBuf[_readBufIndex - 1] == '\n') { 
            uint16_t sendLen = _readBufIndex;
            _readBufIndex = 0;

            trimBuffer(_readBuf, sendLen, _interfaceName);
            debugHandler(_readBuf, sendLen);

            if (_uart_nr == SERIAL_DE_UART_NUM) {
                // Broeacast to all interfaces
                extern PluckyInterfaceGroup controllers;
                controllers.writeAll(_readBuf, sendLen);
            } else {
                // Send to DE
                extern PluckyInterfaceSerial de1Serial;
                de1Serial.writeAll(_readBuf, sendLen);
            }
        }
    }
    // If we are receiving messages longer than the buffer, this prevents overflow and/or blocking
    // Typically this only happens when noise is coming in on the BLE UART, or if baud rates    
    // are misconfigured, as the buffer size ought to be longer than the maximum DE1 does message length  
    if (_readBufIndex >= READ_BUFFER_SIZE) {
        Logger.warning.printf("WARNING: Read Buffer Overrun on interface %s -- purging.\n", _interfaceName);
        Logger.debug.print("    Buffer contents: ");
        Logger.debug.write(_readBuf, READ_BUFFER_SIZE);
        Logger.debug.println();
        _readBufIndex = 0;
    }
    return didRead;
}   

bool PluckyInterfaceSerial::availableForWrite(size_t len) {
    return (_serial->availableForWrite() > len);
}

bool PluckyInterfaceSerial::writeAll(const uint8_t *buf, size_t size) {
    bool didWrite = false;
    if (_serial->availableForWrite() > size) {
        // This if statement is used to prevent blocking in a case where (e.g. HW flow control) is causing
        // a UART to overflow its buffers.  The behavior is to drop writes and log warnings.  
        // ESP32 HardwareSerial writebuffer appears to be 127 bytes by default so this should be enough
        // to conatin any DE1 message (32 bytes of data max, meaning 64 bytes of ascii).
        // It is configurable via some deep uart calls, probably not necessary
        // 
        // Note: DAYBREAK Mk3b has a bug where it does not pull down CTSB.
        // (Pulldown is required to counteract the FTDI2232D's weak-pullup during reset.)
        // This would have the effect of blocking Plucky almost immediately if BLE is not installed
        // (or CTSB otherwise being externally grounded).  
        // BLE-free operation is probably not that exotic so blocking in that setup is Not Good.
        //
        // This code DOES work around that bug and prevent blocking in the bad-case scenario where:
        //  - BLE is not installed 
        //  - and CTSB is not grounded 
        //  - and BLE UART flow control is enabled.
        _serial->write(_readBuf, size);
        didWrite = true;
    } else {
        Logger.warning.printf("WARNING: Interface %s send buffer full", _interfaceName);
    }
    return didWrite;
}

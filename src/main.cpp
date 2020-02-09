#include <Arduino.h>
#include<ArduinoSimpleLogging.h>

#include "config.hpp"
#include "PluckyWebServer.hpp"
#include "PluckyInterfaceSerial.hpp"
#include "PluckyInterfaceGroup.hpp"
#include "PluckyInterfaceTcpPort.hpp"

// Web Server using SPIFFS and IotWebConfig
PluckyWebServer *webServer;

// Interface for the DE1
PluckyInterfaceSerial *de1Serial; 

// Interface Group including all controllers talking to the DE1
// Controllers in the main group:
// 0 = Serial USB
// 1 = Serial BLE
// 2 = TCP Port (a nested group that includes any/all open sockets)
#define NUM_CONTROLLERS 3
PluckyInterfaceGroup controllers(NUM_CONTROLLERS);

void setup() {
  de1Serial = new PluckyInterfaceSerial(SERIAL_DE_UART_NUM); 
  webServer = new PluckyWebServer();

  controllers[0] = new PluckyInterfaceSerial(SERIAL_USB_UART_NUM);
  controllers[1] = new PluckyInterfaceSerial(SERIAL_BLE_UART_NUM);
  controllers[2] = new PluckyInterfaceTcpPort(TCP_PORT);

  for (i=0; i<NUM_CONTROLLERS; i++) {
    controllers[i].doInit();
  }

  webServer->doInit();

  Logger.info.println("Plucky initialization completed.");
}

void loop() {
#ifdef WIFI
  iotWebConf->doLoop();

  if (WiFi.status() != WL_CONNECTED) {
#ifdef TCP
    if (TCPServer) {
      Serial_USB.println("Stopping TCP server");

      TCPServer.end();
    }
#endif // TCP
  } else {
#ifdef TCP
    if (!TCPServer) {
      TCPServer.begin(); // start TCP server
      TCPServer.setNoDelay(true);
      Serial_USB.println("TCP server enabled");
    }
#endif // TCP
  }
#endif // WIFI

#ifdef TCP
  // Check for new incoming connections
  if (TCPServer && TCPServer.hasClient()) { // find free/disconnected spot
     Serial_USB.print("New TCP client ");
     for (uint16_t i = 0; i < maxTcpClients; i ++) {
        if (!TCPClient[i]) {
          TCPClient [i] = TCPServer.available();
          readBufIndex_TCP[i] = 0;
          Serial_USB.print(" in slot: ");
          Serial_USB.println(i);
          break;
        } else {
          Serial_USB.print(".");
        }
        if (i == maxTcpClients - 1) { // no free/disconnected spot so reject it
            WiFiClient TmpserverClient = TCPServer.available();
            TmpserverClient.stop();
            Serial_USB.println("Too many TCP clients; new connection dropped");
        }
      }
  }     
#endif // TCP

  // Receive DE1 messages
  if (Serial_DE.available()) {
    while (Serial_DE.available() && (readBufIndex_DE < bufferSize)) {
      readBuf_DE[readBufIndex_DE] = Serial_DE.read();
      readBufIndex_DE ++;
      if (readBuf_DE[readBufIndex_DE - 1] == '\n') { 
        // let's be super deliberate about resetting the readBufIndex lest we forget to do it below
        uint16_t sendLen = readBufIndex_DE;
        readBufIndex_DE = 0;

        trimBuffer(readBuf_DE, sendLen, "Serial_DE");

        // Broadcast to Serial interfaces

        Serial_USB.write(readBuf_DE, sendLen);

        // BLE UART can support flow control (on by default, as the DE BLE adaptor uses it).  
        // We do not want Plucky to EVER block on writing a DE message to any controller.
        // So we must check for write-able-ness here (even though the DE BLE adaptor
        // does not block for very long under normal operation).
        // 
        // Also, DAYBREAK Mk3b has a bug where it does not pull down CTSB.
        // (Pulldown is required to counteract the FTDI2232D's weak-pullup during reset.)
        // This would have the effect of blocking Plucky almost immediately if BLE is not installed
        // (or CTSB otherwise being externally grounded).  
        // BLE-free operation is probably not that exotic so blocking in that setup is Not Good.
        //
        // This code DOES work around that bug and prevent blocking in the bad-case scenario where:
        //  - BLE is not installed 
        //  - and CTSB is not grounded 
        //  - and BLE UART flow control is enabled.
        // 
        // It is much better however to either externally ground CTSB if not needed, or to
        // to disable flow control on the BLE UART via the webcofig protal.

        if (Serial_BLE.availableForWrite() > sendLen) {
          // ESP32 HardwareSerial writebuffer appears to be 127 bytes by default so this should be enough
          // to conatin any DE1 message (32 bytes of data max, meaning 64 bytes of ascii).
          // It is configurable
          Serial_BLE.write(readBuf_DE, sendLen);
        } else {
          Serial_USB.println("WARNING: BLE send buffer full");
        }


        // Broadcast to TCP clients        
#ifdef TCP
        for (uint16_t i = 0; i < maxTcpClients; i ++) {
          if (TCPClient[i]) {
              TCPClient[i].write(readBuf_DE, sendLen);
          }
        }
#endif // TCP

      }
    }
  } 

  // If we are receiving messages longer than the buffer, this prevents overflow and/or blocking
  // Typically this only happens when noise is coming in on the BLE UART, or if baud rates
  // are misconfigured, as the buffer size ought to be longer than the maximum DE1 does message length  
  if (readBufIndex_DE >= bufferSize) {
      Serial_USB.printf("WARNING: DE Read Buffer Overrun, purging.  Buffer Contents: ");
      Serial_USB.write(readBuf_DE, bufferSize);
      Serial_USB.printf("\n");
      readBufIndex_DE = 0;
  }

  // Bridge BLE messages to DE1
  if (Serial_BLE.available()) {
    while (Serial_BLE.available() && (readBufIndex_BLE < bufferSize)) {
      readBuf_BLE[readBufIndex_BLE] = Serial_BLE.read();
      readBufIndex_BLE ++;
      if (readBuf_BLE[readBufIndex_BLE - 1] == '\n') { 
        // let's be super deliberate about resetting the readBufIndex lest we forget to do it below
        uint16_t sendLen = readBufIndex_BLE;
        readBufIndex_BLE = 0;

        trimBuffer(readBuf_BLE, sendLen, "Serial_BLE");

        // Send to DE
        Serial_DE.write(readBuf_BLE, sendLen);
        Serial_USB.print("   BLE: ");
        Serial_USB.write(readBuf_BLE, sendLen);
      }
    }
  }
  if (readBufIndex_BLE >= bufferSize) {
    Serial_USB.printf("WARNING: BLE Read Buffer Overrun.  Buffer Contents: ");
    Serial_USB.write(readBuf_BLE, bufferSize);
    Serial_USB.printf("\n");
    readBufIndex_BLE = 0;
  }

  // Bridge USB messages to DE1
  if (Serial_USB.available()) {
    while (Serial_USB.available() && (readBufIndex_USB < bufferSize)) {
      readBuf_USB[readBufIndex_USB] = Serial_USB.read();
      readBufIndex_USB ++;
      if (readBuf_USB[readBufIndex_USB - 1] == '\n') { 
        // let's be super deliberate about resetting the readBufIndex lest we forget to do it below
        uint16_t sendLen = readBufIndex_USB;
        readBufIndex_USB = 0;

        trimBuffer(readBuf_USB, sendLen, "Serial_USB");

        if (strncmp((char *)readBuf_USB, "WIFIRESET", 9)) {

        }

        // Send to DE
        Serial_DE.write(readBuf_USB, sendLen);
      }
    }
  }
  if (readBufIndex_USB >= bufferSize) {
    Serial_USB.printf("WARNING: BLE Read Buffer Overrun.  Buffer Contents: ");
    Serial_USB.write(readBuf_USB, bufferSize);
    Serial_USB.printf("\n");
    readBufIndex_USB = 0;
  }


#ifdef TCP
  // Bridge TCP messages to DE1
  for (byte i = 0; i < maxTcpClients; i ++) {
    if (TCPClient[i] && TCPClient[i].available()) {
      while (TCPClient[i].available() && (readBufIndex_TCP[i] < bufferSize)) {
        readBuf_TCP[i][readBufIndex_TCP[i]] = TCPClient[i].read();
        readBufIndex_TCP[i]++;
        if (readBuf_TCP[i][readBufIndex_TCP[i] - 1] == '\n') { 
          // let's be super deliberate about resetting the readBufIndex lest we forget to do it below
          uint16_t sendLen = readBufIndex_TCP[i];
          readBufIndex_TCP[i] = 0;

          char interfaceName[32];
          sprintf (interfaceName, "TCP[%d]", i);
          trimBuffer(readBuf_TCP[i], sendLen, interfaceName);

          // Send to DE
          Serial_DE.write(readBuf_TCP[i], sendLen);
        }
      }
    }
    if (readBufIndex_USB >= bufferSize) {
      Serial_USB.printf("WARNING: BLE Read Buffer Overrun.  Buffer Contents: ");
      Serial_USB.write(readBuf_USB, bufferSize);
      Serial_USB.printf("\n");
      readBufIndex_TCP[i] = 0;
    } 
  } 
#endif // TCP
}

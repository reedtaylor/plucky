#include <Arduino.h>

// config: ////////////////////////////////////////////////////////////

/*************************  General Config *******************************/

#define bufferSize 128
#define DEBUG

// Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "plucky_0.01"


/*************************  UART Config *******************************/

#define UART_BAUD 115200
#define SERIAL_PARAM SERIAL_8N1

#define SERIAL_DE_UART_NUM UART_NUM_1
#define SERIAL_DE_RX_PIN 16
#define SERIAL_DE_TX_PIN 17

#define SERIAL_BLE_UART_NUM UART_NUM_2
#define SERIAL_BLE_RX_PIN 13
#define SERIAL_BLE_TX_PIN 27
#define SERIAL_BLE_CTS_PIN 12
#define SERIAL_BLE_RTS_PIN 33


/*************************  WiFi & TCP Config *******************************/
#define WIFI
#ifdef WIFI

const char wifiInitialApPassword[] = "decentDE1";
#define IOTWEBCONF_DEBUG_TO_SERIAL
#define IOTWEBCONF_DEBUG_PWD_TO_SERIAL
const uint16_t wifiConfigPin = 26; // right next door to GND so easy to short
                                   // in an emergency - no button on the mk3b board :(

#define TCP
#ifdef TCP
const uint16_t maxTcpClients = 6;
const int tcpPort = 9090;
#endif // TCP
#endif // WIFI

// / end config ///////////////////////////////////////////////////////////

// Enumerate the hardware serial devices
#include <driver/uart.h>
// work around a linter bug
#ifndef UART_PIN_NO_CHANGE
#define UART_PIN_NO_CHANGE (-1)
#endif

HardwareSerial & Serial_USB = Serial;
uint8_t readBuf_USB[bufferSize];
uint16_t readBufIndex_USB = 0;

HardwareSerial Serial_DE(SERIAL_DE_UART_NUM);
uint8_t readBuf_DE[bufferSize];
uint16_t readBufIndex_DE = 0;

HardwareSerial Serial_BLE(SERIAL_BLE_UART_NUM);
uint8_t readBuf_BLE[bufferSize];
uint16_t readBufIndex_BLE = 0;

#ifdef WIFI
#include <IotWebConf.h>

char machineName[33]; // initial name of the machine -- used as default AP SSID etc.

DNSServer dnsServer;
WebServer webServer(80);
IotWebConf *iotWebConf;
void handleRoot();

#ifdef TCP
// Instantiate the TCP sockets for talking to the machine
WiFiServer TCPServer(tcpPort, maxTcpClients);
WiFiClient TCPClient[maxTcpClients];
uint8_t readBuf_TCP[maxTcpClients][bufferSize];
uint16_t readBufIndex_TCP[maxTcpClients];
#endif // TCP

#endif // WIFI

void trimBuffer(uint8_t *buf, uint16_t &len, const char* interfaceName="[unspecified]") {
  if (buf[len-1] == '\n') {
    if (buf[len-2] == '\r') {
      // convert CRLF to CR just to make everyone's lives easier
      // but let's complain about it I guess
      buf[len-2] = '\n';
      len = len-1;
      Serial_USB.print("WARNING: stripped CRLF from interface ");
      Serial_USB.println(interfaceName);
    }
  }
  if (len < bufferSize) {
    buf[len] = 0; // force null termination for convenience
  }
}

void wifiConnectedHandler() {
    // esp32 dhcp hostname bug https://github.com/espressif/esp-lwip/pull/6
    // workaround https://github.com/espressif/arduino-esp32/issues/2537#issuecomment-508558849
    char *updatedMachineName = iotWebConf->getThingName(); // pulls in the machine name if overrridden previously via web config
    WiFi.setHostname(updatedMachineName);      
}

void setup() {

    /******* Serial initialization ************/
    Serial_USB.begin(UART_BAUD);
    Serial_DE.begin(UART_BAUD, SERIAL_PARAM, SERIAL_DE_RX_PIN, SERIAL_DE_TX_PIN);
    Serial_BLE.begin(UART_BAUD, SERIAL_PARAM, SERIAL_BLE_RX_PIN, SERIAL_BLE_TX_PIN);
    // HW flow contol for the BLE adaptor.
    // https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/driver/driver/uart.h
    uart_set_hw_flow_ctrl(SERIAL_BLE_UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);
    uart_set_pin(SERIAL_BLE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, SERIAL_BLE_RTS_PIN, SERIAL_BLE_CTS_PIN);

    /******* Wifi initialization ***********/
#ifdef WIFI
    // Initial name of the board. Used e.g. as SSID of the own Access Point.
    sprintf(machineName, "DE1-%04X", (uint32_t)ESP.getEfuseMac());    
    iotWebConf = new IotWebConf(machineName, &dnsServer, &webServer, wifiInitialApPassword);
    iotWebConf->setConfigPin(wifiConfigPin);
    iotWebConf->setWifiConnectionCallback(wifiConnectedHandler);
    iotWebConf->init();

    // -- Set up required URL handlers on the web server.
    webServer.on("/", handleRoot);
    webServer.on("/config", []{ iotWebConf->handleConfig(); });
    webServer.onNotFound([](){ iotWebConf->handleNotFound(); });

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

#endif // WIFI

    delay(10);
    Serial_USB.println("Plucky initialization completed.");
}

void loop() {
#ifdef WIFI
  iotWebConf->doLoop();

  if (WiFi.status() != WL_CONNECTED) {
#ifdef TCP
    if (TCPServer) {
      printf("Stopping TCP server");

      TCPServer.end();
    }
#endif // TCP
  } else {
#ifdef TCP
    if (!TCPServer) {
      TCPServer.begin(); // start TCP server
      TCPServer.setNoDelay(true);
      Serial.println("TCP server enabled");
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
        Serial_BLE.write(readBuf_DE, sendLen);
        Serial_USB.write(readBuf_DE, sendLen);

#ifdef TCP
        // Broadcast to TCP clients
        for (uint16_t i = 0; i < maxTcpClients; i ++) {
          if (TCPClient[i]) {
              TCPClient[i].write(readBuf_DE, sendLen);
          }
        }
#endif // TCP

      }
    }
  } 
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

#ifdef WIFI
// Handle web requests to "/" path.
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf->handleCaptivePortal())
  {
    // Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  webServer.send(200, "text/html", s);
}
#endif // WIFI

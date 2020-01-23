#include <Arduino.h>

// config: ////////////////////////////////////////////////////////////

/*************************  General Config *******************************/

#define bufferSize 2048
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

/***
// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "espresso_plucky";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "decentDE1";
****/



#define MAX_TCP_CLIENTS 10
#define TCP_PORT 9090

/// end config ///////////////////////////////////////////////////////////

// Enumerate the hardware serial devices
#include "driver/uart.h"
HardwareSerial & Serial_USB = Serial;
HardwareSerial Serial_DE(SERIAL_DE_UART_NUM);
HardwareSerial Serial_BLE(SERIAL_BLE_UART_NUM);

#ifdef WIFI

/***
// Instantiate the web configuration interface
#include <IotWebConf.h>
DNSServer dnsServer;
WebServer webServer(80);
HTTPUpdateServer httpUpdater;

IotWebConf iotWebConf(thingName, &dnsServer, &webServer, wifiInitialApPassword);
***/

// Instantiate the TCP sockets for talking to the machine
#include <WiFi.h>
WiFiServer TCPServer(TCP_PORT);
WiFiClient TCPClient[MAX_TCP_CLIENTS];

#endif // ifdef WIFI

bool wifiReady = false;

void setup() {
  delay(100);

  /******* Serial initialization ************/
  Serial_USB.begin(UART_BAUD);
  Serial_DE.begin(UART_BAUD, SERIAL_PARAM, SERIAL_DE_RX_PIN, SERIAL_DE_TX_PIN);
  Serial_BLE.begin(UART_BAUD, SERIAL_PARAM, SERIAL_BLE_RX_PIN, SERIAL_BLE_TX_PIN);

  // Need to make a couple of "low level" esp-idf-esque calls to setup HW flow
  // contol for the BLE adaptor.  I think this is doable because it's provided
  // in the driver here:
  // https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/driver/driver/uart.h
  //
  // For now only enabling CTS because it's only the BLE adaptor side that might
  // use this.  May need to force the RTS pin low, we'll see
  // Reference: esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh);
  uart_set_hw_flow_ctrl(SERIAL_BLE_UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);

  // Reference: esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);
  uart_set_pin(SERIAL_BLE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, SERIAL_BLE_RTS_PIN, SERIAL_BLE_CTS_PIN);


  /******* Wifi initialization ***********/
#ifdef WIFI

/***
  
  // Initialize the web configuration interface
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  iotWebConf.init();
  // Set up required URL handlers on the web server.
  webServer.on("/", handleRoot);
  webServer.on("/config", [] { iotWebConf.handleConfig(); });
  webServer.onNotFound([]() {
    iotWebConf.handleNotFound();
  });
  Serial.println("Web configuration interface initialized.");

  ***/

    Serial.println();
    Serial.println();
    Serial.print("Connecting to wifi");
    WiFi.begin("myssid", "wpa2passwd");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    wifiReady = true;
    TCPServer.begin(); // start TCP server
    TCPServer.setNoDelay(true);
    Serial.println("TCP server enabled");


#endif // WIFI

  delay(10);
  Serial_USB.println("Plucky initialization completed.");
}

void loop() {

  uint8_t buf[bufferSize];
  uint16_t bufIndex = 0;

#ifdef WIFI

/***
  
  // Handler for the web configuration UI
  iotWebConf.doLoop();
***/

  if (wifiReady) {
    // Check for new incoming connections
    if (TCPServer.hasClient()) {
      //find free/disconnected spot
      for (byte i = 0; i < MAX_TCP_CLIENTS; i++) {
       if (!TCPClient[i] || !TCPClient[i].connected()) {
          if (TCPClient[i]) {
            TCPClient[i].stop();
         }
          TCPClient[i] = TCPServer.available();
          Serial_USB.print("New TCP client in slot: ");
          Serial_USB.println(i);
          break;
        }
       if (i == MAX_TCP_CLIENTS - 1 ) {
          //no free/disconnected spot so reject it
          WiFiClient TmpserverClient = TCPServer.available();
          TmpserverClient.stop();
         Serial_USB.println("Too many TCP clients; new connection dropped");
        }
      }
    }
  }

#endif // WIFI

  // Broadcast DE1 messages to controllers
  if (Serial_DE.available()) {
    bufIndex = 0;
    if (Serial_DE.available()) {
      byte c = Serial_DE.read(); // read char from DE UART
      Serial_BLE.write(c);
      Serial_USB.write(c);

#ifdef WIFI
      // Send to TCP clients
      if (wifiReady) {
        for (byte client_num = 0; client_num < MAX_TCP_CLIENTS; client_num++) {
         if (TCPClient[client_num]) {
            Serial_USB.write(c);
            //TCPClient[client_num].write(c); 
          } 
        }
      }
#endif // WIFI
      
      if(c == '\n') {
        //break; // we don't want to get into a state where buffers misalign with the message frames
      }
    }
/***
    // Send to serial interfaces
    Serial_BLE.write(buf, bufIndex);
    Serial_USB.write(buf, bufIndex);

#ifdef WIFI
    // Send to TCP clients
    if (wifiReady) {
      for (byte client_num = 0; client_num < MAX_TCP_CLIENTS; client_num++) {
        if (TCPClient[client_num]) {
          TCPClient[client_num].write(buf, bufIndex); 
        }
      }
    }
#endif // WIFI
***/
  }

  // Bridge BLE messages to DE1
  if (Serial_BLE.available()) {
    bufIndex = 0;
    while (Serial_BLE.available() && (bufIndex < (bufferSize - 1))) {
      buf[bufIndex] = Serial_BLE.read();
      bufIndex++;
      if(buf[bufIndex-1] == '\n') {
        break; // we don't want to get into a state where buffers misalign with the message frames
      }
    }

    // Send to DE
    Serial_DE.write(buf, bufIndex);
  }


  // Bridge USB messages to DE1
  if (Serial_USB.available()) {
    bufIndex = 0;
    while (Serial_USB.available() && (bufIndex < (bufferSize - 1))) {
      buf[bufIndex] = Serial_USB.read();
      bufIndex++;
      if(buf[bufIndex-1] == '\n') {
        break; // we don't want to get into a state where buffers misalign with the message frames
      }
  }

    // Send to DE
    Serial_DE.write(buf, bufIndex);
  }


#ifdef WIFI
  // Bridge TCP messages to DE1
  if (wifiReady) {
    for (byte client_num = 0; client_num < MAX_TCP_CLIENTS; client_num++) {
     if (TCPClient[client_num]) {
       if (TCPClient[client_num].available()) {
         bufIndex = 0;
         while (TCPClient[client_num].available() && (bufIndex < (bufferSize - 1))) {
           buf[bufIndex] = TCPClient[client_num].read();
           bufIndex++;
           if(buf[bufIndex-1] == '\n') {
              break; // we don't want to get into a state where buffers misalign with the message frames
            }         
         }

         // Send to DE
         Serial_DE.write(buf, bufIndex);
       }
     }
   }
  }
#endif // WIFI


}


/***
void wifiConnected()
{
  // Initialize the TCP server for talking to the machine
  Serial.println("Wifi Ready - enabling TCP server");
  wifiReady = true;
  TCPServer.begin(); // start TCP server
  TCPServer.setNoDelay(true);
}
***/

/*
   Web configuration: Handle web requests to "/" path.
*/
#if 0
#ifdef WIFI
void handleRoot()
{
  // Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>Plucky Web Interface</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  webServer.send(200, "text/html", s);
}
#endif // WIFI
#endif // 0

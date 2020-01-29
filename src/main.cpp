#include <Arduino.h>

/*************************  General Config ***********************/

const uint bufferSize = 2048;
#define DEBUG

/*************************  UART Config *******************************/
#include "driver/uart.h"
const int UART_BAUD = 115200;
const long SERIAL_PARAM = SERIAL_8N1;

const uart_port_t SERIAL_DE_UART_NUM = UART_NUM_1;
const uint SERIAL_DE_RX_PIN = 16;
const uint SERIAL_DE_TX_PIN = 17;

const uart_port_t SERIAL_BLE_UART_NUM = UART_NUM_2;
const uint SERIAL_BLE_RX_PIN = 13;
const uint SERIAL_BLE_TX_PIN = 27;
const uint SERIAL_BLE_CTS_PIN = 12;
const uint SERIAL_BLE_RTS_PIN = 33;

/****************  WiFi & WebServer Config *************************/
#define WEBSERVER

#ifdef WEBSERVER
#define WEBSOCKET

#ifdef WEBSOCKET
const uint maxWebsocketClients = 8;
#endif // WEBSOCKET

#endif // WEBSERVER
/***^^^^^^^^^^^^^^^^^^^^^  END OF CONFIGURATION ^^^^^^^^^^^^^^^^^^^^^***/

/***** SERIAL INSTANTIATION *****/
HardwareSerial &Serial_USB = Serial;
uint8_t readBuf_USB[bufferSize];
uint16_t readBufIndex_USB = 0;

HardwareSerial Serial_DE(SERIAL_DE_UART_NUM);
uint8_t readBuf_DE[bufferSize];
uint16_t readBufIndex_DE = 0;

HardwareSerial Serial_BLE(SERIAL_BLE_UART_NUM);
uint8_t readBuf_BLE[bufferSize];
uint16_t readBufIndex_BLE = 0;

/***** WEBSERVER INSTANTIATION *****/
#ifdef WEBSERVER

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "Update.h"

AsyncWebServer server(80);


#ifdef WEBSOCKET

// Wrapper class to add a readbuffer
class AsyncWebSocketClientRB: public AsyncWebSocketClient {
  public:
    AsyncWebSocketClientRB(AsyncWebServerRequest *request, AsyncWebSocket *server) 
      :AsyncWebSocketClient {request, server} { };
    ~AsyncWebSocketClientRB() {};
    uint8_t readBuf[bufferSize];
};

// Wrapper class use the readbuffer client
class AsyncWebSocketRB: public AsyncWebSocket {
  public:
    AsyncWebSocketRB(const String& url) 
      : AsyncWebSocket{ url } { };
    ~AsyncWebSocketRB() { };
    typedef LinkedList<AsyncWebSocketClientRB *> AsyncWebSocketClientLinkedList;
};

AsyncWebSocketRB ws("/ws");            // access at ws://[esp ip]/ws

#endif // WEBSOCKET

// simple wifi signin - to be replaced later
const char *ssid = "myssid";
const char *password = "wpa2passwd";
const char *http_username = "admin";
const char *http_password = "admin";

// flag to use from web update to reboot the ESP
bool shouldReboot = false;

void onRequest(AsyncWebServerRequest *request) {
  // Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len,
            size_t index, size_t total) {
  // Handle body
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index,
              uint8_t *data, size_t len, bool final) {
  // Handle upload
}

#ifdef WEBSOCKET
void onEvent(AsyncWebSocketRB *server, AsyncWebSocketClientRB *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if(type == WS_EVT_CONNECT) {
    // make sure there is room for this client
    server->cleanupClients(maxWebsocketClients-1);    
    Serial_USB.printf("WS connect [client=%u]\n", client->id());
    client->ping();    
  } else if(type == WS_EVT_DISCONNECT) {
    //client disconnected
    Serial_USB.printf("WS disconect [client=%u]\n", client->id());
  } else if(type == WS_EVT_ERROR) {
    //error was received from the other end
    Serial_USB.printf("WS error [client=%u error=%u]: %s\n", client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG) {
    //pong message was received (in response to a ping request maybe)
    Serial_USB.printf("WS pong [client=%u len=%u]: %s\n", client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA) {
    //data packet
    AwsFrameInfo * info = (AwsFrameInfo*)arg;

    // Copy the current frame into our read buffer 
    // (redundant for single-frame messages; keeping for simpliciy)
    for (uint16_t i=0; i<info->len; i++) {
      client->readBuf[info->index + i] =  data[i];
    }

    Serial_USB.printf("WS message [client=%u frame=%u final=%u type=%s length=%llu]: ", client->id(), info->num, info->final, (info->opcode == WS_TEXT)?"text":"binary", (info->index + info->len));
    Serial_USB.write(client->readBuf, (info->index + info->len));
    Serial_USB.printf("\n");
    if(info->final){
      // This message frame was the last (or only) one, so can now transmit the read buffer
      Serial_DE.write(client->readBuf, (info->index + info->len));
    }
  }
}
#endif // WEBSOCKET
#endif // WEBSERVER

void setup() {
  delay(100);

  /***** SERIAL INITIALIZATION *****/
  Serial_USB.begin(UART_BAUD);
  Serial_DE.begin(UART_BAUD, SERIAL_PARAM, SERIAL_DE_RX_PIN, SERIAL_DE_TX_PIN);
  Serial_BLE.begin(UART_BAUD, SERIAL_PARAM, SERIAL_BLE_RX_PIN,
                   SERIAL_BLE_TX_PIN);

  // Enabling HW flow control on BLE UART
  // https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/driver/driver/uart.h
  uart_set_hw_flow_ctrl(SERIAL_BLE_UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);
  uart_set_pin(SERIAL_BLE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
               SERIAL_BLE_RTS_PIN, SERIAL_BLE_CTS_PIN);

  /***** WEBSERVER INITIALIZATION ***********/
#ifdef WEBSERVER
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }

#ifdef WEBSOCKET
  // attach AsyncWebSocket
  server.addHandler(&ws);
#endif // WEBSOCKET  

  // Simple Firmware Update Form
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html",
                  "<form method='POST' action='/update' "
                  "enctype='multipart/form-data'><input type='file' "
                  "name='update'><input type='submit' value='Update'></form>");
  });

  server.on("/update", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              shouldReboot = !Update.hasError();
              AsyncWebServerResponse *response = request->beginResponse(
                  200, "text/plain", shouldReboot ? "OK" : "FAIL");
              response->addHeader("Connection", "close");
              request->send(response);
            },
            [](AsyncWebServerRequest *request, String filename, size_t index,
               uint8_t *data, size_t len, bool final) {
              if (!index) {
                Serial.printf("Update Start: %s\n", filename.c_str());
                if (!Update.begin()) {
                }
              }
              if (!Update.hasError()) {
                if (Update.write(data, len) != len) {
                  Update.printError(Serial);
                }
              }
              if (final) {
                if (Update.end(true)) {
                  Serial.printf("Update Success: %uB\n", index + len);
                } else {
                  Update.printError(Serial);
                }
              }
            });

  // Catch-All Handlers
  // Any request that can not find a Handler that canHandle it
  // ends in the callbacks below.
  server.onNotFound(onRequest);
  server.onFileUpload(onUpload);
  server.onRequestBody(onBody);

  server.begin();
#endif  // WEBSERVER

  delay(10);
  Serial_USB.println("Plucky initialization completed.");
}

void loop() {

#ifdef WEBSERVER
  if (shouldReboot) {
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }

  ws.cleanupClients(maxWebsocketClients);
#endif  // WEBSERVER

  // Broadcast DE1 messages to controllers
  if (Serial_DE.available()) {
    while (Serial_DE.available() && (readBufIndex_DE < bufferSize)) {
      readBuf_DE[readBufIndex_DE] = Serial_DE.read();
      readBufIndex_DE++;
      if (readBuf_DE[readBufIndex_DE - 1] == '\n') {
        // Send to serial interfaces
        Serial_BLE.write(readBuf_DE, readBufIndex_DE);
        Serial_USB.write(readBuf_DE, readBufIndex_DE);

#ifdef WEBSOCKET
        if (ws.count()) {
          if (ws.availableForWriteAll()) {
            ws.binaryAll(readBuf_DE, readBufIndex_DE);
          } else {
            // TODO we should sniff around and kill any clients in this situation  
            // to avoid the tragedy of the commons or whatever
            Serial_USB.println("Websockets blocking");
         }
        }   
#endif // WEBSOCKET

        readBufIndex_DE = 0;  /// <--- Hard to see but important line of code
      }
    }

    if (readBufIndex_DE >= bufferSize) {
      Serial_USB.printf("WARNING: DE Read Buffer Overrun.  Buffer Contents: ");
      Serial_USB.write(readBuf_DE, bufferSize);
      Serial_USB.printf("\n");
      readBufIndex_DE = 0;
    }
  }

  // Bridge BLE messages to DE1
  if (Serial_BLE.available()) {
    while (Serial_BLE.available() && (readBufIndex_BLE < bufferSize)) {
      readBuf_BLE[readBufIndex_BLE] = Serial_BLE.read();
      readBufIndex_BLE++;
      if (readBuf_BLE[readBufIndex_BLE - 1] == '\n') {
        // Send to DE
        Serial_DE.write(readBuf_BLE, readBufIndex_BLE);
        readBufIndex_BLE = 0;
      }
    }
    if (readBufIndex_BLE >= bufferSize) {
      Serial_USB.printf("WARNING: BLE Read Buffer Overrun.  Buffer Contents: ");
      Serial_USB.write(readBuf_BLE, bufferSize);
      Serial_USB.printf("\n");
      readBufIndex_BLE = 0;
    }
  }

  // Bridge USB messages to DE1
  if (Serial_USB.available()) {
    while (Serial_USB.available() && (readBufIndex_USB < bufferSize)) {
      readBuf_USB[readBufIndex_USB] = Serial_USB.read();
      readBufIndex_USB++;
      if (readBuf_USB[readBufIndex_USB - 1] == '\n') {
        // Send to DE
        Serial_DE.write(readBuf_USB, readBufIndex_USB);
        readBufIndex_USB = 0;
      }
    }
    if (readBufIndex_USB >= bufferSize) {
      Serial_USB.printf("WARNING: BLE Read Buffer Overrun.  Buffer Contents: ");
      Serial_USB.write(readBuf_USB, bufferSize);
      Serial_USB.printf("\n");
      readBufIndex_USB = 0;
    }
  }
}
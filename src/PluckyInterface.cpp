#include <ArduinoSimpleLogging.h>
#include <esp_system.h>

#include "PluckyInterface.hpp"

void trimBuffer(uint8_t *buf, uint16_t &len, char *interfaceName) {
  if (buf[len-1] == '\n') {
    if (buf[len-2] == '\r') {
      // convert CRLF to CR just to make everyone's lives easier
      // but also log a complaint about it
      buf[len-2] = '\n';
      len = len-1;
      Logger.warning.printf("Stripped CRLF from interface %s", interfaceName);
    }
  }
  if (len < READ_BUFFER_SIZE) {
    buf[len] = 0; // force null termination for convenience
  }
}

void debugHandler(uint8_t *buf, uint16_t &len) {
  if (strncmp((char *)buf, "HEAP", 4) == 0) {
    Logger.debug.printf("Free Heap: %d", esp_get_free_heap_size());
    len=0;
  }
}

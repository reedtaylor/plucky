#include <ArduinoSimpleLogging.h>
#include <esp_system.h>

#include "PluckyInterface.hpp"

void trimBuffer(uint8_t *buf, uint16_t &len, char *interfaceName) {
  // check for CRLF and convert to CR as a convenience
  // then also check for LF termination and correct if missing
  // NOTE: this will change len to reflect the revised message length
  // also log a WARNING as malformed messages should be corrected upstream
  if (buf[len-1] == '\n') {
    if (buf[len-2] == '\r') {
      buf[len-2] = '\n';
      len = len-1;
      Logger.warning.printf("WARNING: Stripped CRLF from interface %s\n", interfaceName);
    }
  } else {
    if (len < READ_BUFFER_SIZE) {
      // we handle the buffer overrun case below
      buf[len] = '\n';
      len = len + 1;
      Logger.warning.printf("WARNING: Appended missing LF for interface %s\n", interfaceName);
    }
  }

  // force null termination for convenience
  // this does NOT extend len as we don't need/want to transmit the null terminator
  if (len < READ_BUFFER_SIZE) {
    buf[len] = 0;  
  } else {
    buf[READ_BUFFER_SIZE-2] = '\n';
    buf[READ_BUFFER_SIZE-1] = 0;
    Logger.warning.printf("WARNING: read buffer clipped on interface %s\n", interfaceName);
  }
}

void debugHandler(uint8_t *buf, uint16_t &len) {
  if (strncmp((char *)buf, "HEAP", 4) == 0) {
    Logger.info.printf("{PLUCKY} Free Heap: %d\n", esp_get_free_heap_size());
    len=0;
  }

  if (strncmp((char *)buf, "PROMISCUOUS", 11) == 0) {
    extern char *userSettingStr_promiscuous;
    sprintf(userSettingStr_promiscuous, "1");
    Logger.info.println("{PLUCKY} PROMISCUOUS mode enabled");
    len=0;
  }
}

#ifndef _PLUCKY_INTERFACE_HPP_
#define _PLUCKY_INTERFACE_HPP_

class PluckyInterface {
public:
  virtual void doInit();
  virtual void doLoop();

  virtual void begin() = 0;
  virtual void end() = 0;
  virtual bool available() = 0;
  virtual bool readAll() = 0;
  virtual bool availableForWrite(size_t len=0) = 0;
  virtual bool writeAll(const uint8_t *buf, size_t size) = 0;

  virtual operator bool = 0;

}

void trimBuffer(uint8_t *buf, uint16_t &len, const char* interfaceName="[unspecified]") {
  if (buf[len-1] == '\n') {
    if (buf[len-2] == '\r') {
      // convert CRLF to CR just to make everyone's lives easier
      // but also log a complaint about it
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


#endif // _PLUCKY_INTERFACE_HPP_
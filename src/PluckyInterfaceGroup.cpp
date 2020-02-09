#include <driver/uart.h>

#include "PluckyInterfaceGroup.hpp"


PluckyInterfaceGroup::PluckyInterfaceGroup(uint8_t num_interfaces) {
    _num_interfaces = num_interfaces;
    _interfaces = new PluckyInterface*[_num_interfaces];
}

void PluckyInterfaceGroup::doInit() {
    for (uint16_t i=0; i<_num_interfaces, i++) {
        _interfaces[i]->doInit();
    }
}

void PluckyInterfaceGroup::doLoop() {
    for (uint16_t i=0; i<_num_interfaces, i++) {
        _interfaces[i]->doLoop();
    }
}

void PluckyInterfaceGroup::begin() {
    for (uint16_t i=0; i<_num_interfaces, i++) {
        _interfaces[i]->begin();
    }   
}
void PluckyInterfaceGroup::end(){
        for (uint16_t i=0; i<_num_interfaces, i++) {
        _interfaces[i]->end();
    }
}

int PluckyInterfaceGroup::available() {
    // Note this simple loop biases our checking to the lower-index interfaces.
    // However because readAll() visits and reads ALL interfaces in the group
    // this does not introduce a risk of starvation.
    for (uint16_t i=0; i<_num_interfaces, i++) {
        if (_interfaces[i]->available()) {
            return true;
        }
    }
    return false;
}

bool PluckyInterfaceGroup::readAll() {
    bool didRead = false;
    for (uint16_t i=0; i<_num_interfaces, i++) {
        if (_interfaces[i]->available()) {
            didRead = (didRead || _interfaces[i]->readAll());
        }
    }
    return didRead;
}

bool PluckyInterfaceGroup::availableForWrite(size_t len=0) {
    // Note this simple loop biases our checking to the lower-index interfaces.
    // However because writeAll() visits and writes ALL interfaces in the group
    // (leaving it to the leaf interfaces to handle buffering, flushing, closing, etc)
    // this does not introduce a risk of blockage or starvation.
    for (uint16_t i=0; i<_num_interfaces, i++) {
        if (_interfaces[i]->availableForWrite(len)) {
            return true;
        }
    }
    return false;


}

bool PluckyInterfaceGroup::write(const uint8_t *buf, size_t size) {
    bool didWrite = false;
    for (uint16_t i=0; i<_num_interfaces, i++) {
        didWrite = (didWrite || _interfaces[i]->writeAll(buf, size));
    }
    return didWrite;
}

operator PluckyInterfaceGroup::bool {
    for (uint16_t i=0; i<_num_interfaces, i++) {
        if (_interfaces[i]) {
            return true;
        }
    }
    return false;
}



#include <Driver/fluidnc_uart.h>

#include "UartTypes.h"

#include "Capture.h"

#include <sstream>
#include <algorithm>

class InputPin;
void uart_register_input_pin(int32_t uart_num, uint8_t pinnum, InputPin* object) {}

inline std::string uart_key(int32_t uart_num) {
    std::ostringstream key;
    key << "uart." << uart_num;

    return key.str();
}

void uart_discard_input(int32_t uart_num) {}

void uart_init(int32_t uart_num) {}

int32_t uart_buflen(int32_t uart_num) {
    auto        key = uart_key(uart_num);
    const auto& val = Inputs::instance().get(key);
    return val.size();
}

extern int32_t inchar();

int32_t uart_read(int32_t uart_num, uint8_t* buf, int32_t len, int32_t timeout_ms) {
    auto        key = uart_key(uart_num);
    const auto& val = Inputs::instance().get(key);
    auto        max = std::min(size_t(len), val.size());
    for (size_t i = 0; i < max; ++i) {
        buf[i] = uint8_t(val[i]);
    }
    std::vector<uint32_t> newval(val.begin() + max, val.end());
    Inputs::instance().set(key, newval);
    return int(max);
}

int32_t uart_write(int32_t uart_num, const uint8_t* buf, int32_t len) {
    auto key = uart_key(uart_num);
    auto val = Inputs::instance().get(key);
    for (size_t i = 0; i < len; ++i) {
        val.push_back(uint32_t(uint8_t(buf[i])));
    }
    Inputs::instance().set(key, val);
    return int(len);
}

void uart_mode(int32_t uart_num, unsigned long baud, UartData dataBits, UartParity parity, UartStop stopBits) {}

bool uart_half_duplex(int32_t uart_num) {
    return true;
}

void uart_xon(int32_t uart_num) {}

void uart_xoff(int32_t uart_num) {}

void uart_sw_flow_control(int32_t uart_num, bool on, int32_t xon_threshold, int32_t xoff_threshold) {}

bool uart_pins(int32_t uart_num, int32_t tx_pin, int32_t rx_pin, int32_t rts_pin, int32_t cts_pin) {
    return false;
}

int32_t uart_bufavail(int32_t uart_num) {
    return 128 - uart_buflen(uart_num);
}

bool uart_wait_output(int32_t uart_num, int32_t timeout_ms) {
    return true;
}

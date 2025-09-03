#include "UartTypes.h"
#include "Event.h"
#include <cstdint>

class InputPin;

void    uart_init(int32_t uart_num);
void    uart_mode(int32_t uart_num, unsigned long baud, UartData dataBits, UartParity parity, UartStop stopBits);
bool    uart_half_duplex(int32_t uart_num);
int32_t uart_read(int32_t uart_num, uint8_t* buf, int32_t len, int32_t timeout_ms);
int32_t uart_write(int32_t uart_num, const uint8_t* buf, int32_t len);
void    uart_xon(int32_t uart_num);
void    uart_xoff(int32_t uart_num);
void    uart_sw_flow_control(int32_t uart_num, bool on, int32_t xon_threshold, int32_t xoff_threshold);
bool    uart_pins(int32_t uart_num, int32_t tx_pin, int32_t rx_pin, int32_t rts_pin, int32_t cts_pin);
int32_t uart_buflen(int32_t uart_num);
int32_t uart_bufavail(int32_t uart_num);
void    uart_discard_input(int32_t uart_num);
bool    uart_wait_output(int32_t uart_num, int32_t timeout_ms);

void uart_register_input_pin(int32_t uart_num, uint8_t pinnum, InputPin* object);

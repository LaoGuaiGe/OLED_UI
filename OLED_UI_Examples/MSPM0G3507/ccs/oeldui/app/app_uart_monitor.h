#ifndef APP_UART_MONITOR_H
#define APP_UART_MONITOR_H

#include "stdbool.h"

void uart_monitor_loop(void);
void uart_monitor_request_exit(void);
void uart_monitor_request_clear(void);
bool uart_monitor_should_exit(void);

#endif

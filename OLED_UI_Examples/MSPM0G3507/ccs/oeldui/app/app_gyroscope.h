#ifndef APP_GYROSCOPE_H
#define APP_GYROSCOPE_H

#include "stdbool.h"

void gyroscope_loop(void);
void gyroscope_request_exit(void);
bool gyroscope_should_exit(void);

#endif

#ifndef _HW_KEY_H_
#define _HW_KEY_H_

typedef struct {
    unsigned int enter : 1;
    unsigned int back : 1;
    unsigned int encoder_sw : 1;
} KEY_STATUS;

KEY_STATUS key_scan(void);

#endif

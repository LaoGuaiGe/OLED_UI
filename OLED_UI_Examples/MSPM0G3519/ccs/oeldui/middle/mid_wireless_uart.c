#include "mid_wireless_uart.h"
#include "mid_debug_uart.h"
static struct {
    char buf[WIRELESS_RX_BUF_MAX];
    int  len;
} wireless_rx;

// PB23: 低电平=未连接，高电平=已连接，收发数据不影响电平

void wireless_uart_init(void)
{
    DL_UART_Main_enableInterrupt(UART_WIRELESS_INST,DL_UART_MAIN_INTERRUPT_RX);
    NVIC_ClearPendingIRQ(UART_WIRELESS_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_WIRELESS_INST_INT_IRQN);
    wireless_uart_clear_rx_data();
}

void wireless_uart_send_char(char ch)
{
    while (DL_UART_isBusy(UART_WIRELESS_INST));
    DL_UART_Main_transmitData(UART_WIRELESS_INST, ch);
}

void wireless_uart_send_string(char *str)
{
    while (*str) wireless_uart_send_char(*str++);
}

char* wireless_uart_get_rx_data(void)
{
    return wireless_rx.buf;
}

void wireless_uart_clear_rx_data(void)
{
    wireless_rx.len = 0;
    for(int i =0; i < WIRELESS_RX_BUF_MAX; i++ )
    {
        wireless_rx.buf[i] = 0;
    }
}

bool wireless_uart_is_linked(void)
{
    // 软件滤波：连续多次采样都为低电平才判定断开，避免RF瞬态断连导致状态闪烁
    // 该函数约30ms调用一次，阈值100≈持续3秒低电平才确认断开
    static uint8_t low_count = 0;
    static bool last_state = false;

    if (DL_GPIO_readPins(WIRELESS_LINK_PORT, WIRELESS_LINK_PIN) != 0) {
        low_count = 0;
        last_state = true;
    } else {
        if (low_count < 100) low_count++;
        if (low_count >= 100) last_state = false;
    }
    return last_state;
}

void UART_WIRELESS_INST_IRQHandler(void)
{
    // 读 IIDX 清中断标志（含 overrun）；再无条件排空 RX FIFO，避免溢出后 RX 卡死
    DL_UART_getPendingInterrupt(UART_WIRELESS_INST);
    while (!DL_UART_isRXFIFOEmpty(UART_WIRELESS_INST)) {
        char ch = DL_UART_Main_receiveData(UART_WIRELESS_INST);
        wireless_rx.buf[wireless_rx.len++] = ch;
        if (wireless_rx.len >= WIRELESS_RX_BUF_MAX - 1)
            wireless_rx.len = 0;
        // debug_uart_send_char(ch);//串口0回显
    }
}

bool wireless_uart_rf_on(void)
{
    // 关 UART7 RX 中断，保证命令字节连续发出不被打断（AT 解析依赖完整帧）
    NVIC_DisableIRQ(UART_WIRELESS_INST_INT_IRQN);
    wireless_uart_send_string("AT_RF=ON\r\n");
    while (DL_UART_isBusy(UART_WIRELESS_INST));
    NVIC_EnableIRQ(UART_WIRELESS_INST_INT_IRQN);
    return true;
}

bool wireless_uart_rf_off(void)
{
    // 关 UART7 RX 中断，保证命令字节连续发出不被打断（AT 解析依赖完整帧）
    NVIC_DisableIRQ(UART_WIRELESS_INST_INT_IRQN);
    wireless_uart_send_string("AT_RF=OFF\r\n");
    while (DL_UART_isBusy(UART_WIRELESS_INST));
    NVIC_EnableIRQ(UART_WIRELESS_INST_INT_IRQN);
    return true;
}

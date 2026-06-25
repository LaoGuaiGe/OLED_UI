#include "mid_font_burner.h"
#include "mid_debug_uart.h"
#include "hw_delay.h"
#include "stdio.h"

/**
 * @brief 通用烧录函数：擦除指定区域，串口接收数据写入 Flash
 * @param start_addr  起始地址
 * @param total_size  总字节数
 * @param name        烧录名称（用于串口提示）
 */
static void flash_burn(uint32_t start_addr, uint32_t total_size, const char* name)
{
    char msg[64];
    uint32_t write_addr = start_addr;
    uint32_t end_addr = start_addr + total_size;
    uint32_t total_written = 0;
    uint8_t page_buf[256];
    uint16_t buf_index = 0;
    uint32_t sector;

    // 禁用 UART 中断
    NVIC_DisableIRQ(UART_DEBUG_INST_INT_IRQN);

    // 1. 打印 Flash ID
    sprintf(msg, "FLASH ID: 0x%04X\r\n", W25Q128_readID());
    debug_uart_send_string(msg);

    // 2. 擦除目标区域
    sprintf(msg, "Erasing %s area...\r\n", name);
    debug_uart_send_string(msg);
    for (sector = start_addr / 4096; sector < (end_addr + 4095) / 4096; sector++)
    {
        W25Q128_erase_sector(sector);
    }
    debug_uart_send_string("Erase done.\r\n");

    // 3. 提示发送文件
    sprintf(msg, "READY: Send %s bin now.\r\n", name);
    debug_uart_send_string(msg);

    // 4. 等待 PC 端启动字节（ACK 握手协议：PC 收到 READY 后发 0x06 表示准备好）
    //    如果 PC 直接发数据（无握手），首字节当数据处理，兼容旧的直接发送方式
    uint8_t first_byte = DL_UART_Main_receiveDataBlocking(UART_DEBUG_INST);
    if (first_byte != 0x06) {
        // 非握手字节，当作数据首字节
        page_buf[buf_index++] = first_byte;
    }
    // 若收到 0x06，丢弃该字节，进入握手模式

    // 5. 接收数据，256 字节批量页写入，每页写完发 ACK '.'
    while (write_addr < end_addr)
    {
        page_buf[buf_index++] = DL_UART_Main_receiveDataBlocking(UART_DEBUG_INST);

        if (buf_index >= 256 || (write_addr + buf_index >= end_addr))
        {
            W25Q128_write_page(page_buf, write_addr, buf_index);
            write_addr += buf_index;
            total_written += buf_index;
            buf_index = 0;
            debug_uart_send_char('.');  // ACK: 通知 PC 可以发下一页
        }
    }

    // 6. 完成
    sprintf(msg, "\r\nDone! Total: %lu bytes\r\n", total_written);
    debug_uart_send_string(msg);

    // 恢复 UART 中断
    NVIC_EnableIRQ(UART_DEBUG_INST_INT_IRQN);

    while (1);
}

void font_burner_hzk16_run(void)
{
    flash_burn(FONT16_BASE_ADDR, FONT16_TOTAL_SIZE, "HZK16");
}

void font_burner_hzk12_run(void)
{
    flash_burn(FONT12_BASE_ADDR, FONT12_TOTAL_SIZE, "HZK12");
}

void font_burner_hzk20_run(void)
{
    flash_burn(FONT20_BASE_ADDR, FONT20_TOTAL_SIZE, "HZK20");
}

void font_burner_map_run(void)
{
    flash_burn(U2G_MAP_BASE_ADDR, U2G_MAP_SIZE, "U2G_MAP");
}

void font_burner_all_run(void)
{
    // ALL_FONTS.bin 按 Flash 地址对齐拼接: HZK16+U2G_MAP+HZK12+HZK20，中间0xFF填充
    // 总大小 = FONT20_BASE_ADDR + FONT20_TOTAL_SIZE = 0x080000 + 490682 = 1014970
    flash_burn(0x000000, FONT20_BASE_ADDR + FONT20_TOTAL_SIZE, "ALL_FONTS");
}

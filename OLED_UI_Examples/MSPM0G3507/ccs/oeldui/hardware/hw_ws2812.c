#include "hw_ws2812.h"
#include "hw_delay.h"

/*单个灯珠的需要传输的数据对应的比较值数组*/
uint32_t Single_WS2812B_Buffer[DATA_SIZE * WS2812B_NUM + 50] = {0}; // 24bit*灯珠数量 + Reset_Data(1.25us*50>50us)
volatile bool gChannel0InterruptTaken           = false;

/**
 * @brief TIM5_PWM_CH2&DMA1&PA1初始化
 * @param  arr重装载值（105 - 1）
 * @retval 无
 */
void PWM_WS2812B_Init(void)
{
	// DMA_Cmd(DMA1_Stream4, DISABLE); // 失能DMA1的6通道，因为一旦使能就开始传输
    NVIC_EnableIRQ(DMA_INT_IRQn);
    
    DL_Timer_stopCounter(WS2812_INST);

    //设置DMA搬运的起始地址
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)Single_WS2812B_Buffer);
    //设置DMA搬运的目的地址
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &WS2812_INST->COUNTERREGS.CC_01[GPIO_WS2812_C0_IDX]);
   
    //停止DMA
    DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
}

void WS2812B_Write_24Bits(uint16_t num, uint32_t GRB_Data)
{
	uint8_t i, j;
	for (j = 0; j < num; j++)
	{
		for (i = 0; i < DATA_SIZE; i++)
		{
			/*因为数据发送的顺序是GRB，高位先发，所以从高位开始判断，判断后比较值先放入缓存数组*/
			Single_WS2812B_Buffer[i + j * DATA_SIZE] = ((GRB_Data << i) & 0x800000) ? T1H : T0H;
		}
	}
}

void WS2812B_Write_24Bits_independence(uint16_t num, uint32_t *GRB_Data)
{
	uint8_t i, j;
	for (j = 0; j < num; j++)
	{
		for (i = 0; i < DATA_SIZE; i++)
		{
			/*因为数据发送的顺序是GRB，高位先发，所以从高位开始判断，判断后比较值先放入缓存数组*/
			Single_WS2812B_Buffer[i + j * DATA_SIZE] = ((GRB_Data[j] << i) & 0x800000) ? T1H : T0H;
		}
	}
}

void WS2812B_Show(void)
{
	/* 移植时此处对应的通道和中断标志都需要更改 */

	DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, DATA_SIZE * WS2812B_NUM + 50);

	/*开启DMA和TIM5开始传输*/
	DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
	DL_Timer_startCounter(WS2812_INST);
    
    gChannel0InterruptTaken = false;
    DL_DMA_startTransfer(DMA, DMA_CH0_CHAN_ID);

	while (gChannel0InterruptTaken == false) {
        __WFE();
    }

	DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
	DL_Timer_stopCounter(WS2812_INST);
}

// N个灯珠发红光
void PWM_WS2812B_Red(uint16_t num)
{
	WS2812B_Write_24Bits(num, 0x00ff00); 
	WS2812B_Show();
}
// N个灯珠发绿光
void PWM_WS2812B_Green(uint16_t num)
{
	WS2812B_Write_24Bits(num, 0xff0000);
	WS2812B_Show();
}
// N个灯珠发蓝光
void PWM_WS2812B_Blue(uint16_t num)
{
	WS2812B_Write_24Bits(num, 0x0000ff);
	WS2812B_Show();
}

/*呼吸灯*/
void set_ws2812_breathing(uint8_t index)
{
	int i = 0;
	switch (index)
	{
	case 0: /* red */
		for (i = 0; i < 254; i += 2)
		{
			WS2812B_Write_24Bits(64, (uint32_t)(0x00 << 16 | i << 8 | 0x00));
			WS2812B_Show();
			delay_ms(10);
		}
		for (i = 254; i > 0; i -= 2)
		{
			WS2812B_Write_24Bits(64, (uint32_t)(0x00 << 16 | i << 8 | 0x00));
			WS2812B_Show();
			delay_ms(10);
		}
		break;
	case 1: /* green */
		for (i = 0; i < 254; i += 2)
		{
			WS2812B_Write_24Bits(64, (uint32_t)(i << 16 | 0x00 << 8 | 0x00));
			WS2812B_Show();
			delay_ms(10);
		}
		for (i = 254; i > 0; i -= 2)
		{
			WS2812B_Write_24Bits(64, (uint32_t)(i << 16 | 0x00 << 8 | 0x00));
			WS2812B_Show();
			delay_ms(10);
		}
		break;
	case 2: /* blue */
		for (i = 0; i < 254; i += 2)
		{
			WS2812B_Write_24Bits(64, (uint32_t)(0x00 << 16 | 0x00 << 8 | i));
			WS2812B_Show();
			delay_ms(10);
		}
		for (i = 254; i > 0; i -= 2)
		{
			WS2812B_Write_24Bits(64, (uint32_t)(0x00 << 16 | 0x00 << 8 | i));
			WS2812B_Show();
			delay_ms(10);
		}
		break;
	}
}

void DMA_IRQHandler(void)
{
    /* Example interrupt code -- just used to break the WFI in this example */
    switch (DL_DMA_getPendingInterrupt(DMA)) {
        case DL_DMA_EVENT_IIDX_DMACH0:
            gChannel0InterruptTaken = true;
            break;
        default:
            break;
    }
}
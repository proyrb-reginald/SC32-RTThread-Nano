/**
 * @brief RT-Thread的初始化配置代码的移植。
 * @details 初始化片上外设与板载外设。
 * @file board.c
 * @author proyrb
 * @date 2025/8/7
 * @note
 */

/********** 导入需要的头文件 **********/

#include <sc32_conf.h>
#include <rtthread.h>
#include <st7789v.h>

/********** 实现中断配置代码 **********/

/**
 * @brief 实现SysTick中断处理中进行tick更新。
 * @param
 * @retval
 * @warning
 * @note
 */
__attribute__((interrupt)) void SysTick_Handler(void) {
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

/**
 * @brief 实现UART5中断处理。
 * @param
 * @retval
 * @warning
 * @note
 */

__attribute__((interrupt)) void UART1_3_5_IRQHandler(void) {
    rt_interrupt_enter();
    rt_kprintf("in\n");
    if (UART_GetFlagStatus(UART5, UART_Flag_RX)) {
        UART_ClearFlag(UART5, UART_Flag_RX);
    }
    rt_interrupt_leave();
}

__attribute__((interrupt)) void DMA0_IRQHandler(void) {
    st7789v_dma_irq();
    DMA_ClearFlag(DMA0, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
}

__attribute__((interrupt)) void DMA1_IRQHandler(void) {
    // w25q64_dma_irq();
    DMA_ClearFlag(DMA1, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
}

__attribute__((interrupt)) void DMA2_IRQHandler(void) {
    // w25q64_dma_irq();
    DMA_ClearFlag(DMA2, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
}

/********** 实现初始化配置代码 **********/

/**
 * @brief 初始化系统级的时钟配置。
 * @param
 * @retval
 * @warning
 * @note 在rt_hw_board_init中调用一次。
 */
static void rcc_init(void) {
    /* 尝试解锁时钟 */
    if (RCC_Unlock(0xFF) != SUCCESS)
        while (1) {}

    /* 配置系统时钟树 */
    RCC_ITConfig(DISABLE);
    RCC_HXTCmd(DISABLE);
    RCC_LXTCmd(DISABLE);
    RCC_HIRCCmd(ENABLE);
    RCC_LIRCCmd(DISABLE);
    RCC_HIRCDIV1Cmd(ENABLE);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* 切换系统时钟源 */
    if (RCC_SYSCLKConfig(RCC_SYSCLKSource_HIRC) != SUCCESS)
        while (1) {}

    /* 配置并开启SysTick */
    RCC_SystickCLKConfig(RCC_SysTickSource_HCLK_DIV8);
    RCC_SystickSetCounter(8000);
    NVIC_EnableIRQ(SysTick_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 0);
    RCC_SystickCmd(ENABLE);
}

/**
 * @brief 初始化用于输出日志信息的串口外设。
 * @param
 * @retval 0 一般固定。
 * @warning 在port_rcc_init之后调用。
 * @note 在rt_hw_board_init中调用一次。
 */
static void uart5_init(void) {
    /* 配置并开启APB0外设总线时钟 */
    RCC_APB0Config(RCC_HCLK_Div1);
    RCC_APB0Cmd(ENABLE);
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_UART5, ENABLE);

    /* 配置并开启uart5 */
    UART_InitTypeDef UART_InitStruct;
    UART_InitStruct.UART_ClockFrequency = 64000000;
    UART_InitStruct.UART_BaudRate       = 115200;
    UART_InitStruct.UART_Mode           = UART_Mode_10B;
    UART_Init(UART5, &UART_InitStruct);
    // UART_ITConfig(UART5, UART_IT_EN | UART_IT_RX, ENABLE);
    UART_PinRemapConfig(UART5, UART_PinRemap_Default);
    // NVIC_SetPriority(UART1_3_5_IRQn, 2);
    // NVIC_EnableIRQ(UART1_3_5_IRQn);
    UART_TXCmd(UART5, ENABLE);
    // UART_RXCmd(UART5, ENABLE);
    Printf_UartInit(UART5);
}

/**
 * @brief 初始化用于控制板载外设的GPIO外设。
 * @param
 * @retval 0 一般固定。
 * @warning 在port_rcc_init之后调用。
 * @note 在rt_hw_board_init中调用一次。
 */
static void gpio_init(void) {
    // lcd驱动引脚配置
    GPIO_InitTypeDef GPIOInit_PC12_Struct;  // 复位引脚
    GPIOInit_PC12_Struct.GPIO_Pin        = GPIO_Pin_12;
    GPIOInit_PC12_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PC12_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOC, &GPIOInit_PC12_Struct);
    GPIO_InitTypeDef GPIOInit_PC13_Struct;  // 数据命令引脚
    GPIOInit_PC13_Struct.GPIO_Pin        = GPIO_Pin_13;
    GPIOInit_PC13_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PC13_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOC, &GPIOInit_PC13_Struct);
    GPIO_InitTypeDef GPIOInit_PC14_Struct;  // 片选引脚
    GPIOInit_PC14_Struct.GPIO_Pin        = GPIO_Pin_14;
    GPIOInit_PC14_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PC14_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOC, &GPIOInit_PC14_Struct);

    // w25q64引脚配置
    GPIO_InitTypeDef GPIOInit_PB13_Struct;  // 片选引脚
    GPIOInit_PB13_Struct.GPIO_Pin        = GPIO_Pin_13;
    GPIOInit_PB13_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PB13_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOB, &GPIOInit_PB13_Struct);
}

/**
 * @brief 初始化用于输出LCD数据的SPI外设。
 * @param
 * @retval
 * @warning 使用INIT_BOARD_EXPORT宏自动初始化。
 * @note
 */
static void spi2_init(void) {
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_QTWI2, ENABLE);  // 使能时钟
    SPI_InitTypeDef SPI_InitStruct;                        // 初始化结构体
    SPI_InitStruct.SPI_Mode      = SPI_Mode_Master;        // 设置工作模式为主模式
    SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8B;        // 设置数据大小为8位
    SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;         // 设置时钟相位为第一个边沿采样
    SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;           // 设置时钟极性为低电平
    SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;       // 设置数据传输的首位为最高位
    SPI_InitStruct.SPI_Prescaler = SPI_Prescaler_4;        // 设置预分频为4
    SPI_Init(SPI2, &SPI_InitStruct);                       // 初始化
    SPI_PinRemapConfig(SPI2, SPI_PinRemap_C);              // 设置引脚映射
    SPI_ITConfig(SPI2, SPI_IT_INTEN, DISABLE);             // 禁用中断
    SPI_DMACmd(SPI2, SPI_DMAReq_TX, DISABLE);              // 关闭发送DMA请求
    SPI_DMACmd(SPI2, SPI_DMAReq_RX, DISABLE);              // 关闭发送DMA请求
    SPI_Cmd(SPI2, ENABLE);                                 // 使能
}

static void qspi_0_init(void) {
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_QTWI0, ENABLE);  // 使能时钟
    QSPI_InitTypeDef QSPI_InitStruct;                      // 初始化结构体
    QSPI_InitStruct.QSPI_SShift    = QSPI_SShift_OFF;      // 禁止延迟采样
    QSPI_InitStruct.QSPI_Prescaler = QSPI_Prescaler_4;     // 设置预分频
    QSPI_InitStruct.QSPI_DWidth    = QSPI_DWidth_8bit;     // 设置数据宽度
    QSPI_InitStruct.QSP_LMode      = QSPI_LMode_1Line;     // 设置单线模式
    QSPI_InitStruct.QSPI_Mode      = QSPI_Mode_QSPI;       // 设置半双工
    QSPI_InitStruct.QSPI_CPMode    = QSPI_CPMode_Low;      // 设置时钟极性为低
    QSPI_InitStruct.QSPI_RW        = QSPI_RW_Write;        // 设置为写模式
    QSPI_InitStruct.QSPI_CLKONLY   = QSPI_CLKONLY_OFF;     // 禁止只输出时钟
    QSPI_Init(QSPI0, &QSPI_InitStruct);                    // 初始化
    QSPI_ITConfig(QSPI0, QSPI_IT_INTEN, DISABLE);          // 禁用总中断
    QSPI_DMACmd(QSPI0, QSPI_DMAReq_TX, DISABLE);           // 关闭DMA发送请求
    QSPI_DMACmd(QSPI0, QSPI_DMAReq_RX, DISABLE);           // 关闭DMA接收请求
    QSPI_Cmd(QSPI0, ENABLE);                               // 使能
}

static void dma0_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);            // 使能时钟
    DMA_InitTypeDef DMA_InitStruct;                              // 初始化结构体
    DMA_InitStruct.DMA_Priority     = DMA_Priority_HIGH;         // 设置优先级为高
    DMA_InitStruct.DMA_CircularMode = DMA_CircularMode_Disable;  // 禁用循环模式
    DMA_InitStruct.DMA_DataSize     = DMA_DataSize_Byte;         // 设置数据大小为字节
    DMA_InitStruct.DMA_TargetMode   = DMA_TargetMode_FIXED;      // 设置目标地址固定
    DMA_InitStruct.DMA_SourceMode   = DMA_SourceMode_INC;        // 设置源地址循环递增
    DMA_InitStruct.DMA_Burst        = DMA_Burst_Disable;         // 禁用突发传输
    DMA_InitStruct.DMA_BufferSize   = 0;                         // 设置缓冲区大小为0
    DMA_InitStruct.DMA_Request      = DMA_Request_TWI_SPI2_TX;   // 设置请求源为SPI2发送
    DMA_InitStruct.DMA_SrcAddress   = 0;                         // 设置源地址为0
    DMA_InitStruct.DMA_DstAddress =
        (uint32_t)&(SPI2->SPI_DATA);             // 设置目标地址为SPI2数据寄存器
    DMA_Init(DMA0, &DMA_InitStruct);             // 初始化
    DMA_ITConfig(DMA0, DMA_IT_INTEN, ENABLE);    // 使能总中断
    DMA_ITConfig(DMA0, DMA_IT_TCIE, ENABLE);     // 使能传输完成中断
    DMA_ITConfig(DMA0, DMA_IT_HTIE, DISABLE);    // 禁用半传输中断
    DMA_ITConfig(DMA0, DMA_IT_TEIE, DISABLE);    // 禁用传输错误中断
    DMA_DMACmd(DMA0, DMA_DMAReq_CHRQ, DISABLE);  // 关闭的DMA请求
    __NVIC_SetPriority(DMA0_IRQn, 1);            // 设置中断优先级为1
    __NVIC_EnableIRQ(DMA0_IRQn);                 // 使能中断
    DMA_Cmd(DMA0, DISABLE);                      // 先关闭使能
}

static void dma_1_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);            // 使能时钟
    DMA_InitTypeDef DMA_InitStruct;                              // 初始化结构体
    DMA_InitStruct.DMA_Priority     = DMA_Priority_MEDIUM;       // 设置优先级为中
    DMA_InitStruct.DMA_CircularMode = DMA_CircularMode_Disable;  // 禁用循环模式
    DMA_InitStruct.DMA_DataSize     = DMA_DataSize_Byte;         // 设置数据大小为字节
    DMA_InitStruct.DMA_TargetMode   = DMA_TargetMode_FIXED;      // 设置目标地址固定
    DMA_InitStruct.DMA_SourceMode   = DMA_SourceMode_INC;        // 设置源地址循环递增
    DMA_InitStruct.DMA_Burst        = DMA_Burst_Disable;         // 禁用突发传输
    DMA_InitStruct.DMA_BufferSize   = 0;                         // 设置缓冲区大小为0
    DMA_InitStruct.DMA_Request      = DMA_Request_TWI_QSPI0_TX;  // 设置请求源为QSPI0发送
    DMA_InitStruct.DMA_SrcAddress   = 0;                         // 设置源地址为0
    DMA_InitStruct.DMA_DstAddress =
        (uint32_t)&(QSPI0->QSPI_DATA);           // 设置目标地址为QSPI数据寄存器
    DMA_Init(DMA1, &DMA_InitStruct);             // 初始化
    DMA_ITConfig(DMA1, DMA_IT_INTEN, ENABLE);    // 使能总中断
    DMA_ITConfig(DMA1, DMA_IT_TCIE, ENABLE);     // 使能传输完成中断
    DMA_ITConfig(DMA1, DMA_IT_HTIE, DISABLE);    // 禁用半传输中断
    DMA_ITConfig(DMA1, DMA_IT_TEIE, DISABLE);    // 禁用传输错误中断
    DMA_DMACmd(DMA1, DMA_DMAReq_CHRQ, DISABLE);  // 关闭DMA请求
    __NVIC_SetPriority(DMA1_IRQn, 1);            // 设置中断优先级为1
    __NVIC_EnableIRQ(DMA1_IRQn);                 // 使能中断
    DMA_Cmd(DMA1, DISABLE);                      // 先关闭使能
}

static void dma_2_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);            // 使能时钟
    DMA_InitTypeDef DMA_InitStruct;                              // 初始化结构体
    DMA_InitStruct.DMA_Priority     = DMA_Priority_MEDIUM;       // 设置优先级为中
    DMA_InitStruct.DMA_CircularMode = DMA_CircularMode_Disable;  // 禁用循环模式
    DMA_InitStruct.DMA_DataSize     = DMA_DataSize_Byte;         // 设置数据大小为字
    DMA_InitStruct.DMA_TargetMode   = DMA_TargetMode_INC;        // 设置目标地址递增
    DMA_InitStruct.DMA_SourceMode   = DMA_SourceMode_FIXED;      // 设置源地址固定
    DMA_InitStruct.DMA_Burst        = DMA_Burst_Disable;         // 禁用突发传输
    DMA_InitStruct.DMA_BufferSize   = 0;                         // 设置缓冲区大小为0
    DMA_InitStruct.DMA_Request      = DMA_Request_TWI_QSPI0_RX;  // 设置请求源为QSPI0接收
    DMA_InitStruct.DMA_SrcAddress =
        (uint32_t)&(QSPI0->QSPI_DATA);           // 设置源地址为QSPI数据寄存器
    DMA_InitStruct.DMA_DstAddress = 0;           // 设置目标地址为0
    DMA_Init(DMA2, &DMA_InitStruct);             // 初始化
    DMA_ITConfig(DMA2, DMA_IT_INTEN, ENABLE);    // 使能总中断
    DMA_ITConfig(DMA2, DMA_IT_TCIE, ENABLE);     // 使能传输完成中断
    DMA_ITConfig(DMA2, DMA_IT_HTIE, DISABLE);    // 禁用半传输中断
    DMA_ITConfig(DMA2, DMA_IT_TEIE, DISABLE);    // 禁用传输错误中断
    DMA_DMACmd(DMA2, DMA_DMAReq_CHRQ, DISABLE);  // 关闭DMA请求
    __NVIC_SetPriority(DMA2_IRQn, 1);            // 设置中断优先级为1
    __NVIC_EnableIRQ(DMA2_IRQn);                 // 使能中断
    DMA_Cmd(DMA2, DISABLE);                      // 先关闭使能
}

/**
 * @brief 在系统启动阶段进行的初始化操作。
 * @param
 * @retval
 * @warning
 * @note RT-Thread的系统调用。
 */
void rt_hw_board_init(void) {
    /* 片上外设初始化 */
    rcc_init();
    uart5_init();
    gpio_init();
    spi2_init();
    dma0_init();

#ifdef RT_USING_COMPONENTS_INIT
    /* 初始化系统组件 */
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    /* 初始化系统堆内存（动态堆内存：自动将空余的空间用作堆内存） */
    extern int Image$$RW_IRAM1$$ZI$$Limit;
    rt_system_heap_init((void *)&Image$$RW_IRAM1$$ZI$$Limit,
                        (void *)(0x20000000 + 32 * 1024));
#endif
}

#ifdef RT_USING_CONSOLE
/**
 * @brief 实现终端信息输出。
 * @param
 * @retval
 * @warning
 * @note RT-Thread的系统调用。
 */
void rt_hw_console_output(const char * str) {
    const uint32_t len = rt_strlen(str);
    for (uint32_t i = 0; i < len; ++i) {
        UART5->UART_DATA = (str[i] & (uint16_t)0x01FF);
        while (!(UART5->UART_STS & UART_Flag_TX)) {}
        UART5->UART_STS = UART_Flag_TX;
    }
}
#endif

#ifdef RT_USING_FINSH
/**
 * @brief 实现终端信息输入。
 * @param
 * @retval 输入的字符。
 * @warning
 * @note RT-Thread的系统调用。
 */
char rt_hw_console_getchar(void) {
    char ch = -1;
    return ch;
}
#endif

/********** 自动初始化 **********/
INIT_APP_EXPORT(st7789v_init);

/**
 * @brief RT-Thread的初始化配置代码的移植。
 * @details 初始化片上外设与板载外设。
 * @file board.c
 * @author proyrb
 * @date 2025/8/7
 * @note
 */

/********** 导入需要的头文件 **********/

#include <rthw.h>
#include <rtthread.h>
#include <sc32_conf.h>

/********** 实现初始化配置代码 **********/

/**
 * @brief 初始化系统级的时钟配置。
 * @param
 * @retval
 * @warning
 * @note 在rt_hw_board_init中调用一次。
 */
static void port_rcc_init(void) {
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
    RCC_SystickCmd(ENABLE);
}

/**
 * @brief 初始化用于输出日志信息的串口外设。
 * @param
 * @retval
 * @warning 在port_rcc_init之后调用。
 * @note 在rt_hw_board_init中调用一次。
 */
static void port_uart5_init(void) {
    /* 配置并开启APB0外设总线时钟 */
    RCC_APB0Config(RCC_HCLK_Div1);
    RCC_APB0Cmd(ENABLE);
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_UART5, ENABLE);

    /* 配置并开启uart5 */
    UART_InitTypeDef UART_InitStruct;                     // 初始化结构体
    UART_InitStruct.UART_ClockFrequency = 64000000;       // 设置时钟频率
    UART_InitStruct.UART_BaudRate       = 115200;         // 设置波特率为115200
    UART_InitStruct.UART_Mode           = UART_Mode_10B;  // 设置工作模式为10位数据位
    UART_Init(UART5, &UART_InitStruct);                   // 初始化
    UART_ITConfig(UART5, UART_IT_EN, DISABLE);            // 禁用中断
    UART_PinRemapConfig(UART5, UART_PinRemap_Default);    // 设置引脚复用为默认配置
    UART_TXCmd(UART5, ENABLE);                            // 使能发送功能
    UART_RXCmd(UART5, ENABLE);                            // 使能接收功能

    /* 设置uart5为printf的输出设备 */
    Printf_UartInit(UART5);
}

/**
 * @brief 实现SysTick中断处理中进行tick更新。
 * @param
 * @retval
 * @warning
 * @note
 */
void SysTick_Handler(void) {
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

/**
 * @brief 在系统启动阶段进行的初始化操作。
 * @param
 * @retval
 * @warning
 * @note RT-Thread的系统调用。
 */
void rt_hw_board_init(void) {
    /* 初始化系统级的时钟配置 */
    port_rcc_init();

#ifdef RT_USING_CONSOLE
    /* 初始化用于输出日志信息的串口外设 */
    port_uart5_init();
#endif

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
    printf("%s", str);
}
#endif

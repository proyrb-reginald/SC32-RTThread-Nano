#ifndef __MSH_CFG_H__
#define __MSH_CFG_H__

#include <rtconfig.h>

/**
 * @brief 启用命令行交互。
 * @warning
 * @note
 */
// #define RT_USING_FINSH

/**
 * @brief 启用类Linux的交互模式。
 * @warning
 * @note
 */
#define FINSH_USING_MSH

/**
 * @brief 只启用类Linux的交互模式。
 * @warning
 * @note
 */
#define FINSH_USING_MSH_ONLY

/**
 * @brief 配置FinSH的优先级。
 * @warning
 * @note
 */
#define FINSH_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX - 2)

/**
 * @brief 配置FinSH的栈大小。
 * @warning
 * @note
 */
#define FINSH_THREAD_STACK_SIZE 1024

/**
 * @brief 把符号表编译进固件。
 * @warning 一般不变。
 * @note
 */
#define FINSH_USING_SYMTAB

/**
 * @brief 把每个命令的“帮助说明”一起编译进固件。
 * @warning
 * @note
 */
#define FINSH_USING_DESCRIPTION

#endif

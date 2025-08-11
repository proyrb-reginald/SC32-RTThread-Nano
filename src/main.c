#include <rtthread.h>
#include <st7789v.h>

int main(void) {
    rt_thread_t tid = rt_thread_create("lcd", st7789v_test, RT_NULL, 8 * 64, 2, 10);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    }

    while (1) {
        rt_size_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        rt_kprintf("[%u] total: %u B\n used: %u B\n max: %u B\n free: %u B\n",
                   rt_tick_get(), total, used, max_used, total - used);
        rt_thread_delay(3000);
    }
}

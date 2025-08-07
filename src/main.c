#include <rtthread.h>

int main(void) {
    while (1) {
        rt_size_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        rt_kprintf(
            "[%u] total: %u B\n used: %u B\n max: %u B\n free: %u B\n",
            rt_tick_get(), total, used, max_used, total - used);
        rt_thread_delay(1000);
    }
}

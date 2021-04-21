#include <stddef.h>
#include <linux/module.h>

void printk_stub(const char *str, size_t len) {
    pr_info("%*.s", len, str);
}

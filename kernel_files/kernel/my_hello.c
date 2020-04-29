#include <linux/linkage.h>
#include <linux/kernel.h>


asmlinkage void sys_my_hello(void) {
    printk("Hello World!");
}


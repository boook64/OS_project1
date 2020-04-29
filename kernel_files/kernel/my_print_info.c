#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/timer.h>

asmlinkage void sys_printinfo(int pid, long st, long st_ns, long ed, long ed_ns) {
	printk("[Project1] %d %ld.%09ld %ld.%09ld\n", pid, st, st_ns, ed, ed_ns);
}

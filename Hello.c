// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>

int init_time;

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Igor Vurdelja");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
init_time = jiffies_to_msecs(jiffies);
pr_info("Hello module is loaded!\n");
return 0;
}

static void __exit hello_exit(void)
{
pr_info("Hello module is unloaded!\n");
pr_info("Time spent(millis): %d\n", jiffies_to_msecs(jiffies) - init_time);
}

module_init(hello_init);
module_exit(hello_exit);

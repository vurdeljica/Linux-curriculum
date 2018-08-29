// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Igor Vurdelja");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
pr_info("Hello module is loaded!");
return 0;
}

static void __exit hello_exit(void)
{
pr_info("Hello module is unloaded!");
}

module_init(hello_init);
module_exit(hello_exit);

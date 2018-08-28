#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Igor Vurdelja");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
    printk(KERN_INFO "Hello module is loaded!");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Hello module is unloaded!");
}

module_init(hello_init);
module_exit(hello_exit);

// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/device.h>         
#include <linux/fs.h>             
#include <linux/uaccess.h>    
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/semaphore.h>
#define  DEVICE_NAME "pantachar" 
#define  CLASS_NAME  "panta" 

static int init_time;
static int major_number;                 
static char * message;           
static int size_of_message = 20;                         
static struct class* hello_class  = NULL; 
static struct device* hello_device = NULL; 

static char foo_data[PAGE_SIZE];
static struct dentry *eudy;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static DEFINE_SEMAPHORE(foo_sem);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};


static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = dev_read,
	.write = dev_write
};


static ssize_t foo_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    ssize_t ret;
    down(&foo_sem);
    ret = simple_read_from_buffer(buffer, len, offset, foo_data, PAGE_SIZE);
    up(&foo_sem);
    return ret;
}

static ssize_t foo_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    ssize_t ret;
    down(&foo_sem);
    ret = simple_write_to_buffer(foo_data, PAGE_SIZE, offset, buffer, len);
    up(&foo_sem);
    return ret;
}

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};


static int __init hello_init(void)
{
    init_time = jiffies_to_msecs(jiffies);
    pr_info("PantaChar: Initializing the PantaChar LKM\n");
    
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "PantaChar failed to register a major number\n");
        return major_number;
    }
    pr_info("PantaChar: registered correctly with major number %d\n", major_number);

    hello_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(hello_class))
    {                
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(hello_class);          
    }
    pr_info("PantaChar: device class registered correctly\n");

    hello_device = device_create(hello_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(hello_device))
    {             
        class_destroy(hello_class);          
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(hello_device);
    }

    message = kmalloc(size_of_message, GFP_KERNEL);
    sprintf(message, "ZuehlkeCamp2017\n");

    pr_info("PantaChar: device class created correctly\n"); 
    
    eudy = debugfs_create_dir("zuehlke", NULL);
	if (!eudy)
		return -ENODEV;

	if (!debugfs_create_file("foo", 0644, eudy, NULL, &foo_fops))
		return -ENODEV;

	if (!debugfs_create_u32("jiffies", 0444, eudy, (u32*)&jiffies))
		return -ENODEV;
    
	if (!debugfs_create_file("id", 0666, eudy, NULL, &id_fops))
		return -ENODEV;

    pr_info("Hello module is loaded!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    debugfs_remove_recursive(eudy);
    kfree(message);
    device_destroy(hello_class, MKDEV(major_number, 0));     
    class_unregister(hello_class);                         
    class_destroy(hello_class);                             
    unregister_chrdev(major_number, DEVICE_NAME);      
    pr_info("Hello module is unloaded!\n");
    pr_info("Time spent(millis): %d\n", jiffies_to_msecs(jiffies) - init_time);
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("PantaChar: Device has been opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("PantaChar: Device successfully closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    return simple_read_from_buffer(buffer, len, offset, message, size_of_message);
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    
    if (strcmp(message, buffer) == 0)
    {
        pr_info("Message successfully sent!\n");
        return simple_write_to_buffer(message, size_of_message, offset, buffer, len);
    }

    return -EINVAL;

}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Igor Vurdelja");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");




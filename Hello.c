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
#define  DEVICE_NAME "pantachar" 
#define  CLASS_NAME  "panta" 

static int init_time;
static int    major_number;                 
static char *  message;           
static int  size_of_message = 20;                         
static struct class*  hello_class  = NULL; 
static struct device* hello_device = NULL; 
 

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Igor Vurdelja");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
.open = dev_open,
.read = dev_read,
.write = dev_write,
.release = dev_release,
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

pr_info("Hello module is loaded!\n");
return 0;
}

static void __exit hello_exit(void)
{
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





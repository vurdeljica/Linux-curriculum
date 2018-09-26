// SPDX-License-Identifier: GPL-2.0
/*
 * Sample kobject implementation
 *
 * Copyright (C) 2004-2007 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2007 Novell Inc.
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/string.h>

static DEFINE_SEMAPHORE(foo_sem);

static char foo_message[PAGE_SIZE];
static int foo_size;

static char id_message[20] = "ZuehlkeCamp2017\n";
static int id_size = 20;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	down(&foo_sem);
    strncpy(buf, foo_message, foo_size);
    up(&foo_sem);
    
    return foo_size;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
    if (count > PAGE_SIZE)
        return -EINVAL;
    
    down(&foo_sem);
    foo_size = count;
    strncpy(foo_message, buf, foo_size);
    up(&foo_sem);
    

	return count;
}

static struct kobj_attribute foo_attribute =
	__ATTR(foo, 0644, foo_show, foo_store);


static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
    return sprintf(buf, "%lu\n", jiffies);
}

static ssize_t jiffies_store(struct kobject *kobj, struct kobj_attribute *attr,
		      const char *buf, size_t count)
{
    return -EINVAL;
}

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
    strncpy(buf, id_message, id_size);
    
    return id_size;
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
    if (strcmp(id_message, buf) == 0)
    {
        pr_info("Message successfully sent!\n");
        return count;
    }

    return -EINVAL;
}


static struct kobj_attribute jiffies_attribute =
	__ATTR(jiffies, 0444, jiffies_show, jiffies_store);
    
static struct kobj_attribute id_attribute =
	__ATTR(id, 0664, id_show, id_store);


static struct attribute *attrs[] = {
	&foo_attribute.attr,
	&jiffies_attribute.attr,
	&id_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *example_kobj;

static int __init hello_init(void)
{
	int retval;

	example_kobj = kobject_create_and_add("zuehlke", kernel_kobj);
	if (!example_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

	return retval;
}

static void __exit hello_exit(void)
{
	kobject_put(example_kobj);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Igor Vurdelja");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");

#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "character"
#define CLASS_NAME "chara"
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");

int init_module(void)
{
	printk(KERN_INFO "Output module opened.");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Output module closed.");
}

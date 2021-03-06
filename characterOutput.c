#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "characterOutput"
#define CLASS_NAME "chara"
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");

static int majorDeviceNumber;
extern char ourInternalBuffer[BUFFER_SIZE];
extern int messageLength;
extern struct class* deviceClass;
static struct device* devicePointer = NULL;

static void shiftBuffer(int shift);

extern struct mutex chara_mutex;

static int this_open(struct inode * inode, struct file * file);
static int this_release(struct inode * inode, struct file * file);
static ssize_t this_read(struct file * file, char * buffer, size_t size, loff_t * offset);
static ssize_t this_write(struct file * file, const char * buffer, size_t size, loff_t * offset);

static struct file_operations fops =
{
	.open = this_open,
	.release = this_release,
	.read = this_read,
	.write = this_write,
};

/*int init_module(void)
{
	printk(KERN_INFO "Output module opened.");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Output module closed.");
}*/

int init_module(void)
{
	// Register major device number
	majorDeviceNumber = register_chrdev(0, DEVICE_NAME, &fops);

	if (majorDeviceNumber < 0)
	{
		printk(KERN_ALERT "Registering failed. majorDeviceNumber = %d\n", majorDeviceNumber);
		return majorDeviceNumber;
	}
	
	
	// Register device class
	//deviceClass = class_create(THIS_MODULE, CLASS_NAME);
	/*if (IS_ERR(deviceClass))
	{
		unregister_chrdev(majorDeviceNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register class\n");
		return PTR_ERR(deviceClass);
	}*/

	// Register driver
	devicePointer = device_create(deviceClass, NULL, MKDEV(majorDeviceNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(devicePointer))
	{
		//class_destroy(deviceClass);
		unregister_chrdev(majorDeviceNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create device\n");
	}		
	printk(KERN_INFO "Registering success.\n");
	return 0;
}

void cleanup_module(void)
{
	// Remove the device
	device_destroy(deviceClass, MKDEV(majorDeviceNumber, 0));
	// Unregister device class
	//class_unregister(deviceClass);
	// Remove device class
	//class_destroy(deviceClass);
	// Unregister major device number
	unregister_chrdev(majorDeviceNumber, DEVICE_NAME); 
	printk(KERN_INFO "Removing success\n");
}


static int this_open(struct inode * inode, struct file * file)
{
	if (!mutex_trylock(&chara_mutex))
	{
		printk(KERN_ALERT "The mutex is in use right now!");
		return -EBUSY;
	}
	printk(KERN_INFO "Device opened\n");
	return 0;
}

static int this_release(struct inode * inode, struct file * file)
{
	mutex_unlock(&chara_mutex);
	printk(KERN_INFO "Device closed");
	return 0;
}

// Specify number of characters to read from the buffer
static ssize_t this_read(struct file * file, char * buffer, size_t size, loff_t * offset)
{
	if (size > messageLength)
		size = messageLength;

	copy_to_user(buffer, ourInternalBuffer, size);
	
	messageLength -= size;
	shiftBuffer(size);

	//printk(KERN_INFO "Read: messageLength = %d", messageLength);

	return size;
}

// Write a specified number of character to the buffer
static ssize_t this_write(struct file * file, const char * buffer, size_t size, loff_t * offset)
{
	printk(KERN_ALERT "You can't write here");
	/*
	// check to make sure amount of space going to buffer isn't too large
	if (size >= (BUFFER_SIZE-messageLength))
		size = BUFFER_SIZE-messageLength;
	
	copy_from_user(ourInternalBuffer+messageLength,buffer,size);
	messageLength += size;
	
	//printk(KERN_INFO "Write: messageLength = %d", messageLength);
	
	return messageLength;*/

	return 0;
}

// Shift the remaining message or portion of the message to buffer[0]
static void shiftBuffer(int shift)
{
	int counter;
	for(counter = 0; counter < messageLength; counter++)
	{
		ourInternalBuffer[counter] = ourInternalBuffer[counter+shift];
	}
}

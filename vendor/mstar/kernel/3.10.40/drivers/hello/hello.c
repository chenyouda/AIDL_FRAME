#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#include "hello.h"

static int hello_major = 0;
static int hello_minor = 0;

static struct class* hello_class = NULL;
static struct hello_android_dev* hello_dev = NULL;

static int hello_proc_open(struct inode* inode, struct file* filp);
static int hello_proc_release(struct inode* inode, struct file* filp);
static ssize_t hello_proc_read(char* page, char** start, off_t off, int count, int* eof, void* data);
static ssize_t hello_proc_write(struct file* filp, const char __user *buf, size_t count, loff_t* f_pos);
static loff_t seq_proc_lseek(struct file *filp, loff_t off, int whence);

//static struct file_operations hello_fops = {
//	.owner = THIS_MODULE,
//	.open = hello_open,
//	.release = hello_release,
//	.read = hello_read,
//	.write = hello_write, 
//};

static struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = hello_proc_open,
    .write = hello_proc_write,
    .read = hello_proc_read,
    .llseek = seq_proc_lseek,
    .release = hello_proc_release,
};

static ssize_t hello_val_show(struct device* dev, struct device_attribute* attr,  char* buf);
static ssize_t hello_val_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

static DEVICE_ATTR(val, S_IRUGO | S_IWUSR, hello_val_show, hello_val_store);


//static int hello_open(struct inode* inode, struct file* filp) {
//	struct hello_android_dev* dev;        
//	
//	dev = container_of(inode->i_cdev, struct hello_android_dev, dev);
//	filp->private_data = dev;
//	
//	return 0;
//}
 int hello_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%d\n", hello_dev->val);

    return 0;
}

static int hello_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, hello_proc_show, NULL);
}

static loff_t seq_proc_lseek(struct file *filp, loff_t off, int whence)
{
	return 0;
}

static int hello_proc_release(struct inode* inode, struct file* filp) {
	return 0;
}

//static ssize_t hello_proc_read(struct file* filp, char __user *buf, size_t count, loff_t* f_pos) {
//	ssize_t err = 0;
//	struct hello_android_dev* dev = filp->private_data;        
//
//	if(down_interruptible(&(dev->sem))) {
//		return -ERESTARTSYS;
//	}
//
//	if(count < sizeof(dev->val)) {
//		goto out;
//	}        
//
//	if(copy_to_user(buf, &(dev->val), sizeof(dev->val))) {
//		err = -EFAULT;
//		goto out;
//	}
//
//	err = sizeof(dev->val);
//
//out:
//	up(&(dev->sem));
//	return err;
//}

//static ssize_t hello_write(struct file* filp, const char __user *buf, size_t count, loff_t* f_pos) {
//	struct hello_android_dev* dev = filp->private_data;
//	ssize_t err = 0;        
//
//	if(down_interruptible(&(dev->sem))) {
//		return -ERESTARTSYS;        
//	}        
//
//	if(count != sizeof(dev->val)) {
//		goto out;        
//	}        
//
//	if(copy_from_user(&(dev->val), buf, count)) {
//		err = -EFAULT;
//		goto out;
//	}
//
//	err = sizeof(dev->val);
//
//out:
//	up(&(dev->sem));
//	return err;
//}
static ssize_t __hello_set_val(struct hello_android_dev* dev, const char* buf, size_t count) {
	int val = 0;        

    
	val = simple_strtol(buf, NULL, 10);        


	if(down_interruptible(&(dev->sem))) {                
		return -ERESTARTSYS;        
	}        

	dev->val = val;        
	up(&(dev->sem));

	return count;
}

static ssize_t hello_proc_write(struct file *filp, const char __user *buff, size_t count, loff_t *pos)
{
    int err = 0;
    char *page = NULL;

    if(count > PAGE_SIZE)
    {
        printk("The buff is too large: %lu.\n", count);
        return -EFAULT;
    }

    page = (char *)__get_free_page(GFP_KERNEL);
    if(!page)
    {
        printk("Failed to alloc page.\n");
        return -ENOMEM;
    }

    if(copy_from_user(page, buff, count))
    {
        printk("Failed to copy buff from user.\n");
        err = -EFAULT;
        goto out;
    }

    err = __hello_set_val(hello_dev, page, count);

out:
    free_page((unsigned long)page);
    return err;
}

static ssize_t __hello_get_val(struct hello_android_dev* dev, char* buf) {
	int val = 0;        


	if(down_interruptible(&(dev->sem))) {                
		return -ERESTARTSYS;        
	}        

	val = dev->val;        
	up(&(dev->sem));        

	return snprintf(buf, PAGE_SIZE, "%d\n", val);
}




static ssize_t hello_val_show(struct device* dev, struct device_attribute* attr, char* buf) {
	struct hello_android_dev* hdev = (struct hello_android_dev*)dev_get_drvdata(dev);        

	return __hello_get_val(hdev, buf);
}

static ssize_t hello_val_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
	struct hello_android_dev* hdev = (struct hello_android_dev*)dev_get_drvdata(dev);  
	
	return __hello_set_val(hdev, buf, count);
}

static ssize_t hello_proc_read(char* page, char** start, off_t off, int count, int* eof, void* data) {
	if(off > 0) {
		*eof = 1;
		return 0;
	}

	return __hello_get_val(hello_dev, page);
}

//static ssize_t hello_proc_write(struct file* filp, const char __user *buff, unsigned long len, void* data) {
//	int err = 0;
//	char* page = NULL;
//
//	if(len > PAGE_SIZE) {
//		printk(KERN_ALERT"The buff is too large: %lu.\n", len);
//		return -EFAULT;
//	}
//
//	page = (char*)__get_free_page(GFP_KERNEL);
//	if(!page) {                
//		printk(KERN_ALERT"Failed to alloc page.\n");
//		return -ENOMEM;
//	}        
//
//	if(copy_from_user(page, buff, len)) {
//		printk(KERN_ALERT"Failed to copy buff from user.\n");                
//		err = -EFAULT;
//		goto out;
//	}
//
//	err = __hello_set_val(hello_dev, page, len);
//
//out:
//	free_page((unsigned long)page);
//	return err;
//}

static void hello_create_proc(void) {
//	struct proc_dir_entry* entry;
//	
////	entry = proc_create(HELLO_DEVICE_PROC_NAME, 0644, 0,  &hello_fops);
//  entry = proc_create(HELLO_DEVICE_PROC_NAME, 0, NULL, &hello_fops);
//	if(entry) {
//		entry->owner = THIS_MODULE;
//		entry->read_proc = hello_proc_read;
//		entry->write_proc = hello_proc_write;
//	}
    proc_create(HELLO_DEVICE_PROC_NAME, 0, NULL, &hello_fops);
}

static void hello_remove_proc(void) {
	remove_proc_entry(HELLO_DEVICE_PROC_NAME, NULL);
}

static int  __hello_setup_dev(struct hello_android_dev* dev) {
	int err;
	dev_t devno = MKDEV(hello_major, hello_minor);

	memset(dev, 0, sizeof(struct hello_android_dev));

	cdev_init(&(dev->dev), &hello_fops);
	dev->dev.owner = THIS_MODULE;
	dev->dev.ops = &hello_fops;        

	err = cdev_add(&(dev->dev),devno, 1);
	if(err) {
		return err;
	}        

	init_MUTEX(&(dev->sem));
	dev->val = 0;

	return 0;
}

static int __init hello_init(void)
{ 
	int err = -1;
	dev_t dev = 0;
	struct device* temp = NULL;

	printk("Initializing hello device.\n");        

	err = alloc_chrdev_region(&dev, 0, 1, HELLO_DEVICE_NODE_NAME);
	if(err < 0) {
		printk("Failed to alloc char dev region.\n");
		goto fail;
	}

	hello_major = MAJOR(dev);
	hello_minor = MINOR(dev);        

	hello_dev = kmalloc(sizeof(struct hello_android_dev), GFP_KERNEL);
	if(!hello_dev) {
		err = -ENOMEM;
		printk("Failed to alloc hello_dev.\n");
		goto unregister;
	}        

	err = __hello_setup_dev(hello_dev);
	if(err) {
		printk("Failed to setup dev: %d.\n", err);
		goto cleanup;
	}        

	hello_class = class_create(THIS_MODULE, HELLO_DEVICE_CLASS_NAME);
	if(IS_ERR(hello_class)) {
		err = PTR_ERR(hello_class);
		printk("Failed to create hello class.\n");
		goto destroy_cdev;
	}        

	temp = device_create(hello_class, NULL, dev, "%s", HELLO_DEVICE_FILE_NAME);
	if(IS_ERR(temp)) {
		err = PTR_ERR(temp);
		printk("Failed to create hello device.");
		goto destroy_class;
	}        

	err = device_create_file(temp, &dev_attr_val);
	if(err < 0) {
		printk("Failed to create attribute val.");                
		goto destroy_device;
	}

	dev_set_drvdata(temp, hello_dev);        

	hello_create_proc();

	printk("Succedded to initialize hello device.\n");
	return 0;

destroy_device:
	device_destroy(hello_class, dev);

destroy_class:
	class_destroy(hello_class);

destroy_cdev:
	cdev_del(&(hello_dev->dev));

cleanup:
	kfree(hello_dev);

unregister:
	unregister_chrdev_region(MKDEV(hello_major, hello_minor), 1);

fail:
	return err;
}

static void __exit hello_exit(void) {
	dev_t devno = MKDEV(hello_major, hello_minor);

	printk("Destroy hello device.\n");        

	hello_remove_proc();        

	if(hello_class) {
		device_destroy(hello_class, MKDEV(hello_major, hello_minor));
		class_destroy(hello_class);
	}        

	if(hello_dev) {
		cdev_del(&(hello_dev->dev));
		kfree(hello_dev);
	}        

	unregister_chrdev_region(devno, 1);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("First Android Driver");

module_init(hello_init);
module_exit(hello_exit);



#include<linux/string.h>
#include<linux/kernel.h>
#include<linux/fs.h> 
#include<linux/usb.h>
#include<linux/module.h>

#include<asm/unistd.h>
#include <linux/kmod.h>

//#include "header.h"

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512
 
static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];
 
static int pen_open(struct inode *i, struct file *f)
{
    printk(KERN_ALERT "Inside the %s function\n" , __FUNCTION__);

    return 0;
}
static int pen_close(struct inode *i, struct file *f)
{
    printk(KERN_ALERT "Inside the %s function\n" , __FUNCTION__);

    return 0;
}
static ssize_t pen_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
    int read_cnt;

    printk(KERN_ALERT "***************Inside the %s function***************\n" , __FUNCTION__);
    //printk(KERN_ALERT "***************buffer is %s*************** \n" ,buf);
    printk(KERN_ALERT "***************maximum readable length is %zu***************\n" ,cnt);
 
    /* Read the data from the bulk endpoint */
    
    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),
            bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);
    if (retval)
    {
        //printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }
    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
    {
        return -EFAULT;
    }
    
    return MIN(cnt, read_cnt);
}
static ssize_t pen_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
    int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);

    printk(KERN_ALERT "***************Inside the %s function***************\n" , __FUNCTION__);
    printk(KERN_ALERT "***************buffer is %s*************** \n" ,buf);
    printk(KERN_ALERT "***************length is %d***************\n" ,wrote_cnt);
 
    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
    {
        return -EFAULT;
    }
 
    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
            bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);
    if (retval)
    {
       // printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }
 
    return wrote_cnt;
}
 
static struct file_operations fops =
{
    .open = pen_open,
    .release = pen_close,
    .read = pen_read,
    .write = pen_write,
};
 
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;

    device = interface_to_usbdev(interface);
 
    class.name = "usb/os_pro_pen%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
        /* Something prevented us from registering this driver */
        //err("Not able to get a minor for this device.");
    }
    else
    {
        printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
        printk(KERN_INFO "pen_drive attached");

    }
 
    return retval;
}
 
static void pen_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &class);
     printk(KERN_INFO "disconnected");
}
 
/* Table of devices that work with this driver */
static struct usb_device_id pen_table[] =
{
    { USB_DEVICE(0x0781,0x5567) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, pen_table);
 
static struct usb_driver pen_driver =
{
    .name = "pen_driver",
    .probe = pen_probe,
    .disconnect = pen_disconnect,
    .id_table = pen_table,
};
 
static int __init pen_init(void)
{
    int result=0;

    printk(KERN_INFO "[*]sudo driver constructed");
    printk(KERN_INFO "[*]sudo driver registering with the kernel");
    printk(KERN_INFO "[*]COMPLETE!!");
 
    /* Register this driver with the USB subsystem */
    
    if ((result = usb_register(&pen_driver)))
    {
        //err("usb_register failed. Error number %d", result);
    }
    
    return result;
}
 
static void __exit pen_exit(void)
{
    /* Deregister this driver with the USB subsystem */
    usb_deregister(&pen_driver);
    printk(KERN_INFO "[*]sudo driver deregistering with the kernel");
}
 
module_init(pen_init);
module_exit(pen_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHUJA-15103302 PRAFULL-15103304 YASH-15103315");
MODULE_DESCRIPTION("SUDO Device Driver");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/mm.h> //za memorijsko mapiranje
#include <linux/io.h> //iowrite ioread
#include <linux/slab.h>//kmalloc kfree
#include <linux/platform_device.h>//platform driver
#include <linux/of.h>//of match table
#include <linux/ioport.h>//ioremap


#define BUFF_SIZE 30
#define DRIVER_NAME "ASCRS_driver"
#define DEVICE_NAME "ASCRS"

MODULE_AUTHOR("FTN");
MODULE_DESCRIPTION("Test driver for audio scrambler system.");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("custom:ASCRS");
//********************************************GLOBAL VARIABLES *****************************************//
struct ASCRS_info {
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem * base_addr;

};

dev_t my_dev_id;
static struct class * my_class;
static struct device * my_device;
static struct cdev * my_cdev;
static struct ASCRS_info * ip = NULL; //scrambler
static struct ASCRS_info * bp1 = NULL; //bram0
static struct ASCRS_info * bp2 = NULL; //bram1

int position = 0;
int number = 0;
int counter = 0;
int endRead = 0;
int k = 0;

//****************************** FUNCTION PROTOTYPES ****************************************//
static int ASCRS_probe(struct platform_device * pdev);
static int ASCRS_remove(struct platform_device * pdev);
static int ASCRS_open(struct inode * pinode, struct file * pfile);
static int ASCRS_close(struct inode * pinode, struct file * pfile);
static ssize_t ASCRS_read(struct file * pfile, char __user * buf, size_t length, loff_t * offset);
static ssize_t ASCRS_write(struct file * pfile,
    const char __user * buf, size_t length, loff_t * offset);
int ASCRS_mmap(struct file * f, struct vm_area_struct * vma_s);

static int __init ASCRS_init(void);
static void __exit ASCRS_exit(void);

struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = ASCRS_read,
    .write = ASCRS_write,
    .open = ASCRS_open,
    .release = ASCRS_close,
    .mmap = ASCRS_mmap,

};

static struct of_device_id ASCRS_of_match[] = {

    {
        .compatible = "bram0",
    },
    {
        .compatible = "bram1",
    },
    {
        .compatible = "scrambler"
    },
    {
        /* end of list */ }

};

MODULE_DEVICE_TABLE( of , ASCRS_of_match);

static struct platform_driver ASCRS_driver = {

    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = ASCRS_of_match,
    },

    .probe = ASCRS_probe,
    .remove = ASCRS_remove,
};

static int ASCRS_probe(struct platform_device * pdev) {

    struct resource * r_mem;
    int rc = 0;
    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
        printk(KERN_ALERT "Failed to get resource\n");
        return -ENODEV;
    }

    switch (counter) {

    case 0: //bram0

        bp1 = (struct ASCRS_info * ) kmalloc(sizeof(struct ASCRS_info), GFP_KERNEL);
        if (!bp1) {
            printk(KERN_ALERT "Could not allocate memory\n");
            return -ENOMEM;
        }

        bp1 -> mem_start = r_mem -> start;
        bp1 -> mem_end = r_mem -> end;
        printk(KERN_INFO "start address:%x \t end address:%x", r_mem -> start, r_mem -> end);

        if (!request_mem_region(bp1 -> mem_start, bp1 -> mem_end - bp1 -> mem_start + 1, DRIVER_NAME)) {
            printk(KERN_ALERT "Could not lock memory region at %p\n", (void * ) bp1 -> mem_start);
            rc = -EBUSY;
            goto error3;
        }

        bp1 -> base_addr = ioremap(bp1 -> mem_start, bp1 -> mem_end - bp1 -> mem_start + 1);

        if (!bp1 -> base_addr) {
            printk(KERN_ALERT "Could not allocate memory\n");
            rc = -EIO;
            goto error4;
        }

        counter++;
        printk(KERN_WARNING "bram0 registered\n");
        return 0; //ALL OK

        error4:
            release_mem_region(bp1 -> mem_start, bp1 -> mem_end - bp1 -> mem_start + 1);
        error3:
            return rc;

    case 1: //bram1

        bp2 = (struct ASCRS_info * ) kmalloc(sizeof(struct ASCRS_info), GFP_KERNEL);
        if (!bp2) {
            printk(KERN_ALERT "Could not allocate memory\n");
            return -ENOMEM;
        }

        bp2 -> mem_start = r_mem -> start;
        bp2 -> mem_end = r_mem -> end;
        printk(KERN_INFO "startaddress:%x \t end address:%x", r_mem -> start, r_mem -> end);

        if (!request_mem_region(bp2 -> mem_start, bp2 -> mem_end - bp2 -> mem_start + 1, DRIVER_NAME)) {
            printk(KERN_ALERT "Could not lock memory region at %p\n", (void * ) bp2 -> mem_start);
            rc = -EBUSY;
            goto error6;
        }

        bp2 -> base_addr = ioremap(bp2 -> mem_start, bp2 -> mem_end - bp2 -> mem_start + 1);

        if (!bp2 -> base_addr) {
            printk(KERN_ALERT "Could not allocate memory\n");
            rc = -EIO;
            goto error5;
        }

        counter++;
        printk(KERN_WARNING "bram1 registered\n");
        return 0; //ALL OK

        error5:
            release_mem_region(bp2 -> mem_start, bp2 -> mem_end - bp2 -> mem_start + 1);
        error6:
            return rc;

    case 2: //scrambler

        ip = (struct ASCRS_info * ) kmalloc(sizeof(struct ASCRS_info), GFP_KERNEL);
        if (!ip) {
            printk(KERN_ALERT "Could not allocate memory\n");
            return -ENOMEM;
        }

        ip -> mem_start = r_mem -> start;
        ip -> mem_end = r_mem -> end;
        printk(KERN_INFO "start address:%x \t end address:%x", r_mem -> start, r_mem -> end);

        if (!request_mem_region(ip -> mem_start, ip -> mem_end - ip -> mem_start + 1, DRIVER_NAME)) {
            printk(KERN_ALERT "Could not lock memory region at %p\n", (void * ) ip -> mem_start);
            rc = -EBUSY;
            goto error1;
        }

        ip -> base_addr = ioremap(ip -> mem_start, ip -> mem_end - ip -> mem_start + 1);

        if (!ip -> base_addr) {
            printk(KERN_ALERT "Could not allocate memory\n");
            rc = -EIO;
            goto error2;
        }

        printk(KERN_INFO "ASCRS_driver registered\n");
        return 0; //ALL OK

        error2:
            release_mem_region(ip -> mem_start, ip -> mem_end - ip -> mem_start + 1);
        error1:
            return rc;

    }

    return 0;
}

static int ASCRS_remove(struct platform_device * pdev) {

    switch (counter) {

    case 0: //bram0
        printk(KERN_WARNING "bram0_remove: platform driver removing\n");
        iowrite32(0, bp1 -> base_addr);
        iounmap(bp1 -> base_addr);
        release_mem_region(bp1 -> mem_start, bp1 -> mem_end - bp1 -> mem_start + 1);
        kfree(bp1);
        printk(KERN_INFO "bram0_remove: bram0 removed\n");

        break;

    case 1: //bram1
        printk(KERN_WARNING "bram1_remove: platform driver removing\n");
        iowrite32(0, bp2 -> base_addr);
        iounmap(bp2 -> base_addr);
        release_mem_region(bp2 -> mem_start, bp2 -> mem_end - bp2 -> mem_start + 1);
        kfree(bp2);
        printk(KERN_INFO "bram1_remove: bram1 removed \n");
        counter--;
        break;

    case 2: //scrambler
        printk(KERN_WARNING "ASCRS_remove: platform driver removing\n");
        iowrite32(0, ip -> base_addr);
        iounmap(ip -> base_addr);
        release_mem_region(ip -> mem_start, ip -> mem_end - ip -> mem_start + 1);
        kfree(ip);
        printk(KERN_INFO "ASCRS_remove: ASCRS driver removed\n");
        counter--;
        break;

    }

    return 0;
}

int ASCRS_open(struct inode * pinode, struct file * pfile) {

    printk(KERN_INFO "Succesfully opened file\n");
    return 0;

}

int ASCRS_close(struct inode * pinode, struct file * pfile) {

    printk(KERN_INFO "Succesfully closed file\n");
    return 0;

}

ssize_t ASCRS_read(struct file * pfile, char __user * buf, size_t length, loff_t * offset) {

    int ret, pos = 0;
    char buff[BUFF_SIZE];
    int len, value;
    int minor = MINOR(pfile -> f_inode -> i_rdev);
    if (endRead == 1) {
        endRead = 0;
        return 0;
    }
    switch (minor) {

    case 0: //bram0

        pos = position + k * 4;
        value = ioread32(bp1 -> base_addr + pos);
        len = scnprintf(buff, BUFF_SIZE, "%d\n", value);
        * offset += len;
        ret = copy_to_user(buf, buff, len);
        if (ret) {
            return -EFAULT;
        }
        k++;
        if (k == 8192) {
            endRead = 1;
            k = 0;
        }
        break;

    case 1: //bram1
        pos = position + k * 4;
        value = ioread32(bp2 -> base_addr + pos);
        len = scnprintf(buff, BUFF_SIZE, "%d\n", value);
        * offset += len;
        ret = copy_to_user(buf, buff, len);
        if (ret) {
            return -EFAULT;
        }
        k++;
        if (k == 8192) {
            endRead = 1;
            k = 0;
        }
        break;

    case 2: //scrambler

        value = ioread32(ip -> base_addr + k * 4);
        len = scnprintf(buff, BUFF_SIZE, "%d\n", value);
        * offset += len;
        ret = copy_to_user(buf, buff, len);
        if (ret) {
            return -EFAULT;
        }
        k++;
        if (k == 3) {
            endRead = 1;
            k = 0;
        }

        break;

    default:
        printk(KERN_INFO "somethnig went wrong\n");
    }

    return len;
}

ssize_t ASCRS_write(struct file * pfile,
    const char __user * buf, size_t length, loff_t * offset) {

    char buff[BUFF_SIZE];
    int minor = MINOR(pfile -> f_inode -> i_rdev);
    int ret = 0, i = 0, pos = 0;
    unsigned int address = 0;
    unsigned int data = 0;
    int reset, start, ready;
    ret = copy_from_user(buff, buf, length);

    if (ret) {
        printk("copy from user failed \n");
        return -EFAULT;
    }
    buff[length] = '\0';

    switch (minor) {

    case 0: //bram0

        sscanf(buff, "(%d);%d", &address, &data);

        if (xpos > 8191) {
            printk(KERN_WARNING "bram0: position exceeded maximum value \n");
        } else {
            position = address * 4;
            pos = position + i * 4;
            iowrite32(data, bp1 -> base_addr + address);

        }

        break;

    case 1: //bram1

        sscanf(buff, "(%d);%d", & address, & data);

        if (xpos > 8191) {
            printk(KERN_WARNING "bram1: position exceeded maximum value \n");
        } else {

            position = address * 4;
            pos = position + i * 4;
            iowrite32(data, bp2 -> base_addr + pos);

        }

        break;

    case 2: //scrambler

        sscanf(buff, "%d,%d,%d", & reset, & start, & ready);
        if (ret != -EINVAL) {
            if (reset != 0 && reset != 1) {

                printk(KERN_WARNING "scrambler: reset must be 1 or 0 \n");

            } else if (start != 0 && start != 1) {

                printk(KERN_WARNING "scrambler: start must be 1 or 0\n");

            } else if (ready != 0 && ready != 1) {

                printk(KERN_WARNING "scrambler: ready be 1 or 0 \n");

            } else {

                iowrite32(reset, ip -> base_addr); //reset
                iowrite32(start, ip -> base_addr + 4); //start
                iowrite32(ready, ip -> base_addr + 8); //cmd

            }

        }
        break;

    default:
        printk(KERN_INFO "somethnig went wrong\n");
    }

    return length;
}

int ASCRS_mmap(struct file * f, struct vm_area_struct * vma_s) {

    int ret = 0;
    int minor = MINOR(f -> f_inode -> i_rdev);
    unsigned long vsize;
    unsigned long psize;
    switch (minor) {

    case 0:

        vsize = vma_s -> vm_end - vma_s -> vm_start; // velicina addr prostora koji zahteva aplikacija
        psize = bp1 -> mem_end - bp1 -> mem_start + 1; // velicina addr prostora koji zauzima jezgro
        vma_s -> vm_page_prot = pgprot_noncached(vma_s -> vm_page_prot);
        printk(KERN_INFO "bram0: Buffer is being memory mapped\n");

        if (vsize > psize) {
            printk(KERN_ERR "bram0: Trying to mmap more space than it's allocated, mmap failed\n");
            return -EIO;
        }
        //printk(KERN_INFO "psize is %lu\n", psize);
        ret = vm_iomap_memory(vma_s, bp1 -> mem_start, vsize);
        if (ret) {
            printk(KERN_ERR "bram0: memory maped failed\n");
            return ret;

        }
        printk(KERN_INFO "MMAP is a success for bram0\n");

        break;

    case 1:

        vsize = vma_s -> vm_end - vma_s -> vm_start; // velicina addr prostora koji zahteva aplikacija
        psize = bp2 -> mem_end - bp2 -> mem_start + 1; // velicina addr prostora koji zauzima jezgro
        vma_s -> vm_page_prot = pgprot_noncached(vma_s -> vm_page_prot);
        printk(KERN_INFO "bram1: Buffer is being memory mapped\n");

        if (vsize > psize) {
            printk(KERN_ERR "bram1: Trying to mmap more space than it's allocated, mmap failed\n");
            return -EIO;
        }
        ret = vm_iomap_memory(vma_s, bp2 -> mem_start, vsize);
        if (ret) {
            printk(KERN_ERR "bram1: memory maped failed\n");
            return ret;

        }
        printk(KERN_INFO "MMAP is a success for bram1\n");

        break;

    case 2:

        vsize = vma_s -> vm_end - vma_s -> vm_start; // velicina addr prostora koji zahteva aplikacija
        psize = ip -> mem_end - ip -> mem_start + 1; // velicina addr prostora koji zauzima jezgro
        vma_s -> vm_page_prot = pgprot_noncached(vma_s -> vm_page_prot);
        printk(KERN_INFO "ASCRS: Buffer is being memory mapped\n");

        if (vsize > psize) {
            printk(KERN_ERR "ASCRS: Trying to mmap more space than it's allocated, mmap failed\n");
            return -EIO;
        }
        ret = vm_iomap_memory(vma_s, ip -> mem_start, vsize);
        if (ret) {
            printk(KERN_ERR "ASCRS: memory maped failed\n");
            return ret;

        }
        printk(KERN_INFO "MMAP is a success for ASCRS\n");

        break;

    default:
        printk(KERN_INFO "somethnig went wrong\n");

    }

    return 0;
}

static int __init ASCRS_init(void) {
    int num_of_minors = 3;
    int ret = 0;
    ret = alloc_chrdev_region( & my_dev_id, 0, num_of_minors, "ASCRS_region");
    if (ret != 0) {

        printk(KERN_ERR "Failed to register char device\n");
        return ret;
    }
    printk(KERN_INFO "Char device region allocated\n");

    my_class = class_create(THIS_MODULE, "ASCRS_class");
    if (my_class == NULL) {
        printk(KERN_ERR "Failed to create class\n");
        goto fail_0;
    }
    printk(KERN_INFO "Class created\n");

    my_device = device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 0), NULL, "bram0");
    if (my_device == NULL) {
        printk(KERN_ERR "failed to create device bram0\n");
        goto fail_1;
    }
    printk(KERN_INFO "created bram0\n");
    my_device = device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 1), NULL, "bram1");
    if (my_device == NULL) {
        printk(KERN_ERR "failed to create device bram1\n");
        goto fail_1;
    }
    printk(KERN_INFO "created bram1\n");

    my_device = device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 2), NULL, "scrambler");

    if (my_device == NULL) {
        printk(KERN_ERR "Failde to create device ASCRS\n");
        goto fail_1;
    }
    printk(KERN_INFO "created ASCRS\n");

    my_cdev = cdev_alloc();
    my_cdev -> ops = & my_fops;
    my_cdev -> owner = THIS_MODULE;
    ret = cdev_add(my_cdev, my_dev_id, num_of_minors);
    if (ret) {
        printk(KERN_ERR "Failed to add cdev \n");
        goto fail_2;
    }
    printk(KERN_INFO "cdev_added\n");
    printk(KERN_INFO "Hello from ASCRS_driver\n");

    return platform_driver_register( & ASCRS_driver);

    fail_2:
        device_destroy(my_class, my_dev_id);
    fail_1:
        class_destroy(my_class);
    fail_0:
        unregister_chrdev_region(my_dev_id, 1);
    return -1;

}

static void __exit ASCRS_exit(void) {
    printk(KERN_ALERT "ASCRS_exit: rmmod called\n");
    platform_driver_unregister( & ASCRS_driver);
    printk(KERN_INFO "ASCRS_exit: platform_driver_unregister done\n");
    cdev_del(my_cdev);
    printk(KERN_ALERT "ASCRS_exit: cdev_del done\n");
    device_destroy(my_class, MKDEV(MAJOR(my_dev_id), 0));
    printk(KERN_INFO "ASCRS_exit: device destroy 0\n");
    device_destroy(my_class, MKDEV(MAJOR(my_dev_id), 1));
    printk(KERN_INFO "ASCRS_exit: device destroy 1\n");
    device_destroy(my_class, MKDEV(MAJOR(my_dev_id), 2));
    printk(KERN_INFO "ASCRS_exit: device destroy 2\n");
    class_destroy(my_class);
    printk(KERN_INFO "ASCRS_exit: class destroy \n");
    unregister_chrdev_region(my_dev_id, 4);
    printk(KERN_ALERT "Goodbye from ASCRS_driver\n");

}

module_init(ASCRS_init);
module_exit(ASCRS_exit);

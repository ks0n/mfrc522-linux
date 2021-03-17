// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <kunit/test.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");

static ssize_t mfrc522_write(struct file *file, const char *buffer, size_t len, loff_t *offset) {
    pr_info("[MFRC522] Being written to\n");

    return len;
}

static ssize_t mfrc522_read(struct file *file, char *buffer, size_t len, loff_t *offset) {
    pr_info("[MFRC522] Being read\n");

    return len;
}

static const struct file_operations mfrc522_fops = {
    .owner = THIS_MODULE,
    .write = mfrc522_write,
    .read = mfrc522_read,
};

static struct miscdevice mfrc522_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mfrc522_misc",
    .fops = &mfrc522_fops,
};

static int __init mfrc522_init(void)
{
	pr_info("MFRC522 init\r\n");

    int ret = misc_register(&mfrc522_misc);
    if (ret) {
        pr_err("[MFRC522] Misc device initialization failed\n");
        return ret;
    }

	return 0;
}

static void __exit mfrc522_exit(void)
{
    misc_deregister(&mfrc522_misc);
	pr_info("MFRC522 exit\r\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

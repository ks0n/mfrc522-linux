// SPDX-License-Identifier: GPL-2.0

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/spi/spi.h>

#include "mfrc522-rs/mfrc522.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Rust Driver for the MFRC522 RFID Chip");

ssize_t mfrc522_write(struct file *file, const char *buffer, size_t len,
			     loff_t *offset) {
    return mfrc522_write_rs(buffer, len);
}

ssize_t mfrc522_read(struct file *file, char *buffer, size_t len,
			    loff_t *offset)
{
    return mfrc522_read_rs();
}

static const struct file_operations mfrc522_fops = {
       .owner = THIS_MODULE,
       .write = mfrc522_write,
       .read = mfrc522_read,
};

static struct miscdevice mfrc522_misc = {
       .minor = MISC_DYNAMIC_MINOR,
       .name = "mfrc522_rust_misc",
       .fops = &mfrc522_fops,
};

static int __init mfrc522_init(void)
{
	int ret;

	pr_info("[MFRC522-RS] Init\n");

	ret = misc_register(&mfrc522_misc);
	if (ret) {
		pr_err("[MFRC522-RS] Misc device initialization failed\n");
		return ret;
	}

	return 0;
}

static void __exit mfrc522_exit(void)
{
	misc_deregister(&mfrc522_misc);
	pr_info("[MFRC522-RS] Exit\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

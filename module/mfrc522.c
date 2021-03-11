// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");

static int __init mfrc522_init(void)
{
	pr_info("MFRC522 init\r\n");

	return 0;
}

static void __exit mfrc522_exit(void)
{
	pr_info("MFRC522 exit\r\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

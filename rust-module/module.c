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

static int __init mfrc522_init(void)
{
	int ret;

	pr_info("MFRC522-Rust init. Hello Rust? %d\n", hello_rust());

	return 0;
}

static void __exit mfrc522_exit(void)
{
	pr_info("MFRC522-Rust exit\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

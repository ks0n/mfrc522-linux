#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");

extern int mfrc522_init_rs(void);
extern void mfrc522_exit_rs(void);

static int __init mfrc522_init(void) {
    return mfrc522_init_rs();
}

static void __exit mfrc522_exit(void) {
    mfrc522_exit_rs();
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

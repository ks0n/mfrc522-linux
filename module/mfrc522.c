// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");
MODULE_SOFTDEP("pre: regmap");

static int mfrc522_spi_probe(struct spi_device *spi);

static struct spi_driver mfrc522_spi = {
    .driver = {
        .name = "mfrc522",
        .owner = THIS_MODULE,
    },
    .probe = mfrc522_spi_probe,
};

// static int mfrc522_reg_read(void *, unsigned int reg, unsigned int *val);

static struct regmap_config mfrc522_regmap_config = {
    .reg_bits = 8,
    .val_bits = 8,
    // .reg_read = (void *context, unsigned int reg, unsigned int *val);
    // .reg_write)(void *context, unsigned int reg, unsigned int val);
};

static struct regmap *mfrc522_regmap;

static int mfrc522_spi_probe(struct spi_device *client)
{
    pr_info("[MFRC522] SPI Probed\n");

    // mfrc522_regmap = regmap_init_spi(client, &mfrc522_regmap_config);
    // if (IS_ERR(mfrc522_regmap))
    //     pr_err("[MFRC522] Regmap init SPI failed\n");

    /*
    int err;
    struct regmap *my_regmap;
    struct regmap_config bmp085_regmap_config;
    // fill bmp085_regmap_config somewhere

    client->bits_per_word = 8;
    my_regmap = regmap_init_spi(client,&bmp085_regmap_config);
    if (IS_ERR(my_regmap)) {
        err = PTR_ERR(my_regmap);
        dev_err(&client->dev, "Failed to init regmap: %d\n", err);
        return err;
    }
    */

    // spi_write()

    return 0;
}

static int __init mfrc522_init(void)
{
	pr_info("MFRC522 init\r\n");

    if (spi_register_driver(&mfrc522_spi))
	    pr_info("[MFRC522] SPI Register failed\r\n");

	return 0;
}

static void __exit mfrc522_exit(void)
{
	pr_info("MFRC522 exit\r\n");
    spi_unregister_driver(&mfrc522_spi);
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

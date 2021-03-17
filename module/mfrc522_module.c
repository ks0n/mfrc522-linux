// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/delay.h>

#include "mfrc522_spi.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");

#define MFRC522_VERSION_BASE 0x90
#define MFRC522_VERSION_1 0x91
#define MFRC522_VERSION_2 0x92
#define MFRC522_VERSION_NUM(ver) ((ver)-MFRC522_VERSION_BASE)

static int mfrc522_spi_probe(struct spi_device *spi);

static struct spi_driver mfrc522_spi = {
    .driver = {
        .name = "mfrc522",
        .owner = THIS_MODULE,
    },
    .probe = mfrc522_spi_probe,
};

/** Detect if the device we are talking to is an MFRC522 using the VersionReg,
 * section 9.3.4.8 of the datasheet
 *
 * @client SPI device
 *
 * @return -1 if not an MFRC522, version number otherwise
 */
static int mfrc522_detect(struct spi_device *client)
{
	struct address_byte version_reg_read =
		address_byte_build(MFRC522_SPI_READ, MFRC522_VERSION_REG);
	char version;

	spi_write(client, &version_reg_read, 1);
	spi_read(client, &version, 1);

	switch (version) {
	case MFRC522_VERSION_1:
	case MFRC522_VERSION_2:
		version = MFRC522_VERSION_NUM(version);
		pr_info("[MFRC522] MFRC522 version %d detected\n", version);
		return version;
	default:
		pr_info("[MFRC522] this chip is not an MFRC522: 0x%x\n",
			version);
	}

	return -1;
}

static int mfrc522_spi_probe(struct spi_device *client)
{
	pr_info("[MFRC522] SPI Probed\n");

	if (client->max_speed_hz > MFRC522_SPI_MAX_CLOCK_SPEED) {
		pr_info("[MFRC522] Current speed is to high %u. Setting speed to %u\n",
			client->max_speed_hz, MFRC522_SPI_MAX_CLOCK_SPEED);
		client->max_speed_hz = MFRC522_SPI_MAX_CLOCK_SPEED;
	}

	return mfrc522_detect(client) < 0;
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

// SPDX-License-Identifier: GPL-2.0

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/fs.h>

#include "mfrc522_user_command.h"
#include "mfrc522_parser.h"
#include "mfrc522_spi.h"

#define MFRC522_VERSION_BASE 0x90
#define MFRC522_VERSION_1 0x91
#define MFRC522_VERSION_2 0x92
#define MFRC522_VERSION_NUM(ver) ((ver)-MFRC522_VERSION_BASE)

#define MFRC522_MAX_ANSWER_SIZE 256 // FIXME

static struct mfrc522_private_data {
	bool buffer_full;
	char answer[MFRC522_MAX_ANSWER_SIZE];
} mfrc522_struct;

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");

static ssize_t mfrc522_write(struct file *file, const char *buffer, size_t len,
			     loff_t *offset)
{
	int ret;
	int answer_size;
	char *answer;
	struct mfrc522_command command = { 0 };
	struct mfrc522_private_data *mfrc522_data;

	file->private_data = &mfrc522_struct;

	mfrc522_data = file->private_data;
	answer = mfrc522_data->answer;

	pr_info("[MFRC522] Being written to: %.*s\n", len, buffer);

	ret = mfrc522_parse(&command, buffer, len);

	if (ret) {
		pr_err("[MFRC522] Got invalid command\n");
		return -EBADMSG;
	}

	pr_info("[MFRC522] Got following command: %d\n", command.cmd);

	// The data buffer is zero filled if no extra input has been given
	if (command.data[0])
		pr_info("[MFRC522] With extra data: `%s`\n", command.data);

	answer_size = mfrc522_execute(answer, &command);

	if (answer_size < 0) {
		// Error
		pr_err("[MFRC522] Error when executing command\n");
	} else {
		// Non-empty answer
		pr_info("[MFRC522] Answer: \"%.*s\"\n", answer_size, answer);
		mfrc522_data->buffer_full = true;
	}

	return len;
}

static ssize_t mfrc522_read(struct file *file, char *buffer, size_t len,
			    loff_t *offset)
{
	size_t i;
	char *answer;
	struct mfrc522_private_data *mfrc522_data;

	pr_info("[MFRC522] Being read from\n");

	file->private_data = &mfrc522_struct;

	mfrc522_data = file->private_data;
	answer = mfrc522_data->answer;

	if (!mfrc522_data->buffer_full)
		return 0;

	len = (len > MFRC522_MEM_SIZE) ? MFRC522_MEM_SIZE : len;
	for (i = 0; i < len; i++)
		buffer[i] = answer[i];

	mfrc522_data->buffer_full = false;

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

static int mfrc522_spi_probe(struct spi_device *spi);

static struct spi_driver mfrc522_spi_driver = {
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
	u8 version = mfrc522_get_version();

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

	// FIXME: Don't register one global clientstruct spi_device *mfrc522_spi;
	mfrc522_spi = client;

	return mfrc522_detect(client) < 0;
}

static int __init mfrc522_init(void)
{
	int ret;

	pr_info("MFRC522 init\n");

	ret = misc_register(&mfrc522_misc);
	if (ret) {
		pr_err("[MFRC522] Misc device initialization failed\n");
		return ret;
	}

	ret = spi_register_driver(&mfrc522_spi_driver);
	if (ret) {
		pr_err("[MFRC522] SPI Register failed\r\n");
		return ret;
	}

	return 0;
}

static void __exit mfrc522_exit(void)
{
	misc_deregister(&mfrc522_misc);
	spi_unregister_driver(&mfrc522_spi_driver);

	pr_info("MFRC522 exit\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

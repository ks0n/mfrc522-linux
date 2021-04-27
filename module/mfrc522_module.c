// SPDX-License-Identifier: GPL-2.0

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/fs.h>

#include "mfrc522_module.h"
#include "mfrc522_user_command.h"
#include "mfrc522_parser.h"
#include "mfrc522_spi.h"

#define MFRC522_VERSION_BASE 0x90
#define MFRC522_VERSION_1 0x91
#define MFRC522_VERSION_2 0x92
#define MFRC522_VERSION_NUM(ver) ((ver)-MFRC522_VERSION_BASE)

static struct mfrc522_state *g_state;

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ks0n");
MODULE_DESCRIPTION("Driver for the MFRC522 RFID Chip");

static void do_debug_read(const char *answer, int answer_size)
{
	int i;
	char line[16];
	int write_head = 0;

	pr_info("RD\n");

	for (i = 1; i < answer_size + 1; i++) {
		write_head +=
			snprintf(line + write_head, 16, "%02x", answer[i - 1]);

		if (i % 5 == 0) {
			pr_info("%s\n", line);
			write_head = 0;
		} else {
			write_head += snprintf(line + write_head, 16, " ");
		}
	}

	if (write_head > 0)
		pr_info("%s\n", line);
}

static void do_debug_write(const char *cmd)
{
	int i;
	char line[16];
	int write_head = 0;

	pr_info("WR\n");

	for (i = 1; cmd[i - 1]; i++) {
		write_head +=
			snprintf(line + write_head, 16, "%02x", cmd[i - 1]);

		if (i % 5 == 0) {
			pr_info("%s\n", line);
			write_head = 0;
		} else {
			write_head += snprintf(line + write_head, 16, " ");
		}
	}

	if (write_head > 0)
		pr_info("%s\n", line);
}

static void do_debug(const struct mfrc522_command *cmd, const char *answer,
		     int answer_size)
{
	switch (cmd->cmd) {
	case MFRC522_CMD_MEM_READ:
		do_debug_read(answer, answer_size);
		break;
	case MFRC522_CMD_MEM_WRITE:
		do_debug_write(cmd->data);
		break;
	default:
		/* Do nothing */
		break;
	}
}

static ssize_t __mfrc522_write(struct mfrc522_state *state, const char *buffer,
			       size_t len)
{
	int answer_size;
	struct mfrc522_command command = { 0 };

	if (mfrc522_parse(&command, buffer, len) < 0) {
		pr_err("[MFRC522] Got invalid command\n");
		return -EINVAL;
	}

	pr_info("[MFRC522] Got following command: %d\n", command.cmd);

	// The data buffer is zero filled if no extra input has been given
	if (command.data[0])
		pr_info("[MFRC522] With extra data: `%s`\n", command.data);

	answer_size = mfrc522_execute(state, state->answer, &command);

	if (answer_size < 0) {
		// Error
		pr_err("[MFRC522] Error when executing command\n");
		return -EBADE;
	}

	if (state->debug_on)
		do_debug(&command, state->answer, answer_size);

	// Non-empty answer
	pr_info("[MFRC522] Answer: \"%.*s\"\n", answer_size, state->answer);
	state->buffer_full = true;

	return len;
}

static ssize_t mfrc522_write(struct file *file, const char *buffer, size_t len,
			     loff_t *offset)
{
	ssize_t ret;
	char *answer;
	struct mfrc522_state *state;
	char kernel_buffer[MFRC522_MAX_INPUT_LEN] = { 0 };

	if (len > MFRC522_MAX_INPUT_LEN)
		return -EINVAL;

	if (copy_from_user(kernel_buffer, buffer, len) != 0) {
		pr_err("[MFRC522] Fail to copy from user\n");
		return -EINVAL;
	}

	state = container_of(file->private_data, struct mfrc522_state, misc);
	answer = state->answer;

	pr_info("[MFRC522] Being written to: %.*s\n", len, kernel_buffer);

	ret = __mfrc522_write(state, kernel_buffer, len);

	return ret;
}

static ssize_t mfrc522_read(struct file *file, char *buffer, size_t len,
			    loff_t *offset)
{
	struct mfrc522_state *state;
	char *answer;

	pr_info("[MFRC522] Being read from\n");

	state = container_of(file->private_data, struct mfrc522_state, misc);

	answer = state->answer;

	if (!state->buffer_full)
		return 0;

	if (len > MFRC522_MEM_SIZE)
		len = MFRC522_MEM_SIZE;

	if (copy_to_user(buffer, answer, len) != 0) {
		pr_err("[MFRC522] Fail to copy to user\n");
		return -EINVAL;
	}

	state->buffer_full = false;

	return len;
}

static const struct file_operations mfrc522_fops = {
	.owner = THIS_MODULE,
	.write = mfrc522_write,
	.read = mfrc522_read,
};

static const struct of_device_id mfrc522_match_table[] = {
	{ .compatible = "mfrc522" },
	{} // NULL entry
};

static int mfrc522_spi_probe(struct spi_device *spi);

static struct spi_driver mfrc522_spi_driver = {
	.driver = {
		.name = "mfrc522",
		.owner = THIS_MODULE,
		.of_match_table = mfrc522_match_table,
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
		pr_info("[MFRC522] Current speed (%u)Hz is too high. Setting speed to %uHz\n",
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
	struct mfrc522_state *state;

	pr_info("MFRC522 init\n");

	state = kcalloc(1, sizeof(*state), GFP_KERNEL);

	if (!state)
		return -ENOMEM;

	state->misc = (struct miscdevice){
		.minor = MISC_DYNAMIC_MINOR,
		.name = "mfrc522_misc",
		.fops = &mfrc522_fops,
	};

	state->debug_on = false;

	ret = misc_register(&state->misc);
	if (ret) {
		pr_err("[MFRC522] Misc device initialization failed\n");
		return ret;
	}

	ret = spi_register_driver(&mfrc522_spi_driver);
	if (ret) {
		pr_err("[MFRC522] SPI Register failed\r\n");
		return ret;
	}

	g_state = state;

	return 0;
}

static void __exit mfrc522_exit(void)
{
	misc_deregister(&g_state->misc);
	spi_unregister_driver(&mfrc522_spi_driver);

	kfree(g_state);

	pr_info("MFRC522 exit\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);

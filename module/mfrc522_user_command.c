// SPDX-License-Identifier: GPL-2.0

#include "mfrc522_user_command.h"
#include "linux/kernel.h"
#include "linux/slab.h"
#include "linux/string.h"
#include "mfrc522_spi.h"

int mfrc522_command_init(struct mfrc522_command *cmd, u8 cmd_byte, char *data,
			 u8 data_len)
{
	if (data_len > MFRC522_MAX_DATA_LEN) {
		pr_err("[MFRC522] Invalid length for command: Got %d, expected length inferior to 25\n",
		       data_len);
		return -1;
	}

	if (!cmd) {
		pr_err("[MFRC522] Invalid command to initialize: NULL\n");
		return -2;
	}

	cmd->cmd = cmd_byte;
	cmd->data_len = data_len;

	// Copy the user's extra data into the command, and zero out the remaining bytes
	strncpy(cmd->data, data, data_len);
	memset(cmd->data + data_len, '\0', MFRC522_MAX_DATA_LEN - data_len);

	return 0;
}

int mfrc522_command_simple_init(struct mfrc522_command *cmd, u8 cmd_byte)
{
	return mfrc522_command_init(cmd, cmd_byte, NULL, 0);
}

/**
 * Read the internal memory of the MFRC522
 *
 * @return 0 on success, -1 on error
 */
// FIXME: Use answer to return data to module?
static int mem_read(void) {
	u8 answer_buf[MFRC522_MAX_FIFO_LEN] = { 0 };
	int byte_amount;

	mfrc522_fifo_flush();
	mfrc522_send_command(MFRC522_RCV_ON, MFRC522_POWER_DOWN_OFF, MFRC522_COMMAND_MEM);
	byte_amount = mfrc522_fifo_read(answer_buf);
	if (byte_amount < 0) {
		pr_err("[MFRC522] An error happened when reading MFRC522's internal memory\n");
		return -1;
	}

	pr_info("\"%*.s\"\n", byte_amount, answer_buf);

	return 0;
}

int mfrc522_execute(char *answer, struct mfrc522_command *cmd)
{
	switch (cmd->cmd) {
	case MFRC522_CMD_GET_VERSION:
		sprintf(answer, "%d", mfrc522_get_version());
		break;
	case MFRC522_CMD_MEM_READ:
		if (mem_read())
			return -1;
		break;
	default:
		strcpy(answer, "none");
	}

	// FIXME: Add logic
	return 0;
}

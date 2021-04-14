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
 * @return The size of the read on success, -1 on error
 */
static int mem_read(char *answer)
{
	int byte_amount = 0;

	mfrc522_fifo_flush();
	mfrc522_send_command(MFRC522_RCV_ON, MFRC522_POWER_DOWN_OFF,
			     MFRC522_COMMAND_MEM);

	byte_amount = mfrc522_fifo_read(answer);
	if (byte_amount < 0) {
		pr_err("[MFRC522] An error happened when reading MFRC522's internal memory\n");
		return -1;
	}

	pr_info("[MFRC522] Read %d bytes from memory\n", byte_amount);

	return byte_amount;
}

int mfrc522_execute(char *answer, struct mfrc522_command *cmd)
{
	int ret = -1;

	switch (cmd->cmd) {
	case MFRC522_CMD_GET_VERSION:
		ret = sprintf(answer, "%d", mfrc522_get_version());
		break;
	case MFRC522_CMD_MEM_READ:
		ret = mem_read(answer);
		break;
	default:
		ret = sprintf(answer, "%s", "Command unimplemented");
	}

	// FIXME: Add logic
	return ret;
}

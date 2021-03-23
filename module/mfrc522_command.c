// SPDX-License-Identifier: GPL-2.0

#include "mfrc522_command.h"
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

int mfrc522_execute(char *answer, struct mfrc522_command *cmd)
{
	switch (cmd->cmd) {
	case MFRC522_CMD_GET_VERSION:
		sprintf(answer, "%d", mfrc522_get_version());
		break;
	default:
		strcpy(answer, "none");
	}

	// FIXME: Add logic
	return 0;
}

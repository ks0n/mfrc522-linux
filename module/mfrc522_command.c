// SPDX-License-Identifier: GPL-2.0+

#include "mfrc522_command.h"
#include "linux/slab.h"

struct mfrc522_command *mfrc522_command_init(u8 cmd, char *data, u8 data_len)
{
	struct mfrc522_command *command =
		kmalloc(sizeof(struct mfrc522_command), GFP_KERNEL);
	if (!command) {
		pr_err("[MFRC522] Allocation of command (%d, %*.s, %d) failed",
		       cmd, data_len, data, data_len);

		return NULL;
	}

	command->cmd = cmd;
	command->data = data;
	command->data_len = data_len;

	return command;
}

struct mfrc522_command *mfrc522_command_simple_init(u8 cmd)
{
	return mfrc522_command_init(cmd, NULL, 0);
}

int mfrc522_execute(char *answer, struct mfrc522_command *cmd)
{
	// FIXME: Add logic
	return 0;
}

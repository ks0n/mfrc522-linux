// SPDX-License-Identifier: GPL-2.0

#include "mfrc522_user_command.h"
#include "linux/kernel.h"
#include "linux/slab.h"
#include "linux/string.h"
#include "mfrc522_spi.h"

#define MFRC522_MEM_SIZE 25

int mfrc522_command_init(struct mfrc522_command *cmd, u8 cmd_byte, char *data,
			 u8 data_len)
{
	if (data_len > MFRC522_MEM_SIZE) {
		pr_err("[MFRC522] Invalid length for command: Got %d, expected length inferior to 25\n",
		       data_len);
		return -1;
	}

	if (!cmd) {
		pr_err("[MFRC522] Invalid command to initialize: NULL\n");
		return -2;
	}

	cmd->cmd = cmd_byte;

	// Copy the user's extra data into the command, and zero out the remaining bytes
	strncpy(cmd->data, data, data_len);
	memset(cmd->data + data_len, '\0', MFRC522_MEM_SIZE - data_len);

	return 0;
}

int mfrc522_command_simple_init(struct mfrc522_command *cmd, u8 cmd_byte)
{
	return mfrc522_command_init(cmd, cmd_byte, NULL, 0);
}

/**
 * Read the internal memory of the MFRC522
 *
 * @param answer Buffer in which to store the memory's content
 *
 * @return The size of the read on success, -1 on error
 */
static int mem_read(char *answer)
{
	int byte_amount = 0;

	mfrc522_fifo_flush();
	if (mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
				 MFRC522_COMMAND_REG_POWER_DOWN_OFF,
				 MFRC522_COMMAND_MEM) < 0)
		return -1;

	byte_amount = mfrc522_fifo_read(answer);
	if (byte_amount < 0) {
		pr_err("[MFRC522] An error happened when reading MFRC522's internal memory\n");
		return -1;
	}

	pr_info("[MFRC522] Read %d bytes from memory\n", byte_amount);

	return byte_amount;
}

/**
 * Write 25 bytes of data into the MFRC522's internal memory
 *
 * @param data User input to write to the memory
 *
 * @return 0 on success, -1 on error
 */
static int mem_write(char *data)
{
	// We know that data is zero-filled since we initialized it using
	// mfrc522_command_init()
	if (mfrc522_fifo_write(data, MFRC522_MEM_SIZE) < 0) {
		pr_err("[MFRC522] Couldn't write to FIFO\n");
		return -1;
	}

	mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
			     MFRC522_COMMAND_REG_POWER_DOWN_OFF,
			     MFRC522_COMMAND_MEM);

	pr_info("[MFRC522] Wrote data to memory\n");

	return 0;
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
	case MFRC522_CMD_MEM_WRITE:
		ret = mem_write(cmd->data);
		break;
	default:
		ret = sprintf(answer, "%s", "Command unimplemented");
	}

	// FIXME: Add logic
	return ret;
}

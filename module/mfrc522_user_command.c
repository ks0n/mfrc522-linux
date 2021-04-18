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
 * Write data into the MFRC522's internal memory
 *
 * @param data User input to write to the memory
 * @param data_len Length of the data to write
 *
 * @return 0 on success, -1 on error
 */
static int mem_write(char *data, u8 data_len)
{
	u8 remaining_bytes;
	u8 null_char = '\0';

	if (mfrc522_fifo_write(data, data_len) < 0) {
		pr_err("[MFRC522] Couldn't write to FIFO\n");
		return -1;
	}

	// Complete the remaining slot with NULL bytes
	for (remaining_bytes = data_len; remaining_bytes < MFRC522_MAX_DATA_LEN;
	     remaining_bytes++)
		mfrc522_fifo_write(&null_char, 1);

	mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON, MFRC522_COMMAND_REG_POWER_DOWN_OFF,
			     MFRC522_COMMAND_MEM);

	pr_info("[MFRC522] Wrote %d bytes to memory\n", data_len);

	return 0;
}

/**
 * Generate a 10-byte wide random ID
 *
 * @param answer Buffer in which to store the answer
 *
 * @return The amount of bytes received on success, -1 on error
 */
static int generate_random(char *answer)
{
	u8 i;
	int byte_amount;
	u8 tmp_rand_id[MFRC522_MAX_DATA_LEN] = { 0 };

    // Clear the internal buffer
    mem_write(tmp_rand_id, MFRC522_MAX_DATA_LEN);

	mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
			     MFRC522_COMMAND_REG_POWER_DOWN_OFF,
			     MFRC522_COMMAND_GENERATE_RANDOM_ID);

	byte_amount = mem_read(tmp_rand_id);

    // Convert every received character to its actual value
	for (i = 0; i < byte_amount; i++)
		sprintf(answer + i, "%d", tmp_rand_id[i]);

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
	case MFRC522_CMD_MEM_WRITE:
		ret = mem_write(cmd->data, cmd->data_len);
		break;
	case MFRC522_CMD_GEN_RANDOM:
		ret = generate_random(answer);
		break;
	default:
		ret = sprintf(answer, "%s", "Command unimplemented");
	}

	// FIXME: Add logic
	return ret;
}

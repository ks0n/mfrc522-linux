// SPDX-License-Identifier: GPL-2.0

#include "mfrc522_user_command.h"
#include "linux/kernel.h"
#include "linux/slab.h"
#include "linux/string.h"
#include "mfrc522_spi.h"

#define MFRC522_ID_SIZE 10

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
 * @param stats Statistics in which to accumulate data
 *
 * @return The size of the read on success, -1 on error
 */
static int mem_read(char *answer, struct mfrc522_statistics *stats)
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

	if (stats)
		stats->bytes_read += byte_amount;

	return byte_amount;
}

/**
 * Write 25 bytes of data into the MFRC522's internal memory
 *
 * @param data User input to write to the memory
 * @param stats Statistics in which to accumulate data
 *
 * @return 0 on success, -1 on error
 */
static int mem_write(char *data, struct mfrc522_statistics *stats)
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

	if (stats)
		stats->bytes_written += MFRC522_MEM_SIZE;

	return 0;
}

/**
 * Generate a 10-byte wide random ID
 *
 * @param stats Statistics in which to accumulate data
 *
 * @return The amount of bytes received on success, -1 on error
 */
static int generate_random(struct mfrc522_statistics *stats)
{
	u8 buffer[MFRC522_MEM_SIZE] = { 0 };
	char char_buffer[MFRC522_ID_SIZE * 2 + 1] = { 0 };
	int i = 0;

	// Clear the internal buffer
	if (mem_write(buffer, stats) < 0)
		return -1;

	if (mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
				 MFRC522_COMMAND_REG_POWER_DOWN_OFF,
				 MFRC522_COMMAND_GENERATE_RANDOM_ID) < 0)
		return -1;

	/* We are reading for debug print, we don't need to report an error now
	 * but at the next mem_read command. So we don't check mem_read return
	 * value
	 */
	mem_read(buffer, stats);

	for (i = 0; i < MFRC522_ID_SIZE; i++) {
		// Each byte is 2 char wide in hexa so i*2
		sprintf(char_buffer + i * 2, "%02X", buffer[i]);
	}

	pr_info("[MFRC522] Generated random ID: %s\n", char_buffer);

	return 0;
}

static int set_debug(struct mfrc522_state *state,
		     const struct mfrc522_command *cmd)
{
	if (!strncmp(cmd->data, "on", 3))
		state->debug_on = true;
	else if (!strncmp(cmd->data, "off", 4))
		state->debug_on = false;
	else
		return -1;

	return 0;
}

int mfrc522_execute(struct mfrc522_state *state, char *answer,
		    struct mfrc522_command *cmd)
{
	int ret = -1;

	switch (cmd->cmd) {
	case MFRC522_CMD_GET_VERSION:
		ret = sprintf(answer, "%d", mfrc522_get_version());
		break;
	case MFRC522_CMD_MEM_READ:
		ret = mem_read(answer, &state->stats);
		break;
	case MFRC522_CMD_MEM_WRITE:
		ret = mem_write(cmd->data, &state->stats);
		break;
	case MFRC522_CMD_GEN_RANDOM:
		ret = generate_random(&state->stats);
		break;
	case MFRC522_CMD_DEBUG:
		ret = set_debug(state, cmd);
		break;
	default:
		ret = sprintf(answer, "%s", "Command unimplemented");
	}

	return ret;
}

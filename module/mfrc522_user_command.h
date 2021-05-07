/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_COMMAND_H
#define MFRC522_COMMAND_H

#include <linux/types.h>

#include "mfrc522_module.h"

#define MFRC522_MEM_SIZE 25
#define MFRC522_MAX_DATA_LEN 25
#define MFRC522_MAX_FIFO_LEN 64

enum mfrc522_commands {
	MFRC522_CMD_MEM_WRITE = 0x00,
	MFRC522_CMD_MEM_READ,
	MFRC522_CMD_GET_VERSION,
	MFRC522_CMD_GEN_RANDOM,
	MFRC522_CMD_DEBUG,
};

/**
 * MFRC522 User command. A user command contains a command, as well as some
 * optional arguments
 */
struct mfrc522_command {
	u8 cmd;
	char data[MFRC522_MEM_SIZE];
};

/**
 * Create a new MFRC522 command without checking for its validity
 *
 * @param cmd Command struct to initialize
 * @param cmd_byte Command to use, as defined in the MFRC522_CMD_* macros
 * @param data (Optional) Extra data to send alongside the command itself
 * @param data_len (Optional) Length of the extra data to send
 *
 * @return 0 on success, a negative number otherwise
 */
int mfrc522_command_init(struct mfrc522_command *cmd, u8 cmd_byte, char *data,
			 u8 data_len);

/**
 * Create a new simple MFRC522 without extra data
 *
 * @param cmd Command struct to initialize
 * @param cmd_byte Command to use, as defined in the MFRC522_CMD_* macros
 *
 * @return 0 on success, a negative number otherwise
 */
int mfrc522_command_simple_init(struct mfrc522_command *cmd, u8 cmd_byte);

/**
 * Execute a MFRC522 command and check for its validity
 *
 * @param answer Buffer in which to store the MFRC522's answer
 * @param cmd Command to send to the MFRC522
 *
 * @return The size of the answer on success, -1 on error
 */
int mfrc522_execute(struct mfrc522_state *state, char *answer, struct mfrc522_command *cmd);

#endif /* ! MFRC522_COMMAND_H */

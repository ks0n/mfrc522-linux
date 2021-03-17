#ifndef MFRC522_COMMAND_H
#define MFRC522_COMMAND_H

#include <linux/types.h>

enum mfrc522_commands {
    MFRC522_CMD_MEM_WRITE = 0x00,
    MFRC522_CMD_MEM_READ,
    MFRC522_CMD_GET_VERSION,
    MFRC522_CMD_GEN_RANDOM,
};

struct mfrc522_command {
    u8 cmd;
    u8* data;
    u8 data_len;
};

/**
 * Create a new MFRC522 command without checking for its validity
 *
 * @param cmd Command to use, as defined in the MFRC522_CMD_* macros
 * @param data (Optional) Extra data to send alongside the command itself
 * @param data_len (Optional) Length of the extra data to send
 *
 * @return The newly created command
 */
struct mfrc522_command *mfrc522_command_init(u8 cmd, u8* data, u8 data_len);

/**
 * Create a new simple MFRC522 without extra data
 *
 * @param cmd Command to use, as defined in the MFRC522_CMD_* macros
 *
 * @return The newly created command
 */
struct mfrc522_command *mfrc522_command_simple_init(u8 cmd);

/**
 * Execute a MFRC522 command and check for its validity
 *
 * @param answer Buffer in which to store the MFRC522's answer
 * @param command Command to send to the MFRC522
 *
 * @return The size of the answer on success, -1 on error
 */
int mfrc522_execute(char *answer, struct mfrc522_command *cmd);

#endif /* ! MFRC522_COMMAND_H */

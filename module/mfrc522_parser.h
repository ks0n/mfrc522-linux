/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_PARSER_H
#define MFRC522_PARSER_H

#include <linux/types.h>

#include "mfrc522_user_command.h"

#define MFRC522_MAX_INPUT_LEN 255

/**
 * Parse and check input sent to the MFRC522. Return the command asked by the user
 *
 * @param cmd Command struct to fill up
 * @param input User string input. Should be colon separated according to a specific
 *              format
 * @param len Length of the input
 *
 * @return 0 on success, a negative number otherwise
 */
int mfrc522_parse(struct mfrc522_command *cmd, const char *input, size_t len);

#endif /* !MFRC522_PARSER_H */

/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_DEBUG
#define MFRC522_DEBUG

#include "mfrc522_user_command.h"

/**
 * Show debug information based on the given MFRC522 command
 *
 * @param cmd User command
 * @param answer Data returned by the command executor
 * @param answer_size Size of the answer
 */
void do_debug(const struct mfrc522_command *cmd, const char *answer,
	      int answer_size);

#endif /* ! MFRC522_DEBUG */

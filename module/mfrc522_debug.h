/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_DEBUG
#define MFRC522_DEBUG

#include "mfrc522_user_command.h"

void do_debug(const struct mfrc522_command *cmd, const char *answer,
	      int answer_size);

#endif /* ! MFRC522_DEBUG */

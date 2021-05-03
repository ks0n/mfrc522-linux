// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>

#include "mfrc522_debug.h"

static void __print_bytes(const char *bytes, int len)
{
	int i;
	char line[16];
	int write_head = 0;

	for (i = 1; i < len + 1; i++) {
		write_head +=
			snprintf(line + write_head, 16, "%02x", bytes[i - 1]);

		if (i % 5 == 0) {
			pr_info("%s\n", line);
			write_head = 0;
		} else {
			write_head += snprintf(line + write_head, 16, " ");
		}
	}

	if (write_head > 0)
		pr_info("%s\n", line);
}

static void do_debug_read(const char *answer, int answer_size)
{
	pr_info("RD\n");

	__print_bytes(answer, answer_size);
}

static void do_debug_write(const char *cmd)
{
	pr_info("WR\n");

	__print_bytes(cmd, strlen(cmd));
}

void do_debug(const struct mfrc522_command *cmd, const char *answer,
	      int answer_size)
{
	switch (cmd->cmd) {
	case MFRC522_CMD_MEM_READ:
		do_debug_read(answer, answer_size);
		break;
	case MFRC522_CMD_MEM_WRITE:
		do_debug_write(cmd->data);
		break;
	default:
		/* Do nothing */
		break;
	}
}

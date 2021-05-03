/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_MODULE_H
#define MFRC522_MODULE_H

#define MFRC522_MAX_ANSWER_SIZE 256 // FIXME

#include <linux/types.h>
#include <linux/miscdevice.h>

struct mfrc522_statistics {
	unsigned int bytes_read;
	unsigned int bytes_written;
};

struct mfrc522_state {
	struct miscdevice misc;
	bool buffer_full;
	char answer[MFRC522_MAX_ANSWER_SIZE];
	bool debug_on;
	struct mfrc522_statistics stats;
};

#endif /* ! MFRC522_MODULE_H */

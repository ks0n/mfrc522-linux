/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_MODULE_H
#define MFRC522_MODULE_H

#define MFRC522_MAX_ANSWER_SIZE 256 // FIXME

#include <linux/types.h>
#include <linux/miscdevice.h>

/**
 * The mfrc522_statistics structure keeps track of the amounts of bytes written and read
 * by the MFRC522 driver
 */
struct mfrc522_statistics {
	unsigned int bytes_read;
	unsigned int bytes_written;
};

/**
 * Keep global information about the MFRC522 driver. This includes the answer buffer,
 * in which the MFRC522's memory content shall be kept between writes and reads, as well
 * as statistics and information
 */
struct mfrc522_state {
	struct miscdevice misc;
	bool buffer_full;
	char answer[MFRC522_MAX_ANSWER_SIZE];
	bool debug_on;
	struct mfrc522_statistics stats;
};

#endif /* ! MFRC522_MODULE_H */

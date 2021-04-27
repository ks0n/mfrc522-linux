#ifndef MFRC522_MODULE_H
#define MFRC522_MODULE_H

#define MFRC522_MAX_ANSWER_SIZE 256 // FIXME

#include <linux/types.h>
#include <linux/miscdevice.h>

struct mfrc522_state {
	struct miscdevice misc;
	bool buffer_full;
	char answer[MFRC522_MAX_ANSWER_SIZE];
	bool debug_on;
};

#endif /* ! MFRC522_MODULE_H */

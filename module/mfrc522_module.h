#ifndef MFRC522_MODULE_H
#define MFRC522_MODULE_H

struct mfrc522_state {
	struct miscdevice misc;
	bool buffer_full;
	char answer[MFRC522_MAX_ANSWER_SIZE];
	bool debug_on;
};

#endif /* ! MFRC522_MODULE_H */

#ifndef MFRC522_PARSER_H
#define MFRC522_PARSER_H

#include <linux/types.h>

#include "mfrc522_command.h"

/**
 * Parse and check input sent to the MFRC522. Return the command asked by the user
 *
 * @param input User string input. Should be colon separated according to a specific
 *              format
 * @param len Length of the input
 *
 * @return The required command on success, NULL otherwise
 */
struct mfrc522_command *mfrc522_parse(const char *input, size_t len);

#endif /* !MFRC522_PARSER_H */

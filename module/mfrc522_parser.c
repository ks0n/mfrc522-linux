// SPDX-License-Identifier: GPL-2.0

#include "linux/kernel.h"
#include "mfrc522_command.h"

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "mfrc522_parser.h"

#define MFRC522_SEPARATOR ":"
#define MFRC522_CMD_AMOUNT 4
#define MFRC522_MAX_INPUT_LEN 255

struct command {
	const char *input;
	u8 parameter_amount;
	u8 cmd;
};

static const struct command commands[MFRC522_CMD_AMOUNT] = {
	{ .input = "mem_write",
	  .parameter_amount = 2,
	  .cmd = MFRC522_CMD_MEM_WRITE },
	{ .input = "mem_read",
	  .parameter_amount = 0,
	  .cmd = MFRC522_CMD_MEM_READ },
	{ .input = "gen_rand_id",
	  .parameter_amount = 0,
	  .cmd = MFRC522_CMD_GEN_RANDOM },
	{ .input = "version",
	  .parameter_amount = 0,
	  .cmd = MFRC522_CMD_GET_VERSION },
};

/**
 * Get the command associated with a command name
 *
 * @param token Command name found during parsing
 *
 * @return Return a pointer to a command declared in the commands array, or NULL if no
 *         command matched the input
 */
static const struct command *cmd_from_token(const char *token)
{
	size_t i;

	for (i = 0; i < MFRC522_CMD_AMOUNT; i++)
		if (!strcmp(token, commands[i].input))
			return &commands[i];

	return NULL;
}

/**
 * Parse a command with multiple arguments
 *
 * @param cmd Command struct to fill up
 * @param input Remaining user input, mutable and allocated via kmalloc() in mfrc522_parse()
 * @param ref_cmd Reference command returned by cmd_from_token()
 *
 * @return 0 on success, a negative number otherwise
 */
static int parse_multi_arg(struct mfrc522_command *cmd, char *input,
			   const struct command *ref_cmd)
{
	// We only enter this function if arguments have been given to the input
	u8 parameter_amount = 0;
	char *token;
	char *extra_data;
	u8 extra_data_len;
	int ret;

	while (input) {
		token = strsep(&input, MFRC522_SEPARATOR);
		parameter_amount++;

		// The first parameter is the length of the extra data
		if (parameter_amount == 1) {
			ret = kstrtou8(token, 10, &extra_data_len);
			if (ret == -EINVAL) {
				pr_err("[MFRC522] Invalid parameter for Data length: Expected number but got %s\n",
				       token);
				return ret;
			}

			if (ret == -ERANGE) {
				pr_err("[MFRC522] Invalid parameter for Data length: Expected Unsigned Byte (0 - 255) but got %s\n",
				       token);
				return ret;
			}

			if (extra_data_len > MFRC522_MAX_DATA_LEN) {
				pr_err("[MFRC522] Invalid parameter for Data length: Length %d is too important (max length: 25)\n",
				       extra_data_len);
				return -1;
			}
		}

		// The second parameter is the extra data
		if (parameter_amount == 2)
			extra_data = token;
	}

	pr_info("[MFRC522] Found %d parameters for command %s\n",
		parameter_amount, ref_cmd->input);

	if (parameter_amount != ref_cmd->parameter_amount) {
		pr_err("[MFRC522] Invalid command: %s: Expected %d arguments but got %d\n",
		       ref_cmd->input, ref_cmd->parameter_amount,
		       parameter_amount);
		return -2;
	}

	return mfrc522_command_init(cmd, ref_cmd->cmd, extra_data,
				    extra_data_len);
}

int mfrc522_parse(struct mfrc522_command *cmd, const char *input, size_t len)
{
	// Inputs are always organized according to the same format
	//
	// <cmd>[:<extra_data_len>:<extra_data>]
	//
	// Therefore, we will either get a simple command without any colons (in which case
	// we will only be able to run strsep on the string ONCE) or a command with colons
	// in which case it is required for the length of the extra data to be present.
	// Also, since strsep modifies its input, we need to create a copy of the user's
	// input.

	// The input's length does not account for a terminating NULL, but `strsep` expects
	// it. Allocate one more byte for the NULL terminator
	char input_cpy[MFRC522_MAX_INPUT_LEN + 1] = { 0 };
	char *input_mut = &input_cpy[0];
	char *token;
	const struct command *command;

	if (len > MFRC522_MAX_INPUT_LEN) {
		pr_err("[MFRC522] Invalid input length: Max is %d, got %d\n",
		       MFRC522_MAX_INPUT_LEN, len);
		return -1;
	}

	// `strlcpy`, while being the safer version, expects a NULL-terminated string as source.
	// However, the kernel does not NULL-terminate the user's input
	strncpy(input_cpy, input, len);

	token = strsep(&input_mut, MFRC522_SEPARATOR);
	command = cmd_from_token(token);

	// The command does not exist
	if (!command) {
		pr_err("[MFRC522] Invalid command: %s: Does not exist\n",
		       token);
		goto error;
	}

	// Now, two cases are possible: If we didn't find a colon, strsep will nullify our
	// original pointer (input_mut). If we found one, then input_mut is still valid
	if (!input_mut) {
		if (command->parameter_amount != 0) {
			pr_err("[MFRC522] Invalid command: %s: Expected %d arguments but got 0\n",
			       input, command->parameter_amount);
			goto error;
		}

		return mfrc522_command_simple_init(cmd, command->cmd);
	}

	// If `input_mut` is not NULL, then we have found colons. However, we might have
	// also found a command that does not expect any arguments
	if (command->parameter_amount == 0) {
		pr_err("[MFRC522] Invalid command: %s: Expected 0 arguments but found some\n",
		       token);
		goto error;
	}

	return parse_multi_arg(cmd, input_mut, command);

error:
	return -1;
}

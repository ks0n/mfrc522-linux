// SPDX-License-Identifier: GPL-2.0

#include "linux/kernel.h"
#include "mfrc522_user_command.h"

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "mfrc522_parser.h"

#define MFRC522_SEPARATOR ":"
#define MFRC522_CMD_AMOUNT 5
#define MFRC522_MAX_PARAMETER_AMOUNT 2

struct driver_command {
	const char *input;
	u8 parameter_amount;
	u8 cmd;
};

static const struct driver_command commands[MFRC522_CMD_AMOUNT] = {
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
	{ .input = "debug", .parameter_amount = 1, .cmd = MFRC522_CMD_DEBUG },
};

/**
 * Get the command associated with a command name
 *
 * @param token Command name found during parsing
 *
 * @return Return a pointer to a command declared in the commands array, or NULL if no
 *         command matched the input
 */
static const struct driver_command *find_cmd_from_token(const char *token)
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
 * @param input Remaining user input, allocated on mfrc522_parse()'s stack
 * @param ref_cmd Reference command returned by find_cmd_from_token()
 *
 * @return 0 on success, a negative number otherwise
 */
static int parse_multi_arg(struct mfrc522_command *cmd, char *input,
			   const struct driver_command *ref_cmd)
{
	u8 extra_data_len;
	char *extra_data;
	char *token;
	int ret;

	token = strsep(&input, MFRC522_SEPARATOR);
	ret = kstrtou8(token, 10, &extra_data_len);

	if (ref_cmd->parameter_amount == 1) {
		extra_data = token;
		extra_data_len = strnlen(extra_data, 4);
		goto finish;
	}

	// The remaining user input is the extra data
	extra_data = input;

	if (ret == -EINVAL) {
		pr_err("[MFRC522] Invalid parameter for Data length: Expected number but got \"%s\"\n",
		       token);
		return ret;
	}

	if (ret == -ERANGE) {
		pr_err("[MFRC522] Invalid parameter for Data length: Expected Unsigned Byte (0 - 255) but got \"%s\"\n",
		       token);
		return ret;
	}

	if (extra_data_len > MFRC522_MAX_DATA_LEN) {
		pr_err("[MFRC522] Invalid parameter for Data length: Length %d is too important (max length: 25)\n",
		       extra_data_len);
		return -1;
	}

	// We haven't found a semicolon in the user's input. Therefore, we're in
	// a case where we have a length, but no data. This is an invalid amount
	// of parameters
	if (!extra_data) {
		pr_err("[MFRC522] Invalid command: %s: Expected %d arguments but got 1\n",
		       ref_cmd->input, ref_cmd->parameter_amount);
		return -1;
	}

finish:
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
	char *input_mut = input_cpy;
	char *cmd_name;
	const struct driver_command *command;

	// `strlcpy`, while being the safer version, expects a NULL-terminated string as source.
	// However, the kernel does not NULL-terminate the user's input
	strncpy(input_cpy, input, len);

	cmd_name = strsep(&input_mut, MFRC522_SEPARATOR);
	command = find_cmd_from_token(cmd_name);

	// The command does not exist
	if (!command) {
		pr_err("[MFRC522] Invalid command: %s: Does not exist\n",
		       cmd_name);
		return -1;
	}

	// Now, two cases are possible: If we didn't find a colon, strsep will nullify our
	// original pointer (input_mut). If we found one, then input_mut is still valid
	if (!input_mut) {
		if (command->parameter_amount != 0) {
			pr_err("[MFRC522] Invalid command: %s: Expected %d arguments but got 0\n",
			       cmd_name, command->parameter_amount);
			return -2;
		}

		return mfrc522_command_simple_init(cmd, command->cmd);
	}

	return parse_multi_arg(cmd, input_mut, command);
}

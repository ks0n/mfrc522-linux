#include "mfrc522_command.h"

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "mfrc522_parser.h"

#define MFRC522_SEPARATOR ":"
#define MFRC522_CMD_AMOUNT 3

struct command {
    const char *input;
    u8 parameter_amount;
    u8 cmd;
};

static struct command commands[MFRC522_CMD_AMOUNT] = {
    { .input = "mem_write", .parameter_amount = 2, .cmd = MFRC522_CMD_MEM_WRITE },
    { .input = "mem_read", .parameter_amount = 0, .cmd = MFRC522_CMD_MEM_READ },
    { .input = "gen_rand_id", .parameter_amount = 0, .cmd = MFRC522_CMD_GEN_RANDOM },
};

static struct command *cmd_from_token(const char *token) {
    size_t i;

    for (i = 0; i < MFRC522_CMD_AMOUNT; i++)
        if (!strcmp(token, commands[i].input))
            return &commands[i];

    return NULL;
}

struct mfrc522_command *parse_multi_arg(char *input, struct command *cmd) {
    // We only enter this function if arguments have been given to the input
    u8 parameter_amount = 0;
    char *token;

    while (input) {
        token = strsep(&input, MFRC522_SEPARATOR);
        parameter_amount++;
    }

    return NULL;
}

struct mfrc522_command *mfrc522_parse(const char *input, size_t len) {
    // Inputs are always organized according to the same format
    //
    // <cmd>[:<extra_data>:<extra_data_len>]
    //
    // Therefore, we will either get a simple command without any colons (in which case
    // we will only be able to run strsep on the string ONCE) or a command with colons
    // in which case it is required for the length of the extra data to be present.
    // Also, since strsep modifies its input, we need to create a copy of the user's
    // input.

    // The input's length does not account for a terminating NULL, but `strsep` expects
    // it. Allocate one more byte for the NULL terminator
    char *input_mut = kmalloc(len + 1, GFP_KERNEL);
    char *token;
    struct command *command;

    if (!input_mut) {
        pr_err("[MFRC522] Failed to copy user input %*.s\n", len, input);
        return NULL;
    }

    // `strlcpy`, while being the safer version, expects a NULL-terminated string as source.
    // However, the kernel does not NULL-terminate the user's input
    strncpy(input_mut, input, len);

    input_mut[len] = '\0';

    token = strsep(&input_mut, MFRC522_SEPARATOR);
    command = cmd_from_token(token);

    // The command does not exist
    if (!command) {
        pr_err("[MFRC522] Invalid command: %s: Does not exist\n", token);
        return NULL;
    }

    // Now, two cases are possible: If we didn't find a colon, strsep will nullify our
    // original pointer (input_mut). If we found one, then input_mut is still valid
    if (!input_mut) {
        if (command->parameter_amount != 0) {
            pr_err("[MFRC522] Invalid command: %s: Expected %d arguments but got 0\n", input, command->parameter_amount);
            return NULL;
        }

        return mfrc522_command_simple_init(command->cmd);
    }

    // If `input_mut` is not NULL, then we have found colons. However, we might have
    // also found a command that does not expect any arguments
    if (command->parameter_amount == 0) {
        pr_err("[MFRC522] Invalid command: %s: Expected 0 arguments but found some\n", input);
        return NULL;
    }

    return parse_multi_arg(input_mut, command);
}

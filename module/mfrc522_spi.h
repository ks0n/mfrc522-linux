/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_SPI_H
#define MFRC522_SPI_H

#include <linux/spi/spi.h>
#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/compiler.h>

/**
 * Abstraction on the format used to define the address bytes sent to the MFRC522
 * as part of an SPI transfer
 */
struct address_byte {
	u8 lsb : 1; // 0
	u8 addr : 6;
	u8 rw : 1; // 1 -> Read, 0 -> Write
} __packed;

#define MFRC522_SPI_MAX_CLOCK_SPEED 1000000

#define MFRC522_SPI_WRITE 0
#define MFRC522_SPI_READ 1

// MFRC522 commands, see 10.3
#define MFRC522_COMMAND_IDLE 0b0000
#define MFRC522_COMMAND_MEM 0b0001
#define MFRC522_COMMAND_GENERATE_RANDOM_ID 0b0010
#define MFRC522_COMMAND_CALC_CRC 0b0011
#define MFRC522_COMMAND_TRANSMIT 0b0100
#define MFRC522_COMMAND_NO_CMD_CHANGE 0b0111
#define MFRC522_COMMAND_RECEIVE 0b1000
#define MFRC522_COMMAND_TRANSCEIVE 0b1100
#define MFRC522_COMMAND_MF_AUTHENT 0b1110
#define MFRC522_COMMAND_SOFT_RESET 0b1111

// MFRC522 registers, see 9.2
#define MFRC522_COMMAND_REG 0x1
#define MFRC522_FIFO_DATA_REG 0x9
#define MFRC522_FIFO_LEVEL_REG 0xA
#define MFRC522_VERSION_REG 0x37

// Helpers for command register values
#define MFRC522_COMMAND_REG_RCV_ON 0
#define MFRC522_COMMAND_REG_RCV_OFF 1
#define MFRC522_COMMAND_REG_POWER_DOWN_ON 1
#define MFRC522_COMMAND_REG_POWER_DOWN_OFF 0

/**
 * Global SPI device registered by our driver during probing
 */
extern struct spi_device *mfrc522_spi;

/**
 * Build a SPI address byte from a given address and a mode
 */
struct address_byte address_byte_build(u8 mode, u8 addr);

/**
 * Get the version number of the attached MFRC522
 *
 * @return A negative number on error, the version number otherwise
 */
int mfrc522_get_version(void);

/**
 * Read the FIFO level of the MFRC522
 *
 * @return A positive number indicating the amount of bytes in the FIFO on success,
 *         a negative number otherwise
 */
int mfrc522_fifo_level(void);

/**
 * Flush the FIFO buffer of the MFRC522
 */
void mfrc522_fifo_flush(void);

/**
 * Send an MFRC522 command (9.3.1.2)
 * Parameters are not checked, you should use provided macros
 *
 * @param rcv_off If 1, turn off analog part of the receiver
 * @param power_down If 1, enter soft power down mode
 * @param command MFRC522 commands as described 10.3
 *
 * @return 0 on success, -1 on error
 */
int mfrc522_send_command(u8 rcv_off, u8 power_down, u8 command);

/**
 * Read the CommandReg register and return the MFRC522's current command
 *
 * @return The current command on success, a negative number on error
 */
int mfrc522_read_command(void);

/**
 * Read FIFO content into a provided buffer
 *
 * @param buf Buffer to write the FIFO content to. It must be at least MFRC522_MAX_FIFO_SIZE wide
 *
 * @return A negative number on error, number of byte read otherwise
 */
int mfrc522_fifo_read(u8 *buf);

/**
 * Write content to the MFRC522's FIFO
 *
 * @warn The FIFO's max size is 64 bytes
 *
 * @param buf Buffer from which to write into the FIFO
 * @param len Amount of bytes to write to the FIFO
 *
 * @return 0 on success, -1 on error
 */
int mfrc522_fifo_write(const u8 *buf, size_t len);

/**
 * Reads a mfrc522 register
 *
 * @param client SPI client to talk to
 * @param reg Register to read from
 * @param read_buff Buffer to write the read content to. It must be at least read_len wide
 * @param read_len Number of bytes to read
 *
 * @return A negative number on error, 0 on success
 */
int mfrc522_register_read(struct spi_device *client, u8 reg, u8 *read_buff,
			  u8 read_len);

/**
 * Write a value to a mfrc522 register
 *
 * @param client SPI client to talk to
 * @param reg Register to write to
 * @param value Data to write in the register
 *
 * @return A negative number on error, 0 on success
 */
int mfrc522_register_write(struct spi_device *client, u8 reg, u8 value);

#endif /* !MFRC522_SPI_H */

/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_SPI_H
#define MFRC522_SPI_H

#include <linux/spi/spi.h>
#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/compiler.h>

struct address_byte {
	u8 lsb : 1; // 0
	u8 addr : 6;
	u8 rw : 1; // 1 -> Read, 0 -> Write
} __packed;

#define MFRC522_SPI_MAX_CLOCK_SPEED 1000000

#define MFRC522_SPI_WRITE 0
#define MFRC522_SPI_READ 1

#define MFRC522_VERSION_REG 0x37

extern struct spi_device *mfrc522_spi;

/**
 * Build a SPI address byte from a given address and a mode
 */
struct address_byte address_byte_build(u8 mode, u8 addr);

/**
 * Get the version number of the attached MFRC522
 */
u8 mfrc522_get_version(void);

/**
 * Reads a mfrc522 register
 *
 * @param client SPI client to talk to
 * @param reg Register to read from
 *
 * @return Value read from the register
 */
u8 mfrc522_register_read(struct spi_device *client, u8 reg);

/**
 * Write a value to a mfrc522 register
 *
 * @param client SPI client to talk to
 * @param reg Register to write to
 * @param value Data to write in the register
 */
void mfrc522_register_write(struct spi_device *client, u8 reg, u8 value);

#endif /* !MFRC522_SPI_H */


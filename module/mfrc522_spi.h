/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_SPI_H
#define MFRC522_SPI_H

#include <linux/types.h>
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

/**
 * Build a SPI address byte from a given address and a mode
 */
struct address_byte address_byte_build(u8 mode, u8 addr);

#endif /* !MFRC522_SPI_H */

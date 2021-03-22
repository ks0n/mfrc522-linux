// SPDX-License-Identifier: GPL-2.0

#include "mfrc522_spi.h"

struct address_byte address_byte_build(u8 mode, u8 addr)
{
	struct address_byte byte = {
		.addr = addr,
		.rw = mode,
	};

	return byte;
}

u8 mfrc522_register_read(struct spi_device *client, u8 reg)
{
	struct address_byte reg_read =
		address_byte_build(MFRC522_SPI_READ, reg);
	u8 value;

	spi_write(client, &reg_read, 1);
	spi_read(client, &value, 1);

	return value;
}

void mfrc522_register_write(struct spi_device *client, u8 reg, u8 value)
{
	struct address_byte reg_write =
		address_byte_build(MFRC522_SPI_WRITE, reg);

	spi_write(client, &reg_write, 1);
	spi_write(client, &value, 1);
}

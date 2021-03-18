// SPDX-License-Identifier: GPL-2.0

#include <linux/spi/spi.h>

#include "mfrc522_spi.h"

struct spi_device *mfrc522_spi;

struct address_byte address_byte_build(u8 mode, u8 addr)
{
	struct address_byte byte = {
		.addr = addr,
		.rw = mode,
	};

	return byte;
}

u8 mfrc522_get_version(void)
{
	struct address_byte version_reg_read =
		address_byte_build(MFRC522_SPI_READ, MFRC522_VERSION_REG);
	u8 version;

	if (!mfrc522_spi)
		return 0;

	spi_write(mfrc522_spi, &version_reg_read, 1);
	spi_read(mfrc522_spi, &version, 1);

	return version;
}

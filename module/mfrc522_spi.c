// SPDX-License-Identifier: GPL-2.0

#include <linux/spi/spi.h>
#include <linux/string.h>

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

int mfrc522_get_version(void)
{
	u8 version;
	int ret;

	ret = mfrc522_register_read(mfrc522_spi, MFRC522_VERSION_REG, &version,
				    1);

	if (ret < 0)
		return ret;

	return version;
}

void mfrc522_fifo_flush(void)
{
	u8 flush_byte = 1 << 7;

	mfrc522_register_write(mfrc522_spi, MFRC522_FIFO_LEVEL_REG, flush_byte);
}

static void wait_for_cmd(void)
{
	int cmd;

	do {
		cmd = mfrc522_read_command();
		pr_info("[MFRC522] Current command is: 0x%x\n", cmd);
	} while (cmd != MFRC522_COMMAND_IDLE);
}

void mfrc522_send_command(u8 rcv_off, u8 power_down, u8 command)
{
	u8 command_byte = rcv_off << 5 | power_down << 4 | command;

	mfrc522_register_write(mfrc522_spi, MFRC522_COMMAND_REG, command_byte);

	wait_for_cmd();
}

int mfrc522_read_command(void)
{
	u8 command_reg;
	int ret;

	ret = mfrc522_register_read(mfrc522_spi, MFRC522_COMMAND_REG,
				    &command_reg, 1);

	if (ret < 0)
		return ret;

	return command_reg & 0b00001111;
}

int mfrc522_fifo_level(void)
{
	u8 fifo_level;
	int ret;

	ret = mfrc522_register_read(mfrc522_spi, MFRC522_FIFO_LEVEL_REG,
				    &fifo_level, 1);
	if (ret < 0)
		return ret;

	// Mask the MSb to get the amount of bytes in the FIFO buffer
	fifo_level &= 0x7F;

	pr_info("[MFRC522] Fifo level: %d\n", fifo_level);

	return fifo_level;
}

int mfrc522_fifo_read(u8 *buf)
{
	int ret;
	int fifo_level = mfrc522_fifo_level();

	if (fifo_level < 0)
		return fifo_level;

	ret = mfrc522_register_read(mfrc522_spi, MFRC522_FIFO_DATA_REG, buf,
				    fifo_level);
	if (ret < 0)
		return ret;

	return fifo_level;
}

int mfrc522_fifo_write(u8 *buf, size_t len)
{
	size_t i;
	int ret;

	for (i = 0; i < len; i++) {
		ret = mfrc522_register_write(mfrc522_spi, MFRC522_FIFO_DATA_REG,
					     buf[i]);
		if (ret < 0)
			return ret;
	}

	return 0;
}

int mfrc522_register_read(struct spi_device *client, u8 reg, u8 *read_buff,
			  u8 read_len)
{
	size_t i;
	int ret;
	struct address_byte reg_read =
		address_byte_build(MFRC522_SPI_READ, reg);

	for (i = 0; i < read_len; i++) {
		ret = spi_write_then_read(client, &reg_read, 1, read_buff + i,
					  1);
		if (ret < 0)
			return ret;
	}

	return read_len;
}

int mfrc522_register_write(struct spi_device *client, u8 reg, u8 value)
{
	struct address_byte reg_write =
		address_byte_build(MFRC522_SPI_WRITE, reg);
	u8 data[2] = { 0, value };

	// We cannot directly put reg_write in data[0] at init time
	memcpy(&data[0], &reg_write, sizeof(u8));

	return spi_write(client, &data, 2);
}

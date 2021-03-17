#include "mfrc522_spi.h"

struct address_byte address_byte_build(u8 mode, u8 addr)
{
	struct address_byte byte = {
		.addr = addr,
		.rw = mode,
	};

	return byte;
}

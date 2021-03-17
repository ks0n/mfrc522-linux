# Default configuration
RELEASE = m
TEST = n

ifeq ($(ENABLE_TESTS), 1)
	RELEASE = n
	TEST = m
endif

obj-$(RELEASE) += mfrc522.o
obj-$(TEST) += mfrc522_tests.o

mfrc522-objs += mfrc522_module.o mfrc522_parser.o mfrc522_command.o

MAKE = make -C ../linux/ M=$(PWD)

all:
	$(MAKE) modules

clean:
	$(MAKE) clean

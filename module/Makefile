# Default configuration
RELEASE = m
TEST = n

ifeq ($(ENABLE_TESTS), 1)
	RELEASE = n
	TEST = m
endif

obj-$(RELEASE) += mfrc522.o
obj-$(TEST) += mfrc522_tests.o

mfrc522-objs += mfrc522_module.o mfrc522_parser.o mfrc522_command.o mfrc522_spi.o

MAKE = make -C ../linux/ M=$(PWD)

all:
	$(MAKE) modules

modules:
	$(MAKE) modules

clean:
	$(MAKE) clean

flash: modules
	scp mfrc522.ko pi@192.168.103.202:~/

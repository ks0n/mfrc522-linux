obj-m += mfrc522.o

mfrc522-objs += mfrc522_module.o \
				mfrc522_parser.o \
				mfrc522_user_command.o \
				mfrc522_spi.o \
				mfrc522_debug.o

MAKE = make -C ../linux/ M=$(PWD)

all:
	$(MAKE) modules

modules:
	$(MAKE) modules

clean:
	$(MAKE) clean

flash: modules
	scp mfrc522.ko pi@192.168.103.202:~/

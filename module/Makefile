RELEASE = n
TEST = m

obj-$(RELEASE) += mfrc522.o
obj-$(TEST) += mfrc522_tests.o

mfrc522-objs += mfrc522_module.o mfrc522_parser.o

MAKE = make -C ../linux/ M=$(PWD)

all:
	$(MAKE) modules

clean:
	$(MAKE) clean

obj-m += mfrc522.o

MAKE = make LLVM=1 -C ../linux-next M=$(PWD)

all: modules

modules:
	$(MAKE) modules

clean:
	$(MAKE) clean

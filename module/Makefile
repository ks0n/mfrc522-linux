obj-m += mfrc522.o

MAKE = make -C ../linux/ M=$(PWD)

all:
	$(MAKE) modules

modules:
	$(MAKE) modules

clean:
	$(MAKE) clean

flash: modules
	scp mfrc522.ko pi@192.168.103.202:~/

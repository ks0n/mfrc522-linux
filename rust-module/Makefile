RUST_DIR = mfrc522-rs
RUST_LIB = libmfrc522

obj-m += mfrc522.o
mfrc522-objs += $(RUST_LIB).o

MAKE = make -C ../linux M=$(PWD)

all: modules

modules:
	cd $(RUST_DIR)/ && cargo build --release
	touch .$(RUST_LIB).o.cmd
	cp $(RUST_DIR)/target/release/$(RUST_LIB).a $(RUST_LIB).o_shipped
	$(MAKE) modules

clean:
	cd $(RUST_DIR)/ && cargo clean
	rm $(RUST_LIB).o_shipped
	$(MAKE) clean

flash: modules
	scp mfrc522.ko pi@192.168.103.202:~/

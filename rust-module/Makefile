RUST_DIR = mfrc522-rs
RUST_LIB = libmfrc522
TARGET = armv7-unknown-linux-musleabi

obj-m += mfrc522.o
mfrc522-objs += module.o $(RUST_LIB).o

MAKE = make -C ../linux M=$(PWD)

all: modules

modules:
	cd $(RUST_DIR)/ && cargo build --release --target $(TARGET)
	touch .$(RUST_LIB).o.cmd
	cp $(RUST_DIR)/target/$(TARGET)/release/$(RUST_LIB).a $(RUST_LIB).o_shipped
	$(MAKE) modules

test:
	cd $(RUST_DIR)/ && cargo test

clean:
	cd $(RUST_DIR)/ && cargo clean
	rm $(RUST_LIB).o_shipped
	$(MAKE) clean

flash: modules
	scp mfrc522.ko pi@192.168.103.202:~/

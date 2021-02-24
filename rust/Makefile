obj-m += mfrc522_full.o
mfrc522_full-objs := mfrc522.o ./target/armv7-unknown-linux-musleabihf/debug/libmfrc522_rs.a

MAKE = make -C ../linux/ M=$(PWD)

all:
	cargo build --target=armv7-unknown-linux-musleabihf
	$(MAKE) modules

clean:
	cargo clean
	$(MAKE) clean
